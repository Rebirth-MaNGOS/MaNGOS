/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Ouro
SD%Complete: 50
SDComment: For emerge, use handleemotestate submerged, but when its removed boss won't turn.
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "temple_of_ahnqiraj.h"
#include "TemporaryGameObject.h"

enum
{
    SPELL_SWEEP             = 26103,
    SPELL_SANDBLAST         = 26102,
    SPELL_GROUND_RUPTURE    = 26100,
    SPELL_BIRTH             = 26262,                        //The Birth Animation
    SPELL_BOULDER           = 26616,
    SPELL_BERSERK           = 26615,
    SPELL_ROOT_SELF         = 23973,

    SPELL_SUMMON_SCARABS    = 26060,
    SPELL_SUMMON_OURO_MOUND = 26058,
    SPELL_SUMMON_OURO       = 26642,

    SPELL_DIRTMOUND_PASSIVE = 26092,//26093
    SPELL_SUBMERGE_VISUAL   = 26063,

    NPC_OURO_SCARAB         = 15718,
    NPC_OURO_SPAWNER        = 15957,
    NPC_OURO_TRIGGER        = 15717,
    NPC_DIRT_MOUND			= 15712,

    GO_SAND_WORM_ROCK_BASE  = 180795,

    BOSS_STATE_NORMAL       = 0,
    BOSS_STATE_SUBMERGE     = 1,
    BOSS_STATE_BOULDER		= 2,

    BIRTH_TIME              = 250 
};

struct MANGOS_DLL_DECL boss_ouroAI : public ScriptedAI
{
    boss_ouroAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        SetCombatMovement(false);
        m_bossState = BOSS_STATE_NORMAL;
        Reset();
    }

    uint32 m_uiSweepTimer;
    uint32 m_uiSandBlastTimer;
    uint32 m_uiSubmergeTimer;
    uint32 m_uiForceSubmergeTimer;
    uint32 m_uiBackTimer;
    uint32 m_uiBirthTimer;
    uint32 m_uiChangeTargetTimer;
    uint32 m_uiSpawnTimer;
    uint32 m_uiBoulderTimer;

    uint8  m_bossState;

    bool m_bForceSubmerge;
    bool m_bEnraged;

    GUIDList m_lDirtMounds;

    ObjectGuid m_WormBase;

    void Reset()
    {
        m_uiForceSubmergeTimer = 5000;
        m_bForceSubmerge = false;
        m_uiSweepTimer = urand(5000, 10000);
        m_uiSandBlastTimer = urand(4000, 8000);
        m_uiSubmergeTimer = 90000;//urand(90000, 150000);		
        m_uiBackTimer = 10000;//urand(30000, 45000);
        m_uiBirthTimer = 0;
        m_uiChangeTargetTimer = urand(5000, 8000);
        m_uiSpawnTimer = urand(10000, 20000);
        m_uiBoulderTimer = 5000;
        m_bEnraged = false;

        m_creature->RemoveAurasDueToSpell(SPELL_ROOT_SELF);
        DespawnWormBase();
        m_creature->SetVisibility(VISIBILITY_OFF);

        if (m_bossState == BOSS_STATE_BOULDER)
            m_bossState = BOSS_STATE_NORMAL;

        m_creature->setFaction(14);

        RemoveDirtMounds(1);			// remove all that would be up after a wipe
        m_lDirtMounds.clear();
    }

    Player* DoGetPlayerInMeleeRangeByThreat()
    {
        std::vector<Player*> tmp_list;

        if (!m_creature->CanHaveThreatList())
            return NULL;

        GUIDVector vGuids;
        m_creature->FillGuidsListFromThreatList(vGuids);

        if (!vGuids.empty())
        {
            for (ObjectGuid current_guid : vGuids)
            {
                if (Unit* current_target = m_creature->GetMap()->GetUnit(current_guid))
                {
                    // We need only a player
                    if (!current_target->IsCharmerOrOwnerPlayerOrPlayerItself())
                        continue;

                    if (m_creature->CanReachWithMeleeAttack(current_target))
                        tmp_list.push_back(dynamic_cast<Player*>(current_target));
                }
            }
        }

        // If there's just one player on the threat list we return that player.
        if (tmp_list.size() == 1)
            return tmp_list.front();

        // Sort the list from highest to lowest threat.
        std::sort(tmp_list.begin(), tmp_list.end(), 
                [&]( Player* first, Player* second) -> bool 
                { 
                    return m_creature->getThreatManager().getThreat(first) > 
                    m_creature->getThreatManager().getThreat(second); 
                });

        return tmp_list.empty() ? NULL : tmp_list.front();
    }

    void Aggro(Unit* /*pWho*/)
    {
        m_creature->SetVisibility(VISIBILITY_ON);
        m_creature->UpdateVisibilityAndView();

        m_uiBirthTimer = BIRTH_TIME;

        SpawnWormBase();

        if(Player* pPlayer = GetPlayerAtMinimumRange(5))
            m_creature->CastSpell(pPlayer, SPELL_GROUND_RUPTURE, true);      

        m_creature->CastSpell(m_creature, SPELL_ROOT_SELF, true);
    }

    void SpawnDirtMound()
    {
        float fX, fY, fZ;
        m_creature->GetPosition(fX, fY, fZ);
        for(uint8 i = 0; i < 2; ++i)
        {
            if(Creature* pDirtMound = m_creature->SummonCreature(NPC_DIRT_MOUND, fX+urand(-5,5),fY+urand(-5,5),fZ, 0,TEMPSUMMON_TIMED_DESPAWN,45000,false))
            {
                pDirtMound->SetRespawnDelay(-10);				// to stop them from randomly respawning
                m_lDirtMounds.push_back(pDirtMound->GetObjectGuid());
            }
        }
    }

    void RemoveDirtMounds(int action = 0)		// just the basics for now
    {
        for (GUIDList::iterator itr = m_lDirtMounds.begin(); itr != m_lDirtMounds.end(); itr++)
        {
            if (Creature* pDirtMound = m_creature->GetMap()->GetCreature(*itr))
                if (pDirtMound->isAlive())
                {
                    if (action == 0)
                    {
                        float fX, fY, fZ;
                        pDirtMound->GetPosition(fX, fY, fZ);
                        for (uint8 i = 0; i < 5; ++i)
                        {
                            if (Creature* pScarab = m_creature->SummonCreature(NPC_OURO_SCARAB, 
                                        fX+urand(-5,5),
                                        fY+urand(-5,5),
                                        fZ, 0,
                                        TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,
                                        15000,false))
                            {
                                if(Player* pPlayer = GetRandomPlayerInCurrentMap(50))
                                    pScarab->AddThreat(pPlayer,100000.0f);
                                pScarab->SetRespawnDelay(-10);				// to stop them from randomly respawning
                            }
                        }	
                        pDirtMound->ForcedDespawn();		// should be instant despawn?
                    }
                    else
                        pDirtMound->ForcedDespawn();		// should be instant despawn?
                }
        }
    }

    void Submerge()
    {           
        // Submerge		
        m_creature->AttackStop();
        if (m_creature->IsNonMeleeSpellCasted(false))
            m_creature->InterruptNonMeleeSpells(false);

        m_creature->HandleEmote(EMOTE_ONESHOT_SUBMERGE);
        m_creature->SetVisibility(VISIBILITY_OFF);

        DespawnWormBase();

        m_creature->RemoveAllAuras(AuraRemoveMode::AURA_REMOVE_BY_DEFAULT);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->setFaction(34); // not quite friendly because it stops combat if he is 35

        m_bossState = BOSS_STATE_SUBMERGE;
        m_uiBackTimer = 10000;//urand(30000, 45000);			// for testing purposes, change back after
        SpawnDirtMound();
    }      

    void DoSubmergeState(uint32 uiDiff)
    {
        // Just call this so that if there's a wipe we reset properly
        m_creature->SelectHostileTarget();

        // Back
        if (m_uiBackTimer < uiDiff)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->setFaction(14);	

            SpawnWormBase();

            m_creature->SetVisibility(VISIBILITY_ON);
            m_creature->UpdateVisibilityAndView();

            m_uiBirthTimer = BIRTH_TIME;

            if(Player* pPlayer = GetPlayerAtMinimumRange(5))
                m_creature->CastSpell(pPlayer, SPELL_GROUND_RUPTURE, true);

            m_bossState = BOSS_STATE_NORMAL;
            m_uiSubmergeTimer = 90000;//urand(60000, 120000);

            RemoveDirtMounds(0);
        }
        else
            m_uiBackTimer -= uiDiff;
    }

    void DoNormalState(uint32 uiDiff)
    {
        if(!m_bEnraged)
        {
            if(m_bForceSubmerge)
            {
                if (m_uiForceSubmergeTimer <= uiDiff)
                {
                    m_uiSubmergeTimer = 1;
                    m_bForceSubmerge = false;
                }
                else
                    m_uiForceSubmergeTimer -= uiDiff;
            }
            // Submerge
            if (m_uiSubmergeTimer <= uiDiff)
            {
                Submerge();
                return;
            }
            else
                m_uiSubmergeTimer -= uiDiff;
        }

        // Sweep
        if (m_uiSweepTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SWEEP);
            m_uiSweepTimer = urand(15000, 30000);
        }
        else
            m_uiSweepTimer -= uiDiff;

        // Sand Blast
        if (m_uiSandBlastTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SANDBLAST);
            m_uiSandBlastTimer = urand(20000, 25000);
        }
        else
            m_uiSandBlastTimer -= uiDiff;

        if (!m_bEnraged)
        {
            if (m_creature->GetHealthPercent() < 21.0f)
            {
                m_creature->CastSpell(m_creature, SPELL_BERSERK, true);
                m_bEnraged = true;
                return;
            }
        }

        if (m_bossState == BOSS_STATE_NORMAL)
        { 
            if (!m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
            {
                if (Player* melee_player = DoGetPlayerInMeleeRangeByThreat())
                {
                    // If there are players in melee range prioritise them.
                    m_creature->SetTargetGuid(melee_player->GetGUID());
                    if (m_creature->getVictim())
                    {
                        this->DoMeleeAttackIfReady();
                        m_bForceSubmerge = false;       // if there's someone in melee don't submerge
                    }
                }
                else
                {
                    if(m_bEnraged) // Cannot find target and enraged - move to boulder mode	
                        m_bossState = BOSS_STATE_BOULDER;
                    else			
                    {
                        if(!m_bForceSubmerge)
                        {
                            m_uiForceSubmergeTimer = 5000;
                            m_bForceSubmerge = true;	
                        }
                    }
                }
            }
            else
            {
                this->DoMeleeAttackIfReady();
                m_bForceSubmerge = false;       // if there's someone in melee don't submerge
            }
        }
        else
        {
            if (m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
                m_bossState = BOSS_STATE_NORMAL;
        }

        if (m_bossState == BOSS_STATE_BOULDER)
        {
            // Boulder
            if (m_uiBoulderTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_BOULDER);
                m_uiBoulderTimer = 10000;
            }
            else
                m_uiBoulderTimer -= uiDiff;
        }
    }

    void SpawnWormBase()
    {
        // To ensure we never get more than one base.
        DespawnWormBase();

        GameObject* pWormBase = m_creature->SummonGameObject(GO_SAND_WORM_ROCK_BASE, 
                0,
                m_creature->GetPositionX(),
                m_creature->GetPositionY(),
                m_creature->GetPositionZ(),
                0, GOState::GO_STATE_READY,
                0);
        if (pWormBase)
            m_WormBase = pWormBase->GetObjectGuid();
    }

    void DespawnWormBase()
    {
        TemporaryGameObject* pWormBase = 
            dynamic_cast<TemporaryGameObject*>(m_creature->GetMap()->GetGameObject(m_WormBase));
        if (pWormBase)
            pWormBase->Delete();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Visual effect for when Ouro emerges.
        if (m_uiBirthTimer)
        {
            if (m_uiBirthTimer <= uiDiff)
            {
                m_creature->CastSpell(m_creature, SPELL_BIRTH, false);

                m_uiBirthTimer = 0;
            }
            else
                m_uiBirthTimer -= uiDiff;

            return;
        }

        if (m_bossState == BOSS_STATE_SUBMERGE)
        {
            DoSubmergeState(uiDiff);
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_bossState)
        {
            case BOSS_STATE_NORMAL:
            case BOSS_STATE_BOULDER:
                DoNormalState(uiDiff);
                break;
        }
    }
};

CreatureAI* GetAI_boss_ouro(Creature* pCreature)
{
    return new boss_ouroAI(pCreature);
}

struct MANGOS_DLL_DECL mob_dirt_moundAI : public ScriptedAI				// should they behave like the clouds on nightmare dragons?
{
    mob_dirt_moundAI(Creature* pCreature) : ScriptedAI(pCreature) 
    { 
        m_creature->CastSpell(m_creature,SPELL_DIRTMOUND_PASSIVE,true);
        Reset(); 
    }

    uint32 m_uiRoamTimer;
    bool m_bCanChangeTarget;
    uint32 m_uiChangeTargetTimer;

    void Reset()
    {
        m_uiRoamTimer = 0;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
    }

    void MoveInLineOfSight(Unit* /*pWho*/)
    {
        // Must to be empty to ignore aggro
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == 26092 && pTarget->GetTypeId() == TYPEID_PLAYER && m_bCanChangeTarget)		
            ChangeTarget();
    }

    void ChangeTarget()
    {
        if (Unit* pTarget = GetRandomPlayerInCurrentMap(50))
        {
            DoResetThreat();
            m_creature->AddThreat(pTarget,100000.0f);
            m_creature->SelectHostileTarget();
            m_uiChangeTargetTimer = 5000;
            m_bCanChangeTarget = false;
            m_uiRoamTimer = urand(20000, 30000);			// Reset the 30 sec timer in case the change is due to hitting a player
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_uiRoamTimer < uiDiff)
        {
            ChangeTarget();
            m_uiRoamTimer = urand(20000, 30000);
        }
        else
            m_uiRoamTimer -= uiDiff;

        if (!m_bCanChangeTarget)
        {
            if (m_uiChangeTargetTimer < uiDiff)
                m_bCanChangeTarget = true;
            else
                m_uiChangeTargetTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_mob_dirt_mound(Creature* pCreature)
{
    return new mob_dirt_moundAI(pCreature);
}

void AddSC_boss_ouro()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_ouro";
    newscript->GetAI = &GetAI_boss_ouro;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dirt_mound";
    newscript->GetAI = &GetAI_mob_dirt_mound;
    newscript->RegisterSelf();
}
