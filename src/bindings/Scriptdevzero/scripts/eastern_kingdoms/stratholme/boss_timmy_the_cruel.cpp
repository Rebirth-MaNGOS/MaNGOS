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
SDName: boss_timmy_the_cruel
SD%Complete: 100
SDComment: Timmy the Cruel AI + bastion's AT
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

enum eTimmy
{
    SAY_SPAWN_TIMMY         = -1329012,

    SPELL_ENRAGE            = 8599,
    SPELL_RAVENOUS_CLAW     = 17470
};

static Locations Timmy[]=
{
    {3613.19f,-3188.20f,131.61f, 0, 0},
    {3669.12f,-3186.15f,126.19f, 0, 0}
};

const uint32 CrimsonEntries[4]= {NPC_CRIMSON_INITIATE, NPC_CRIMSON_CONJUROR, NPC_CRIMSON_GALLANT, NPC_CRIMSON_GUARDSMAN};

struct MANGOS_DLL_DECL boss_timmy_the_cruelAI : public ScriptedAI
{
    boss_timmy_the_cruelAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    uint32 m_uiEnrageTimer;
    uint32 m_uiRavenousClawTimer;

    void Reset()
    {
        m_uiEnrageTimer = 0;
        m_uiRavenousClawTimer = urand(3000,6000);
    }

    void JustReachedHome()
    {
        m_creature->RemoveFromWorld();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_TIMMY_THE_CRUEL, FAIL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_TIMMY_THE_CRUEL, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Enrage spell
        if (HealthBelowPct(50))
        {
            if (m_uiEnrageTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_ENRAGE);
                m_uiEnrageTimer = 240000;
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        // Ravenous Claw spell
        if (m_uiRavenousClawTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_RAVENOUS_CLAW);
            m_uiRavenousClawTimer = urand(6000,10000);
        }
        else
            m_uiRavenousClawTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_timmy_the_cruel(Creature* pCreature)
{
    return new boss_timmy_the_cruelAI(pCreature);
}

bool AreaTrigger_at_bastion(Player* pPlayer, AreaTriggerEntry const* /*pAt*/)
{
    instance_stratholme* m_pInstance = (instance_stratholme*)pPlayer->GetInstanceData();

    if (!m_pInstance || m_pInstance->GetData(TYPE_TIMMY_THE_CRUEL) != NOT_STARTED)
        return false;

    for (uint32 i = 0; i < 4; ++i)
    {
        std::list<Creature*> m_lScarlet;
        GetCreatureListWithEntryInGrid(m_lScarlet, pPlayer, CrimsonEntries[i], 70.0f);
        if (!m_lScarlet.empty())
        {
            for(std::list<Creature*>::iterator itr = m_lScarlet.begin(); itr != m_lScarlet.end(); ++itr)
            {
                if ((*itr) && (*itr)->isAlive())
                    return false;
            }
        }
    }

    m_pInstance->SetData(TYPE_TIMMY_THE_CRUEL, IN_PROGRESS);
    if (Creature* pTimmy = pPlayer->SummonCreature(NPC_TIMMY_THE_CRUEL, Timmy[0].x, Timmy[0].y, Timmy[0].z, 0, TEMPSUMMON_DEAD_DESPAWN, 5000))
    {
        DoScriptText(SAY_SPAWN_TIMMY, pTimmy);
        pTimmy->GetMotionMaster()->MovePoint(0, Timmy[1].x, Timmy[1].y, Timmy[1].z);
    }

    return true;
}

void AddSC_boss_timmy_the_cruel()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_timmy_the_cruel";
    pNewscript->GetAI = &GetAI_boss_timmy_the_cruel;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "at_bastion";
    pNewscript->pAreaTrigger = &AreaTrigger_at_bastion;
    pNewscript->RegisterSelf();
}
