/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Arlokk
SD%Complete: 95
SDComment: Red aura is missing.
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"

enum eArlokk
{
    SAY_AGGRO                 = -1309012,
    SAY_FEAST_PANTHER         = -1309013,
    SAY_DEATH                 = -1309014,

    SPELL_ARLOKK_TRANSFORM    = 24190,
    SPELL_BACKSTAB            = 15582,
    SPELL_GOUGE               = 12540,      //24698,
    SPELL_MARK_OF_ARLOKK      = 24210,
    SPELL_RAVAGE              = 24213,
    SPELL_SHADOW_WORD_PAIN    = 24212,
    SPELL_THRASH              = 3391,
    SPELL_WHIRLWIND           = 15589,
    SPELL_WHIRLWIND_          = 24236,
    SPELL_SNEAK               = 22766,      // Panthers's stealth

    MODEL_ID_NORMAL           = 15218,
    MODEL_ID_PANTHER          = 15215,
    MODEL_ID_BLANK            = 11686,
};

static Loc Cage[]= 
{
    {-11532.79f, -1649.67f, 41.48f, 0},
    {-11532.99f, -1606.48f, 41.29f, 0},
};

struct MANGOS_DLL_DECL boss_arlokkAI : public ScriptedAI
{
    boss_arlokkAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    bool m_bIsPhaseTwo;
    bool m_bIsVanished;

    uint32 m_uiGougeTimer;
    uint32 m_uiMarkTimer;
    uint32 m_uiShadowWordPainTimer;
    uint32 m_uiThrashTimer;
    uint32 m_uiWhirlwindTimer;
    uint32 m_uiWhirlwind_Timer;
    uint32 m_uiVanishTimer;
    uint32 m_uiVisibleTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiSummonCount;

    ObjectGuid m_uiMarkedGUID;
    ObjectGuid m_uiBackstabGUID;

    void Reset()
    {
        m_bIsPhaseTwo = false;
        m_bIsVanished = false;

        m_uiGougeTimer = 14000;
        m_uiMarkTimer = 35000;
        m_uiShadowWordPainTimer = 1000;
        m_uiThrashTimer = 4000;
        m_uiWhirlwindTimer = 4000;
        m_uiWhirlwind_Timer = 8000;
        m_uiVanishTimer = 60000;
        m_uiVisibleTimer = 6000;
        m_uiSummonTimer = 5000;
        m_uiSummonCount = 0;

        m_uiMarkedGUID.Clear();
        m_uiBackstabGUID.Clear();

        m_creature->SetDisplayId(MODEL_ID_NORMAL);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ARLOKK, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ARLOKK, NOT_STARTED);

        // She should be summoned, so despawn.
        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoScriptText(SAY_DEATH, m_creature);

        m_creature->SetDisplayId(MODEL_ID_NORMAL);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ARLOKK, DONE);
    }

    void SummonPhanters()
    {
        m_creature->SummonCreature(NPC_ZULIAN_PROWLER, Cage[0].x, Cage[0].y, Cage[0].z, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
        m_creature->SummonCreature(NPC_ZULIAN_PROWLER, Cage[1].x, Cage[1].y, Cage[1].z, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);

        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiMarkedGUID))
            if (pPlayer->isAlive() && pPlayer->HasAura(SPELL_MARK_OF_ARLOKK))
                DoScriptText(SAY_FEAST_PANTHER, m_creature, pPlayer);
    }

    void JustSummoned(Creature* pSummoned)
    {
        ++m_uiSummonCount;

        // New health for new panther(s)
        uint32 m_uiNewHealth = (pSummoned->GetMaxHealth() + ((m_uiSummonCount < 13 ? 50 : (m_uiSummonCount < 24 ? 200 : (m_uiSummonCount > 24 ? 500 : 0))) * m_uiSummonCount));

        if (m_uiNewHealth > 25000)
            m_uiNewHealth = 25000;

        pSummoned->SetMaxHealth(m_uiNewHealth);
        pSummoned->SetHealth(m_uiNewHealth);

        if (Unit* pUnit = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
        {
            pSummoned->CastSpell(pSummoned, SPELL_SNEAK, true);   // Stealth
            pSummoned->SetSplineFlags(SPLINEFLAG_WALKMODE);
            pSummoned->GetMotionMaster()->MoveFollow(pUnit, 1.0f, 0.0f);
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ZULIAN_PROWLER)
            if (m_uiSummonCount > 0)
                --m_uiSummonCount;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bIsVanished)
        {
            if (!m_bIsPhaseTwo)
            {
                // Mark of Arlokk
                if (m_uiMarkTimer <= uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (Player* pMark = pTarget->GetCharmerOrOwnerPlayerOrPlayerItself())
                        {
                            DoCastSpellIfCan(pMark, SPELL_MARK_OF_ARLOKK);
                            m_uiMarkedGUID = pMark->GetObjectGuid();
                        }
                        else
                        {
                            if (m_uiMarkedGUID)
                                m_uiMarkedGUID.Clear();

                            error_log("SD0: ZG: boss_arlokk could not accuire a new target to mark.");
                        }
                    }

                    m_uiMarkTimer = 15000;
                }
                else
                    m_uiMarkTimer -= uiDiff;

                // Shadow Word: Pain
                if (m_uiShadowWordPainTimer <= uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_WORD_PAIN);
                    m_uiShadowWordPainTimer = 10000;
                }
                else
                    m_uiShadowWordPainTimer -= uiDiff;

                // Gouge (in humanoid form - by wowwiki)
                if (m_uiGougeTimer <= uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_GOUGE);

                    if (m_creature->getThreatManager().getThreat(m_creature->getVictim()))
                        m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(),-80);

                    m_uiGougeTimer = urand(17000, 27000);
                }
                else
                    m_uiGougeTimer -= uiDiff;
            }
            else
            {
                // Thrash
                if (m_uiThrashTimer <= uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_THRASH);
                    m_uiThrashTimer = 5000;
                }
                else
                    m_uiThrashTimer -= uiDiff;

                // Whirlwind
                if (m_uiWhirlwindTimer <= uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND);
                    m_uiWhirlwindTimer = 16000;
                }
                else
                    m_uiWhirlwindTimer -= uiDiff;

                // Whirlwind (+300 damage)
                if (m_uiWhirlwind_Timer <= uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND_);
                    m_uiWhirlwind_Timer = 16000;
                }
                else
                    m_uiWhirlwind_Timer -= uiDiff;
            }
        }

        // Summon Phanters
        if (m_uiSummonCount < 100)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                SummonPhanters();
                m_uiSummonTimer = 5000;
            }
            else
                m_uiSummonTimer -= uiDiff;
        }

        if (m_uiVanishTimer <= uiDiff)
        {
            // Set invisible
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            m_creature->RemoveAllAuras();
            m_creature->AttackStop();
            m_creature->setFaction(35);
            DoResetThreat();
            m_creature->SetVisibility(VISIBILITY_OFF);

            // Acquire a new target for the Backstab ability
            if (Unit* pTarget = SelectUnitWithPower(POWER_MANA))
            {
                m_uiBackstabGUID = pTarget->GetObjectGuid();
                m_creature->GetMotionMaster()->MoveFollow(pTarget, 2.0f, 0);
            }

            m_bIsVanished = true;

            m_uiVanishTimer = 45000;
            m_uiVisibleTimer = 6000;
        }
        else
            m_uiVanishTimer -= uiDiff;

        if (m_bIsVanished)
        {
            if (m_uiVisibleTimer <= uiDiff)
            {
                // The Panther
                if (m_creature->GetDisplayId() != MODEL_ID_PANTHER)
                    m_creature->SetDisplayId(MODEL_ID_PANTHER);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
                m_creature->SetVisibility(VISIBILITY_ON);

                const CreatureInfo *cinfo = m_creature->GetCreatureInfo();
                m_creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 35)));
                m_creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 35)));
                m_creature->UpdateDamagePhysical(BASE_ATTACK);

                // Backstab
                if (m_uiBackstabGUID)
                {
                    Unit* pBackstabUnit = m_creature->GetMap()->GetUnit(m_uiBackstabGUID);
                    
                    if (pBackstabUnit && m_creature->IsWithinDist(pBackstabUnit, ATTACK_DISTANCE) && m_creature->HasInArc((const float)M_PI, pBackstabUnit))
                    {
                        m_uiBackstabGUID.Clear();
                        DoCastSpellIfCan(pBackstabUnit, SPELL_BACKSTAB, CAST_INTERRUPT_PREVIOUS);
                        AttackStart(pBackstabUnit);
                    }
                }

                m_bIsPhaseTwo = true;
                m_bIsVanished = false;
            }
            else
                m_uiVisibleTimer -= uiDiff;
        }
        else
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_arlokk(Creature* pCreature)
{
    return new boss_arlokkAI(pCreature);
}

/*######
## mob_zulian_prowler
######*/

struct MANGOS_DLL_DECL mob_zulian_prowlerAI : public ScriptedAI
{
    mob_zulian_prowlerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    uint32 m_uiCheckTimer;
    uint32 m_uiThrashTimer;

    void Reset()
    {
        m_uiCheckTimer = 1000;
        m_uiThrashTimer = urand(8000, 12000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Attack marked target
        if (m_uiCheckTimer <= uiDiff)
        {
            if (m_pInstance && m_pInstance->GetData(TYPE_ARLOKK) == IN_PROGRESS)
            {
                Map* pMap = m_creature->GetMap();
                if (pMap && pMap->IsDungeon())
                {
                    Map::PlayerList const& lPlayers = m_pInstance->instance->GetPlayers();
                    for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                        if (Player* pPlayer = itr->getSource())
                            if (pPlayer->HasAura(SPELL_MARK_OF_ARLOKK) && m_creature->IsWithinDistInMap(pPlayer, DEFAULT_VISIBILITY_INSTANCE) && pPlayer->IsWithinLOSInMap(m_creature))
                                if (!m_creature->getVictim() || (m_creature->getVictim() && !m_creature->getVictim()->HasAura(SPELL_MARK_OF_ARLOKK)))
                                {
                                    DoResetThreat();
                                    m_creature->AddThreat(pPlayer, 10000.0f);
                                }
                }
            }

            m_uiCheckTimer = 1000;
        }
        else
            m_uiCheckTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Thrash
        if (m_uiThrashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_THRASH);
            m_uiThrashTimer = urand(8000, 12000);
        }
        else
            m_uiThrashTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_zulian_prowler(Creature* pCreature)
{
    return new mob_zulian_prowlerAI(pCreature);
}

/*######
## go_gong_of_bethekk
######*/

bool GOUse_go_gong_of_bethekk(Player* pPlayer, GameObject* pGo)
{
    instance_zulgurub* m_pInstance = (instance_zulgurub*)pGo->GetInstanceData();

    if (m_pInstance && m_pInstance->GetData(TYPE_ARLOKK) == NOT_STARTED)
    {
        if (Creature* pArlokk = pGo->SummonCreature(NPC_ARLOKK, -11531.01f, -1628.36f, 41.29f, 0, TEMPSUMMON_DEAD_DESPAWN, 0))
            pArlokk->AI()->AttackStart(pPlayer);
    }
    return true;        // must be TRUE!
}

void AddSC_boss_arlokk()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_arlokk";
    pNewScript->GetAI = &GetAI_boss_arlokk;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_zulian_prowler";
    pNewScript->GetAI = &GetAI_mob_zulian_prowler;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_gong_of_bethekk";
    pNewScript->pGOUse = &GOUse_go_gong_of_bethekk;
    pNewScript->RegisterSelf();
}
