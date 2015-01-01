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
SDName: Boss_Hakkar
SD%Complete: 95
SDComment:
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"

enum eHakkar
{
    SAY_AGGRO                   = -1309022,
    SAY_FLEEING                 = -1309023,

    SPELL_BERSERK               = 27680,
    SPELL_BLOOD_SIPHON          = 24324,        // doesn't sort targets yet - figure out how to do it in core
    SPELL_BLOOD_SIPHON_DMG      = 24323,
    SPELL_BLOOD_SIPHON_HEAL     = 24322,
    SPELL_CAUSE_INSANITY        = 24327,
    SPELL_CORRUPTED_BLOOD       = 24328,
    SPELL_POISONOUS_BLOOD       = 24321,
    SPELL_WILL_OF_HAKKAR        = 24178,

    SPELL_ASPECT_OF_JEKLIK      = 24687,
    SPELL_ASPECT_OF_VENOXIS     = 24688,
    SPELL_ASPECT_OF_MARLI       = 24686,
    SPELL_ASPECT_OF_THEKAL      = 24689,
    SPELL_ASPECT_OF_ARLOKK      = 24690
};

struct MANGOS_DLL_DECL boss_hakkarAI : public ScriptedAI
{
    boss_hakkarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulgurub*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulgurub* m_pInstance;

    bool m_bEnraged;
    bool m_bJeklikAlive;
    bool m_bVenoxisAlive;
    bool m_bMarliAlive;
    bool m_bThekalAlive;
    bool m_bArlokkAlive;

    uint32 m_uiBerserkTimer;
    uint32 m_uiBloodSiphonTimer;
    uint32 m_uiCauseInsanityTimer;
    uint32 m_uiCorruptedBloodTimer;
    uint32 m_uiWillOfHakkarTimer;

    uint32 m_uiAspectOfJeklikTimer;
    uint32 m_uiAspectOfVenoxisTimer;
    uint32 m_uiAspectOfMarliTimer;
    uint32 m_uiAspectOfThekalTimer;
    uint32 m_uiAspectOfArlokkTimer;

    void Reset()
    {
        m_bEnraged = false;
        m_bJeklikAlive = false;
        m_bVenoxisAlive = false;
        m_bMarliAlive = false;
        m_bThekalAlive = false;
        m_bArlokkAlive = false;

        m_uiBerserkTimer = 600000;
        m_uiBloodSiphonTimer = 90000;
        m_uiCauseInsanityTimer = 10000;
        m_uiCorruptedBloodTimer = 25000;
        m_uiWillOfHakkarTimer = 17000;

        m_uiAspectOfJeklikTimer = 4000;
        m_uiAspectOfVenoxisTimer = 7000;
        m_uiAspectOfMarliTimer = 12000;
        m_uiAspectOfThekalTimer = 8000;
        m_uiAspectOfArlokkTimer = 18000;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_JEKLIK) != DONE)
                m_bJeklikAlive = true;
            if (m_pInstance->GetData(TYPE_VENOXIS) != DONE)
                m_bVenoxisAlive = true;
            if (m_pInstance->GetData(TYPE_MARLI) != DONE)
                m_bMarliAlive = true;
            if (m_pInstance->GetData(TYPE_THEKAL) != DONE)
                m_bThekalAlive = true;
            if (m_pInstance->GetData(TYPE_ARLOKK) != DONE)
                m_bArlokkAlive = true;
        }
    }

    bool BloodSiphon()
    {
        Map* pMap = m_creature->GetMap();

        if (!pMap || !pMap->IsRaid())
            return false;

        Map::PlayerList const& PlayerList = pMap->GetPlayers();

        if (PlayerList.isEmpty())
            return false;

        std::list<Player*> lPlayers;

        for(Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* pPlayer = itr->getSource();

            if (!pPlayer || !pPlayer->isAlive() || pPlayer->GetDistance(m_creature) > 100.0f)
                continue;

            if (!pPlayer->HasAura(SPELL_POISONOUS_BLOOD, EFFECT_INDEX_0))
                lPlayers.push_front(pPlayer);
            else
                lPlayers.push_back(pPlayer);
        }

        if (lPlayers.empty())
            return false;

        for(std::list<Player*>::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            if (!(*itr))
                continue;

            Player* pPlayer = (*itr);

            pPlayer->StopMoving();
            pPlayer->AttackStop();
            pPlayer->InterruptNonMeleeSpells(false);

            if (pPlayer->HasAura(SPELL_POISONOUS_BLOOD, EFFECT_INDEX_0))
            {
                pPlayer->RemoveAurasDueToSpell(SPELL_POISONOUS_BLOOD);
                pPlayer->CastSpell(m_creature, SPELL_BLOOD_SIPHON_DMG, true);
            }
            else
                pPlayer->CastSpell(m_creature, SPELL_BLOOD_SIPHON_HEAL, true);
        }

        return true;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bEnraged && (HealthBelowPct(5) || m_uiBerserkTimer <= uiDiff))
        {
            DoCastSpellIfCan(m_creature, SPELL_BERSERK);
            m_bEnraged = true;
        }
        else
            m_uiBerserkTimer -= uiDiff;

        // Blood Siphon
        if (m_uiBloodSiphonTimer <= uiDiff)
        {
            // Free debuff slots
            for(int i = 0; i < TOTAL_AURAS; ++i)
            {
                Unit::AuraList const& Auras = m_creature->GetAurasByType(AuraType(i));
                for(Unit::AuraList::const_iterator itr = Auras.begin(); itr != Auras.end(); ++itr)
                {
                    SpellEntry const* spell = (*itr)->GetSpellProto();
                    if (!(*itr)->IsPositive())
                    {
                        //m_creature->RemoveAura(*itr);
                        m_creature->RemoveAurasDueToSpell(spell->Id);
                        itr = Auras.begin();
                    }
                }
            }

            // Don't know how to sort players in core by presence of aura yet -> so we make it here
            // If something goes wrong, we cast core effect without sorting players
            if (!BloodSiphon())
                DoCastSpellIfCan(m_creature, SPELL_BLOOD_SIPHON, CAST_INTERRUPT_PREVIOUS | CAST_TRIGGERED);
            else
            {
                int32 spellId = SPELL_BLOOD_SIPHON;
                m_creature->CastCustomSpell(m_creature, SPELL_BLOOD_SIPHON, &spellId, NULL, NULL, true);
            }

            m_uiBloodSiphonTimer = 90000;
        }
        else
            m_uiBloodSiphonTimer -= uiDiff;

        // Corrupted Blood
        if (m_uiCorruptedBloodTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CORRUPTED_BLOOD);
            m_uiCorruptedBloodTimer = urand(15000, 20000);
        }
        else
            m_uiCorruptedBloodTimer -= uiDiff;

        // Cause Insanity
        if (m_uiCauseInsanityTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO,0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_CAUSE_INSANITY);
            m_uiCauseInsanityTimer = urand(10000, 20000);
        }
        else
            m_uiCauseInsanityTimer -= uiDiff;

        // Will Of Hakkar
        if (m_uiWillOfHakkarTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,1);
            if (DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_WILL_OF_HAKKAR) == CAST_OK)
                DoScriptText(SAY_FLEEING, m_creature);

            m_uiWillOfHakkarTimer = urand(25000, 35000);
        }
        else
            m_uiWillOfHakkarTimer -= uiDiff;

        // Aspects of Priests
        if (m_bJeklikAlive)
	{
            if (m_uiAspectOfJeklikTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_ASPECT_OF_JEKLIK);
                m_uiAspectOfJeklikTimer = urand(10000, 14000);
            }
            else
                m_uiAspectOfJeklikTimer -= uiDiff;
	}

        if (m_bVenoxisAlive)
	{
            if (m_uiAspectOfVenoxisTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_ASPECT_OF_VENOXIS);
                m_uiAspectOfVenoxisTimer = 8000;
            }
            else
                m_uiAspectOfVenoxisTimer -= uiDiff;
	}

        if (m_bMarliAlive)
	{
            if (m_uiAspectOfMarliTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_ASPECT_OF_MARLI);
                m_uiAspectOfMarliTimer = 10000;
            }
            else
                m_uiAspectOfMarliTimer -= uiDiff;
	}

        if (m_bThekalAlive)
	{
            if (m_uiAspectOfThekalTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_ASPECT_OF_THEKAL);
                m_uiAspectOfThekalTimer = 15000;
            }
            else
                m_uiAspectOfThekalTimer -= uiDiff;
	}

        if (m_bArlokkAlive)
	{
            if (m_uiAspectOfArlokkTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_ASPECT_OF_ARLOKK);
                DoResetThreat();
                m_uiAspectOfArlokkTimer = urand(10000, 15000);
            }
            else
                m_uiAspectOfArlokkTimer -= uiDiff;
	}

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(uiDiff);
    }
};

CreatureAI* GetAI_boss_hakkar(Creature* pCreature)
{
    return new boss_hakkarAI(pCreature);
}

/*######
## mob_son_of_hakkar
######*/

enum eSonOfHakkar
{
    SPELL_KNOCKDOWN                 = 16790,
    SPELL_THRASH                    = 3391,
    SPELL_SUMMON_POISONOUS_CLOUD    = 24319,
};

struct MANGOS_DLL_DECL mob_son_of_hakkarAI : public ScriptedAI
{
    mob_son_of_hakkarAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiKnockdownTimer;
    uint32 m_uiThrashTimer;

    void Reset()
    {
        m_uiKnockdownTimer = urand(0,2000);
        m_uiThrashTimer = 3000;
    }

    void JustDied(Unit* /*pKiller*/)
    {
        // Summon Poisonous Cloud
        //DoCastSpellIfCan(m_creature, SPELL_SUMMON_POISONOUS_CLOUD, CAST_TRIGGERED);
        DoSpawnCreature(NPC_POISONOUS_CLOUD, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 30000);
    }

    /*void SummonedCreatureDespawn(Creature* pSummoned)
    {
        // Forced respawn.
        if (pSummoned->GetEntry() == NPC_POISONOUS_CLOUD)
            if (m_creature->isDead())
                m_creature->Respawn();
    }
	*/
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Knockdown
        if (m_uiKnockdownTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCKDOWN);
            m_uiKnockdownTimer = urand(7000,11000);
        }
        else
            m_uiKnockdownTimer -= uiDiff;

        // Thrash
        if (m_uiThrashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_THRASH);
            m_uiThrashTimer = urand(2500,7000);
        }
        else
            m_uiThrashTimer -= uiDiff;            

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_son_of_hakkar(Creature* pCreature)
{
    return new mob_son_of_hakkarAI(pCreature);
}

/*######
## mob_poisonous_cloud
######*/

enum ePoisonousCloud
{
    SPELL_POISON_CLOUD      = 23861,
    MODELID_INVISIBLE       = 11686,
};

struct MANGOS_DLL_DECL mob_poisonous_cloudAI : public ScriptedAI
{
    mob_poisonous_cloudAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        pCreature->SetDisplayId(MODELID_INVISIBLE);
        Reset();
    }

    uint32 m_uiPoisonCloudTimer;

    void Reset()
    {
        m_uiPoisonCloudTimer = 0;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho)
            return;

        if (pWho->GetTypeId() != TYPEID_PLAYER || ((Player*)pWho)->isGameMaster())
            return;

        // When player is too close, add him a buff named "Poisonous Blood"
        if (m_creature->IsWithinDistInMap(pWho, 4.0f))
        {
            if (pWho->HasAura(SPELL_POISONOUS_BLOOD, EFFECT_INDEX_0))
            {
                if (SpellAuraHolder* holder = pWho->GetSpellAuraHolder(SPELL_POISONOUS_BLOOD))
                {
                    holder->SetAuraDuration(holder->GetAuraDuration());
                    holder->UpdateAuraDuration();
                }
            }
            else
                pWho->CastSpell(pWho, SPELL_POISONOUS_BLOOD, false);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Poison Cloud (only visual because friendly faction)
        // Creature must to have set faction 35 in the database,
        // else will damage surroundings creatures/players
        if (m_uiPoisonCloudTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_POISON_CLOUD);
            m_uiPoisonCloudTimer = 9000;
        }
        else
            m_uiPoisonCloudTimer -= uiDiff;

        // no meele
    }
};

CreatureAI* GetAI_mob_poisonous_cloud(Creature* pCreature)
{
    return new mob_poisonous_cloudAI(pCreature);
}

void AddSC_boss_hakkar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_hakkar";
    pNewScript->GetAI = &GetAI_boss_hakkar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_son_of_hakkar";
    pNewScript->GetAI = &GetAI_mob_son_of_hakkar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_poisonous_cloud";
    pNewScript->GetAI = &GetAI_mob_poisonous_cloud;
    pNewScript->RegisterSelf();
}
