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
SDName: Boss_Hazzarah
SD%Complete: 100
SDComment:
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"

enum eHazzarah
{
    SPELL_MANA_BURN             = 26046,
    SPELL_SLEEP                 = 24664,
    SUMMON_NIGHTMARE_ILLUSION   = 24729,
    SUMMON_NIGHTMARE_ILLUSION_  = 24681,

    MODELID_FELGUARD            = 5049,
    MODELID_ABOMINATION         = 1693,
    MODELID_LASHER              = 8172,
    MODELID_DEVILSAUR           = 5240,
};

struct MANGOS_DLL_DECL boss_hazzarahAI : public ScriptedAI
{
    boss_hazzarahAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiManaBurnTimer;
    uint32 m_uiSleepTimer;
    uint32 m_uiIllusionsTimer;

    void Reset()
    {
        m_uiManaBurnTimer = urand(4000, 10000);
        m_uiSleepTimer = urand(10000, 18000);
        m_uiIllusionsTimer = urand(10000, 18000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mana Burn
        if (m_uiManaBurnTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MANA_BURN);
            m_uiManaBurnTimer = urand(8000, 16000);
        }
        else
            m_uiManaBurnTimer -= uiDiff;

        // Sleep
        if (m_uiSleepTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SLEEP);
            m_uiSleepTimer = urand(12000, 20000);
        }
        else
            m_uiSleepTimer -= uiDiff;

        // Illusions
        if (m_uiIllusionsTimer <= uiDiff)
        {
            //We will summon 3 illusions that will spawn on a random player and attack this player
            for(uint8 i = 0; i < 3; ++i)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (Creature* pIllusion = m_creature->SummonCreature(NPC_NIGHTMARE_ILLUSION, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                    {
                        // Summoned Illusion will to have a random model
                        uint32 m_uiNewDisplayId = 0;
                        switch(rand()%4)
                        {
                            case 0: m_uiNewDisplayId = MODELID_ABOMINATION; break;
                            case 1: m_uiNewDisplayId = MODELID_LASHER; break;
                            case 2: m_uiNewDisplayId = MODELID_FELGUARD; break;
                            case 3: m_uiNewDisplayId = MODELID_DEVILSAUR; break;
                        }

                        pIllusion->SetDisplayId(m_uiNewDisplayId);
                        //pIllusion->SetFloatValue(OBJECT_FIELD_SCALE_X, 3.0f);   // bigger model
                        pIllusion->AI()->AttackStart(pTarget);
                    }
            }
            m_uiIllusionsTimer = urand(15000, 25000);
        }
        else
            m_uiIllusionsTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_hazzarah(Creature* pCreature)
{
    return new boss_hazzarahAI(pCreature);
}

void AddSC_boss_hazzarah()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_hazzarah";
    pNewScript->GetAI = &GetAI_boss_hazzarah;
    pNewScript->RegisterSelf();
}
