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
SDName: Boss_Rend_Blackhand
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

enum Says
{
    SAY_NEFARIAN_1        = -1229001,
    SAY_NEFARIAN_2        = -1229002,
    SAY_NEFARIAN_3        = -1229003,
    SAY_REND_1            = -1229004,
    SAY_NEFARIAN_4        = -1229005,
    SAY_NEFARIAN_5        = -1229006,
    SAY_NEFARIAN_6        = -1229007,
    SAY_REND_2            = -1229008,
    SAY_REND_3            = -1229009,
    SAY_NEFARIAN_7        = -1229010,
    SAY_NEFARIAN_8        = -1229011,
    SAY_REND_4            = -1229012,
    SAY_NEFARIAN_9        = -1229013,
    SAY_NEFARIAN_10       = -1229014,
};

enum Spells
{
    SPELL_CLEAVE          = 15284,
    SPELL_FRENZY          = 8269,
    SPELL_MORTAL_STRIKE   = 15708,
    SPELL_WHIRLWIND       = 15589
};

static uint32 auiWave[8][5] =
{
    {NPC_CHROMATIC_DRAGONSPAWN, NPC_CHROMATIC_WHELP, NPC_CHROMATIC_WHELP, NPC_CHROMATIC_WHELP, 0},
    {NPC_CHROMATIC_DRAGONSPAWN, NPC_CHROMATIC_WHELP, NPC_CHROMATIC_WHELP, NPC_CHROMATIC_WHELP, 0},
    {NPC_FIRE_TONGUE, NPC_CHROMATIC_WHELP, NPC_CHROMATIC_WHELP, NPC_BLACKHAND_DRAGON_HANDLER, 0},
    {NPC_FIRE_TONGUE, NPC_CHROMATIC_WHELP, NPC_CHROMATIC_WHELP, NPC_BLACKHAND_DRAGON_HANDLER, 0},
    {NPC_FIRE_TONGUE, NPC_CHROMATIC_WHELP, NPC_CHROMATIC_WHELP, NPC_CHROMATIC_WHELP, NPC_BLACKHAND_DRAGON_HANDLER},
    {NPC_CHROMATIC_DRAGONSPAWN, NPC_FIRE_TONGUE, NPC_CHROMATIC_WHELP, NPC_CHROMATIC_WHELP, NPC_BLACKHAND_DRAGON_HANDLER},
    {NPC_CHROMATIC_DRAGONSPAWN, NPC_FIRE_TONGUE, NPC_CHROMATIC_WHELP, NPC_CHROMATIC_WHELP, NPC_BLACKHAND_DRAGON_HANDLER},
    {NPC_GYTH, 0, 0, 0, 0}
};

static Loc aWaveMove[]=
{
    {133.73f,-409.99f,110.86f},
    {132.43f,-431.69f,110.86f},
    {144.51f,-420.32f,110.47f},
    {148.66f,-432.76f,110.86f},
    {146.56f,-406.03f,110.86f}
};

struct MANGOS_DLL_DECL boss_rend_blackhandAI : public ScriptedAI
{
    boss_rend_blackhandAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_spire*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_spire* m_pInstance;

    bool m_bEvent;
    bool m_bEventReset;
    bool m_bEventResetConfirmed;
    bool m_bPortcullis;

    uint32 m_uiEventPhase;
    uint32 m_uiEventTimer;
    uint32 m_uiEventResetTimer;
    uint32 m_uiPortcullisTimer;
    uint32 m_uiSummonCount;

    uint32 m_uiCleaveTimer;
    uint32 m_uiMortalStrike;
    uint32 m_uiWhirlwindTimer;

    GUIDList m_uiSummonList;

    void Reset()
    {
        m_bEvent = false;
        m_bEventReset = false;
        m_bEventResetConfirmed = false;
        m_bPortcullis = false;

        m_uiEventPhase = 0;
        m_uiEventTimer = 0;
        m_uiEventResetTimer = 0;
        m_uiPortcullisTimer = 0;
        m_uiSummonCount = 0;

        m_uiCleaveTimer = urand(2000,4000);
        m_uiMortalStrike = urand(8000,10000);
        m_uiWhirlwindTimer = urand(14000,16000);

        m_uiSummonList.clear();
    }

    void JustReachedHome()
    {
        m_creature->RemoveFromWorld();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_REND_BLACKHAND, NOT_STARTED);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_REND_BLACKHAND, DONE);
    }

    void HandleYell(int64 iYell, bool bOpen = true)
    {
        if (m_pInstance)
        {
            if (Creature* pNefarius = m_pInstance->GetSingleCreatureFromStorage(NPC_LORD_VICTOR_NEFARIUS))
                DoScriptText(iYell, pNefarius);

            if (bOpen)
            {
                if (GameObject* pPort = m_pInstance->GetSingleGameObjectFromStorage(GO_STADIUM_PORTCULLIS))
                    pPort->SetGoState(GO_STATE_ACTIVE);

                m_uiPortcullisTimer = 5500;
                m_bPortcullis = true;
            }
        }
    }

    void HandleSummon(uint32 aWave[])
    {
        for(uint32 i = 0; i < 5; ++i)
        {
            if (aWave[i] == 0)
                break;

            if (Creature* pSummoned = m_creature->SummonCreature(aWave[i], 210.02f, -420.45f, 110.94f, 3.14f, TEMPSUMMON_DEAD_DESPAWN, 120000))
            {
                pSummoned->GetMotionMaster()->MovePoint(0, aWaveMove[i].x, aWaveMove[i].y, aWaveMove[i].z);
                CreatureCreatePos pos(pSummoned->GetMap(), aWaveMove[i].x, aWaveMove[i].y, aWaveMove[i].z, 0);
                pSummoned->SetSummonPoint(pos);
            }

            m_uiEventResetTimer = 50000;
            m_bEventReset = true;
        }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId)
    {
        if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 0)
        {
            m_uiEventTimer = 2000;
            m_bEvent = true;
            m_creature->SetOrientation(1.56f);
            if (m_pInstance)
                if (Creature* pNefarius = m_pInstance->GetSingleCreatureFromStorage(NPC_LORD_VICTOR_NEFARIUS))
                    pNefarius->SetOrientation(1.56f);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        ++m_uiSummonCount;
        m_uiSummonList.push_front(pSummoned->GetObjectGuid());
        pSummoned->SetInCombatWithZone();

        if (pSummoned->GetEntry() == NPC_GYTH)
            m_creature->RemoveFromWorld();
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId)
    {
        if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 0)
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                pSummoned->AI()->AttackStart(pTarget);
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        --m_uiSummonCount;
        m_uiSummonList.remove(pSummoned->GetObjectGuid());

        if (m_uiSummonCount == 0 && m_uiEventTimer > 5000)
            m_uiEventTimer = 5000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bEvent)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                switch(++m_uiEventPhase)
                {
                    case 1:
                        HandleSummon(auiWave[0]);
                        HandleYell(SAY_NEFARIAN_1);
                        break;
                    case 2:
                        HandleSummon(auiWave[1]); 
                        HandleYell(SAY_NEFARIAN_2);
                        break;
                    case 3:
                        HandleSummon(auiWave[2]);
                        HandleYell(SAY_NEFARIAN_3);
                        DoScriptText(SAY_REND_1, m_creature);
                        break;
                    case 4:
                        HandleSummon(auiWave[3]);
                        HandleYell(SAY_NEFARIAN_4);
                        break;
                    case 5:
                        HandleSummon(auiWave[4]);
                        HandleYell(SAY_NEFARIAN_5);
                        break;
                    case 6:
                        HandleSummon(auiWave[5]);
                        HandleYell(SAY_NEFARIAN_6);
                        DoScriptText(SAY_REND_2, m_creature);
                        break;
                    case 7:
                        HandleSummon(auiWave[6]);
                        DoScriptText(SAY_REND_3, m_creature);
                        HandleYell(SAY_NEFARIAN_7);
                        break;
                    case 8:
                        HandleYell(SAY_NEFARIAN_8, false);
                        break;
                    case 9:
                        DoScriptText(SAY_REND_4, m_creature);
                        break;
                    case 10:
                        HandleYell(SAY_NEFARIAN_9, false);
                        m_creature->SetVisibility(VISIBILITY_OFF);
                        break;
                    case 11:
                        HandleSummon(auiWave[7]);
                        HandleYell(SAY_NEFARIAN_10);
                        break;
                    default:
                        break;
                }

                if (m_uiEventPhase < 8)
                    m_uiEventTimer = 60000;
                else if (m_uiEventPhase < 10)
                    m_uiEventTimer = 3500;
                else if (m_uiEventPhase < 11)
                    m_uiEventTimer = 55000;
                else
                    m_bEvent = false;
            }
            else
                m_uiEventTimer -= uiDiff;
        }

        if (m_bEventReset)
        {
            if (m_uiEventResetTimer <= uiDiff)
            {
                for(GUIDList::iterator itr = m_uiSummonList.begin(); itr != m_uiSummonList.end(); ++itr)
                    if (Creature* pSummon = m_creature->GetMap()->GetCreature(*itr))
                        if (pSummon->isAlive() && !pSummon->isInCombat())
                        {
                            m_bEventResetConfirmed = true;
                            break;
                        }

                if (m_bEventResetConfirmed)
                { 
                    for(GUIDList::iterator itr = m_uiSummonList.begin(); itr != m_uiSummonList.end(); ++itr)
                        if (Creature* pSummon = m_creature->GetMap()->GetCreature(*itr))
                            pSummon->RemoveFromWorld();

                    if (m_pInstance)
                        m_pInstance->SetData(TYPE_REND_BLACKHAND, NOT_STARTED);

                    Reset();
                    m_creature->RemoveFromWorld();
                }
                m_bEventReset = false;
            }
            else
                m_uiEventResetTimer -= uiDiff;
        }

        if (m_bPortcullis)
        {
            if (m_uiPortcullisTimer <= uiDiff)
            {
                if (m_pInstance)
                    if (GameObject* pPort = m_pInstance->GetSingleGameObjectFromStorage(GO_STADIUM_PORTCULLIS))
                        pPort->SetGoState(GO_STATE_READY);

                m_bPortcullis = false;
            }
            else
                m_uiPortcullisTimer -= uiDiff;
        }

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Frenzy
        if (HealthBelowPct(33))
            DoCastSpellIfCan(m_creature, SPELL_FRENZY, CAST_AURA_NOT_PRESENT);

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(5000,9000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Mortal Strike
        if (m_uiMortalStrike <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
            m_uiMortalStrike = urand(6000,12000);
        }
        else
            m_uiMortalStrike -= uiDiff;

        // Whirlwind
        if (m_uiWhirlwindTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND);
            m_uiWhirlwindTimer = urand(12000,16000);
        }
        else
            m_uiWhirlwindTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_rend_blackhand(Creature* pCreature)
{
    return new boss_rend_blackhandAI(pCreature);
}

void AddSC_boss_rend_blackhand()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_rend_blackhand";
    pNewscript->GetAI = &GetAI_boss_rend_blackhand;
    pNewscript->RegisterSelf();
}
