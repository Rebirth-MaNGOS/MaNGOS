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
SDName: Boss_Darkmaster_Gandling
SD%Complete: 95
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_ARCANE_MISSILES           = 15791,
    SPELL_SHADOW_SHIELD             = 12040,
    SPELL_CURSE                     = 18702,

    SAY_AGGRO                       = -1289001
};

static Loc Room[]=
{
    {GO_GATE_POLKELT, 250.06f, 0.39f, 85.6f},
    {GO_GATE_THEOLEN, 181.91f, -89.97f, 85.6f},
    {GO_GATE_MALICIA, 95.15f, -1.81f, 85.6f},
    {GO_GATE_ILLUCIA, 250.06f, 0.39f, 72.9f},
    {GO_GATE_ALEXEI, 181.91f, -89.97f, 70.9f},
    {GO_GATE_RAVENIAN, 103.90f,-1.55f, 75.6f}
};

struct MANGOS_DLL_DECL boss_darkmaster_gandlingAI : public ScriptedAI
{
    boss_darkmaster_gandlingAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

    uint32 m_uiArcaneMissilesTimer;
    uint32 m_uiShadowShieldTimer;
    uint32 m_uiCurseTimer;
    uint32 m_uiTeleportTimer;
    uint32 m_uiNo;
    uint32 m_uiSummonCount;

    void Reset()
    {
        m_uiArcaneMissilesTimer = urand(3000,5000);
        m_uiShadowShieldTimer = urand(10000,12000);
        m_uiCurseTimer = urand(6000,8000);
        m_uiTeleportTimer = urand(15000,20000);
        m_uiNo = 0;
        m_uiSummonCount = 0;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DARKMASTER_GANDLING, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DARKMASTER_GANDLING, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DARKMASTER_GANDLING, DONE);
    }

    void JustSummoned(Creature* /*pSummoned*/)
    {
        ++m_uiSummonCount;
    }

    void SummonedCreatureDespawn(Creature* /*pDespawned*/)
    {
        --m_uiSummonCount;
        if (m_uiSummonCount == 0 && m_pInstance)
			m_pInstance->HandleGameObject(Room[m_uiNo].gate, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Arcane Missiles spell
        if (m_uiArcaneMissilesTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANE_MISSILES);
            m_uiArcaneMissilesTimer = urand(4000,6000);
        }
        else
            m_uiArcaneMissilesTimer -= uiDiff;

        // Shadow Shield spell
        if (m_uiShadowShieldTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SHADOW_SHIELD);
            m_uiShadowShieldTimer = urand(30000,40000);
        }
        else
            m_uiShadowShieldTimer -= uiDiff;

        // Curse spell
        if (m_uiCurseTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE);
            m_uiCurseTimer = urand(20000,25000);
        }
        else
            m_uiCurseTimer -= uiDiff;

        //Teleporting Random Target to one of the six pre boss rooms and spawn 3 skeletons near the player.
        //We will only telport if gandling has more than 5% of hp so teleported gamers can always loot and if previsou group was killed.
        if (!HealthBelowPct(5) && m_uiSummonCount == 0)
        {
            if (m_uiTeleportTimer < uiDiff)
            {
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
                {
                    m_uiNo = urand(0,5);
                    if (m_pInstance)
                        m_pInstance->HandleGameObject(Room[m_uiNo].gate, false);

                    if (m_creature->getThreatManager().getThreat(pTarget))
                        m_creature->getThreatManager().modifyThreatPercent(pTarget, -100);

                    DoTeleportPlayer(pTarget, Room[m_uiNo].x, Room[m_uiNo].y, Room[m_uiNo].z, 0);
                    for(uint32 i = 0; i < 3; ++i)
                    {
                        if (Creature* Summoned = m_creature->SummonCreature(NPC_RISEN_GUARDIAN, Room[m_uiNo].x+irand(-5,5), Room[m_uiNo].y+irand(-5,5), Room[m_uiNo].z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000))
                            Summoned->AI()->AttackStart(pTarget);
                    }
                }
                m_uiTeleportTimer = urand(5000,15000);
            }
            else
                m_uiTeleportTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_darkmaster_gandling(Creature* pCreature)
{
    return new boss_darkmaster_gandlingAI(pCreature);
}

void AddSC_boss_darkmaster_gandling()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_darkmaster_gandling";
    pNewscript->GetAI = &GetAI_boss_darkmaster_gandling;
    pNewscript->RegisterSelf();
}
