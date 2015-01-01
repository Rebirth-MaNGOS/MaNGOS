/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Boss_Ayamiss
SD%Complete: 90
SDComment:
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "ruins_of_ahnqiraj.h"

enum eAyamiss
{
    EMOTE_FRENZY                = -1000002,

    // Ayamiss
    SPELL_FRENZY                = 8269,
    SPELL_LASH                  = 25852,
    SPELL_PARALYZE              = 25725,
    SPELL_POISON_STINGER        = 25748,
    SPELL_STINGER_SPRAY         = 25749,
    SPELL_TRASH                 = 3391,

    // Hive'Zara Larva
    SPELL_FEED                  = 25721,
};

static Loc Larva[]=
{
    {-9695.0f, 1585.0f, 25.0f, 0, 0},
    {-9627.0f, 1538.0f, 21.44f, 0, 0}
};

static Loc Swarmers[]=
{
    {-9650.0f, 1577.0f, 47.0f, 0, 0}
};

struct MANGOS_DLL_DECL boss_ayamissAI : public ScriptedAI
{
    boss_ayamissAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData());
        Reset();
    }

    instance_ruins_of_ahnqiraj* m_pInstance;

    bool m_bPhaseTwo;

    uint32 m_uiPoisonStingerTimer;
    uint32 m_uiStingerSprayTimer;
    uint32 m_uiParalyzeTimer;
    uint32 m_uiLashTimer;
    uint32 m_uiTrashTimer;
    uint32 m_uiFrenzyTimer;
    uint32 m_uiSwarmerTimer;

    ObjectGuid m_uiLarvaTargetGUID;

    void Reset()
    {
        m_bPhaseTwo = true;

        m_uiPoisonStingerTimer = urand(5000,9000);
        m_uiStingerSprayTimer = urand(6000,8000);
        m_uiParalyzeTimer = urand(35000,45000);
        m_uiLashTimer = urand(7000,9000);
        m_uiTrashTimer = urand(6000,8000);
        m_uiFrenzyTimer = 0;
        m_uiSwarmerTimer = urand(20000,30000);

		m_uiLarvaTargetGUID.Clear();

        SetCombatMovement(false);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AYAMISS, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AYAMISS, IN_PROGRESS);

        m_creature->SendMonsterMove(m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ()+20.0f, SPLINETYPE_NORMAL, SPLINEFLAG_UNKNOWN7, 3000);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AYAMISS, DONE);
    }

    void JustSummoned(Creature* pSummoned)
    {
        switch(pSummoned->GetEntry())
        {
            case NPC_HIVEZARA_LARVA:
                if (Unit* pLarvaTarget = m_creature->GetMap()->GetUnit(m_uiLarvaTargetGUID))
                    pSummoned->AI()->AttackStart(pLarvaTarget);
                break;
            case NPC_HIVEZARA_SWARMER:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummoned->AI()->AttackStart(pTarget);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If Ayamiss is 70% let's take him down :-) Phase 2 on the ground
        if (!m_bPhaseTwo && HealthBelowPct(70))
        {
            SetCombatMovement(true);
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            m_creature->AttackerStateUpdate(m_creature->getVictim(), BASE_ATTACK, true);
            m_bPhaseTwo = true;
        }

        // Summon Swarmers
        if (m_uiSwarmerTimer <= uiDiff)
        {
            for (uint32 i = 0; i < 10; ++i)
                m_creature->SummonCreature(NPC_HIVEZARA_SWARMER, Swarmers[0].x + irand(-10,10), Swarmers[0].y + irand(-10,10), Swarmers[0].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);

            m_uiSwarmerTimer = urand(20000,30000);
        }
        else
            m_uiSwarmerTimer -= uiDiff;

        // Paralyze and Summon Larva
        if (m_uiParalyzeTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_PARALYZE);
                uint32 i = urand(0,1);
                m_creature->SummonCreature(NPC_HIVEZARA_LARVA, Larva[i].x, Larva[i].y, Larva[i].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
				m_uiLarvaTargetGUID = pTarget->GetObjectGuid();
            }
            m_uiParalyzeTimer = urand(15000,20000);
        }
        else
            m_uiParalyzeTimer -= uiDiff;

        if (!m_bPhaseTwo)
        {
            // Poison Stinger
            if (m_uiPoisonStingerTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_POISON_STINGER);
                m_uiPoisonStingerTimer = urand(7000,9000);
            }
            else
                m_uiPoisonStingerTimer -= uiDiff;
        }
        else
        {
            // Frenzy spell
            if (HealthBelowPct(20))
                if (DoCastSpellIfCan(m_creature, SPELL_FRENZY, CAST_AURA_NOT_PRESENT))
                    DoScriptText(EMOTE_FRENZY, m_creature);

            // Lash
            if (m_uiLashTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_LASH);
                m_uiLashTimer = urand(7000,9000);
            }
            else
                m_uiLashTimer -= uiDiff;

            // Stinger Spray
            if (m_uiStingerSprayTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_STINGER_SPRAY);
                m_uiStingerSprayTimer = urand(8000,12000);
            }
            else
                m_uiStingerSprayTimer -= uiDiff;

            // Trash
            if (m_uiTrashTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_TRASH);
                m_uiTrashTimer = urand(6000,8000);
            }
            else
                m_uiTrashTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ayamiss(Creature* pCreature)
{
    return new boss_ayamissAI(pCreature);
}

struct MANGOS_DLL_DECL mob_hivezara_larvaAI : public ScriptedAI
{
    mob_hivezara_larvaAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    ObjectGuid m_uiTargetGUID;

    void Reset() 
    {
		m_uiTargetGUID.Clear();
    }

    void Aggro(Unit* pWho)
    {
		m_uiTargetGUID = pWho->GetObjectGuid();
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Unit* pTarget = m_creature->GetMap()->GetUnit(m_uiTargetGUID))
            if (m_creature->IsWithinDistInMap(pTarget, ATTACK_DISTANCE))
                if (m_creature->getVictim()->HasAura(SPELL_PARALYZE))
                    DoCastSpellIfCan(pTarget, SPELL_FEED);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_hivezara_larva(Creature* pCreature)
{
    return new mob_hivezara_larvaAI(pCreature);
}

void AddSC_boss_ayamiss()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_ayamiss";
    pNewscript->GetAI = &GetAI_boss_ayamiss;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_hivezara_larva";
    pNewscript->GetAI = &GetAI_mob_hivezara_larva;
    pNewscript->RegisterSelf();
}
