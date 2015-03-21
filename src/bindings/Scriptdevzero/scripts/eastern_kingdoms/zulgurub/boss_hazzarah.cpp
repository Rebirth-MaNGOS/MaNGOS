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
	MODELID_BARTHILAS			= 10433,
};

static Loc Summon[9]=
{
    { -11912.7f, -1892.4f, 65.14f, 0.f },// end corner
	{ -11907.29f, -1896.57f, 65.14f, 0.f },
	{ -11901.59f, -1915.91f, 65.14f, 0.f },
	{ -11888.51f, -1911.85f, 65.14f, 0.f },
	{ -11897.6f, -1901.6, 65.14f, 0.f },
    { -11879.3f, -1920.56f, 65.14f, 0.f },
	{ -11916.45f, -1902.36f, 65.14f, 0.f },
	{ -11892.12f, -1920.99f, 65.14f, 0.f },
	{ -11879.3f, -1920.56f, 65.14f, 0.f },//end corner
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
        if (m_uiIllusionsTimer <= uiDiff)			// get the positions within the platform, and random between those because movemaps are buggy.
        {
            //We will summon 3 illusions that will spawn on a random player and attack this player
            for (uint8 i = 0; i < 3; ++i)
            {
                float x, y, z;
                short attempts = 0;
                float radius = frand(0, 10);

                // Make 20 attempts at finding a correct spawn position.
                while (attempts < 20)
                {
                    float angle = frand(0, 2 * PI);
                    
                    x = m_creature->GetPositionX() + radius * cosf(angle);
                    y = m_creature->GetPositionY() + radius * sinf(angle);
                    z = m_creature->GetPositionZ();

                    if (m_creature->IsWithinLOS(x, y, z))
                        break;

                    ++attempts;
                }

                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    Creature* pIllusion = m_creature->SummonCreature(NPC_NIGHTMARE_ILLUSION, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
					if (pIllusion)
                    {
                        // Summoned Illusion will to have a random model
                        uint32 m_uiNewDisplayId = 0;
                        switch (rand() % 5)
                        {
                            case 0: m_uiNewDisplayId = MODELID_ABOMINATION;
								pIllusion->SetFloatValue(OBJECT_FIELD_SCALE_X, 3.0f);
								break;
                            case 1: m_uiNewDisplayId = MODELID_LASHER;
								pIllusion->SetFloatValue(OBJECT_FIELD_SCALE_X, 3.0f);
								break;
                            case 2: m_uiNewDisplayId = MODELID_FELGUARD;
								pIllusion->SetFloatValue(OBJECT_FIELD_SCALE_X, 3.0f);
								break;
                            case 3: m_uiNewDisplayId = MODELID_DEVILSAUR; 
								pIllusion->SetFloatValue(OBJECT_FIELD_SCALE_X, 3.0f);
								break;
								case 4: m_uiNewDisplayId = MODELID_BARTHILAS; 
								pIllusion->SetFloatValue(OBJECT_FIELD_SCALE_X, 3.0f);
								break;
                        }

                        pIllusion->SetDisplayId(m_uiNewDisplayId);
                       //pIllusion->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.0f);   // bigger model
                        pIllusion->AI()->AttackStart(pTarget);
                    }
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
