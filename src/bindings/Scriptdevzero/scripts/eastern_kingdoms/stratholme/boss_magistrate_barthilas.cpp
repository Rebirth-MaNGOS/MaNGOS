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
SDName: Boss_Magistrate_Barthilas
SD%Complete: 90
SDComment:
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

enum
{
    SPELL_CROWD_PUMMEL        = 10887,
    SPELL_DRAINING_BLOW       = 16793,
    SPELL_FURIOUS_ANGER       = 16791,
    SPELL_MIGHTY_BLOW         = 14099,

    MODEL_NORMAL              = 10433,
    MODEL_HUMAN               = 3637
};

struct MANGOS_DLL_DECL boss_magistrate_barthilasAI : public ScriptedAI
{
    boss_magistrate_barthilasAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }
	
    instance_stratholme* m_pInstance;

    uint32 m_uiAngerCount;
    uint32 m_uiCrowdPummelTimer;
    uint32 m_uiDrainingBlowTimer;
    uint32 m_uiFuriousAngerTimer;
    uint32 m_uiMightyBlowTimer;   

    void Reset()
    {
        m_uiCrowdPummelTimer = urand(12000,14000);
        m_uiDrainingBlowTimer = urand(17000,20000);
        m_uiFuriousAngerTimer = urand(1000,4000);
        m_uiMightyBlowTimer = urand(7000,9000);
        m_uiAngerCount = 0;

        if (m_creature->isAlive())
            m_creature->SetDisplayId(MODEL_NORMAL);
        else
            m_creature->SetDisplayId(MODEL_HUMAN);

		GameObject* pDoor = GetClosestGameObjectWithEntry(m_creature, GO_PORT_GAUNTLET, 100.f);
		    if (pDoor && pDoor->IsInRange(m_creature, 0, 100))
		    {
			    m_creature->GetMotionMaster()->MovePoint(0, 4065.45f, -3532.95f, 122.35f);
			  //  m_creature->SetFacingTo(2.64f);			not working well
		    }
    }

    void MovementInform(uint32 /*uiMotionType*/, uint32 uiPointId)
    {
		if (uiPointId == 3)
        {
            if (m_pInstance)
            {
                //m_pInstance->SetData(TYPE_BARON_RUN, IN_PROGRESS);
                m_pInstance->SetData(TYPE_MAGISTRATE, DONE);
            }
			m_creature->NearTeleportTo(4065.45f, -3532.95f, 122.35f, 2.64f);
			m_creature->GetMotionMaster()->MovementExpired();
		}
    }

    void JustDied(Unit* /*pKiller*/)
    {
        m_creature->SetDisplayId(MODEL_HUMAN);
        if (m_pInstance && m_pInstance->GetData(TYPE_MAGISTRATE) != DONE)
            m_pInstance->SetData(TYPE_MAGISTRATE, FAIL);
    }

    void UpdateAI(const uint32 uiDiff)
    {   
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Crowd Pummel spell
        if (m_uiCrowdPummelTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CROWD_PUMMEL);
            m_uiCrowdPummelTimer = urand(12000,16000);
        }
        else
            m_uiCrowdPummelTimer -= uiDiff;

        // Draining Blow spell
        if (m_uiDrainingBlowTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DRAINING_BLOW);
            m_uiDrainingBlowTimer = urand(10000,15000);
        }
        else
            m_uiDrainingBlowTimer -= uiDiff;

        // Furious Anger spell
        if (m_uiFuriousAngerTimer <= uiDiff)
        {
            m_uiFuriousAngerTimer = urand(4000,5000);
            if (m_uiAngerCount > 25)
                return;

            ++m_uiAngerCount;
            DoCastSpellIfCan(m_creature, SPELL_FURIOUS_ANGER);
        }
        else
            m_uiFuriousAngerTimer -= uiDiff;

        // Mighty Blow spell
        if (m_uiMightyBlowTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIGHTY_BLOW);
			{
				m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), -25);				//added threat reduction
				m_uiMightyBlowTimer = urand(8000,12000);	
			}
        }
        else
            m_uiMightyBlowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_magistrate_barthilas(Creature* pCreature)
{
    return new boss_magistrate_barthilasAI(pCreature);
}

void AddSC_boss_magistrate_barthilas()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_magistrate_barthilas";
    pNewscript->GetAI = &GetAI_boss_magistrate_barthilas;
    pNewscript->RegisterSelf();
}
