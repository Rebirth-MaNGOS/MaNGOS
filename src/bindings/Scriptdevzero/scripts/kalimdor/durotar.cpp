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
SDName: Durotar
SD%Complete: 100
SDComment: Quest support: 5441
SDCategory: Durotar
EndScriptData */

/* ContentData
npc_lazy_peon
EndContentData */

#include "precompiled.h"

/*######
## npc_lazy_peon
######*/

enum LazyPeon
{
    SAY_WORK              = -1000159,

    GO_LUMBERPILE         = 175784,
    QUEST_LAZY_PEONS      = 5441,

    SPELL_BUFF_SLEEP      = 17743,
    SPELL_AWAKEN_PEON     = 19938,

	WORKING_TIME          = 60000
};

struct npc_lazy_peonAI : public ScriptedAI
{
    npc_lazy_peonAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool m_bWorking;
    uint32 m_uiRebuffTimer;
	uint32 m_uiUpdateTimer;

    void Reset()
    {
        m_bWorking = false;
        m_uiRebuffTimer = 0;
		m_uiUpdateTimer = 1000;
		m_creature->CastSpell(m_creature,SPELL_BUFF_SLEEP,false);
    }

	bool IsMoving()
	{
		return m_creature->GetMotionMaster() && (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE
				|| m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == HOME_MOTION_TYPE);
	}

    void JustReachedHome()
    {
        //m_creature->LoadCreatureAddon();
		m_creature->CastSpell(m_creature,SPELL_BUFF_SLEEP,false);
    }

	//void MoveInLineOfSight(Unit* U)
	//{
	//	if (!m_bWorking && !m_creature->HasAura(SPELL_BUFF_SLEEP,EFFECT_INDEX_0) && !IsMoving())
	//		m_creature->CastSpell(m_creature,SPELL_BUFF_SLEEP,false);
	//}

    void MovementInform(uint32 /*uiType*/, uint32 uiPointId)
    {
        if (uiPointId == 1)
            m_bWorking = true;
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (m_bWorking || pCaster->GetTypeId() != TYPEID_PLAYER)
            return;

        if (pSpell->Id == SPELL_AWAKEN_PEON && ((Player*)pCaster)->GetQuestStatus(QUEST_LAZY_PEONS) == QUEST_STATUS_INCOMPLETE)
        {
			((Player*)pCaster)->KilledMonsterCredit(m_creature->GetEntry(), m_creature->GetObjectGuid());
            DoScriptText(SAY_WORK, m_creature, pCaster);
            m_creature->RemoveAllAuras();
            m_uiRebuffTimer = WORKING_TIME;
            if (GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_LUMBERPILE, 20.0f))
                m_creature->GetMotionMaster()->MovePoint(1, pGo->GetPositionX()-1, pGo->GetPositionY(), pGo->GetPositionZ());
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bWorking)
        {
			m_creature->HandleEmote(173);
			if (m_uiRebuffTimer > WORKING_TIME)
				m_uiRebuffTimer = 10000;
            if (m_uiRebuffTimer <= uiDiff)
            {
                m_creature->GetMotionMaster()->MoveTargetedHome();
                m_bWorking = false;
                m_uiRebuffTimer = 0;
            }
            else
                m_uiRebuffTimer -= uiDiff;
        }
		else 
		{
			if (m_uiUpdateTimer <= uiDiff)
			{
				m_uiUpdateTimer = 1000;
				if (!m_creature->HasAura(SPELL_BUFF_SLEEP,EFFECT_INDEX_0) && !IsMoving())
					m_creature->CastSpell(m_creature,SPELL_BUFF_SLEEP,false);
			}
			else
				m_uiUpdateTimer -= uiDiff;
		}

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_lazy_peon(Creature* pCreature)
{
    return new npc_lazy_peonAI(pCreature);
}

void AddSC_durotar()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_lazy_peon";
    pNewscript->GetAI = &GetAI_npc_lazy_peon;
    pNewscript->RegisterSelf();
}
