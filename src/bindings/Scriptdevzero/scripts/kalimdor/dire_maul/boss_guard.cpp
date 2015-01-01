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
SDName: Boss Guard Moldar
SD%Complete: 90
SDComment:
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

#define GOSSIP_GUARD_PLAYER_BUFF "Hetchman, grant me your power!"

enum Gossips
{
    GOSSIP_MOLDAR                   = 6907,
    GOSSIP_MOLDAR_BUFF              = 6908,
    GOSSIP_FENGUS                   = 6903,
    GOSSIP_FENGUS_BUFF              = 6904,
    GOSSIP_SLIPKIK                  = 6905,
    GOSSIP_SLIPKIK_BUFF             = 6906,
};

enum Spells
{
    SPELL_FREEZING_TRAP_EFFECT  = 14309,
    SPELL_FRENZY                = 8269,
    SPELL_KNOCK_AWAY            = 10101,
    SPELL_SHIELD_BASH           = 11972,
    SPELL_SHIELD_CHARGE         = 15749,
    SPELL_STRIKE                = 15580,
    SPELL_MOLDARS_MOXIE         = 22818,
    SPELL_FENGUS_FEROCITY       = 22817,
    SPELL_SLIPKIKS_SAVY         = 22820,
};

struct MANGOS_DLL_DECL boss_guardAI : public ScriptedAI
{
    boss_guardAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
		m_bEvade = false;
		m_bFrozen = false;
        Reset();
    }

    instance_dire_maul* m_pInstance;

    bool m_bCalled;
    bool m_bEvade;
	bool m_bFrozen;

    uint32 m_uiEncounter;
    uint32 m_uiEvadeTimer;
    uint32 m_uiKnockAwayTimer;
    uint32 m_uiShieldBashTimer;
    uint32 m_uiShieldChargeTimer;
    uint32 m_uiStrikeTimer;
	uint32 m_uiUpdateTimer;

    void Reset()
    {
        m_bCalled = false;

        m_uiEvadeTimer = 0;
		m_uiUpdateTimer = 500;
        m_uiKnockAwayTimer = urand(6000,8000);
        m_uiShieldBashTimer = urand(3000,5000);
        m_uiShieldChargeTimer = urand(1000,2000);
        m_uiStrikeTimer = urand(5000,6000);

        switch(m_creature->GetEntry())
        {
            case NPC_GUARD_MOLDAR:
                m_uiEncounter = TYPE_GUARD_MOLDAR;
                break;
            case NPC_GUARD_FENGUS:
                m_uiEncounter = TYPE_GUARD_FENGUS;
                break;
            case NPC_GUARD_SLIPKIK:
                m_uiEncounter = TYPE_GUARD_SLIPKIK;
                break;
        }

		//if (m_bEvade && m_creature->GetEntry() == TYPE_GUARD_SLIPKIK)
		//{
		//	m_bEvade = false;
		//	m_creature->SetOwnerGuid(ObjectGuid());
		//	int32 bp1 = 1000000;
		//	int32 bp2 = 1000000;
		//	int32 bp3 = 1000000;
		//	m_creature->CastCustomSpell(m_creature,27619,&bp1,&bp2,&bp3,false);
		//}
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(m_uiEncounter, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
		m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(m_uiEncounter, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(m_uiEncounter, DONE);
    }

   // void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
   // {
   //     if (m_creature->GetEntry() == NPC_GUARD_SLIPKIK && pSpell->Id == SPELL_FREEZING_TRAP_EFFECT)
   //     {
   //         m_bEvade = true;
   //         //m_uiEvadeTimer = 2000;
			//m_creature->SetOwnerGuid(pCaster->GetObjectGuid());
			//m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			//m_creature->SetMaxPower(POWER_MANA,5000);
			//m_creature->SetPower(POWER_MANA,5000);
			//ResetToHome();
			//GameObject* trap = m_pInstance->GetSingleGameObjectFromStorage(GO_BROKEN_TRAP);
			//if (trap && trap->isSpawned())
			//{
			//	trap->SetGoState(GO_STATE_ACTIVE);
			//	trap->SetLootState(GO_JUST_DEACTIVATED);
			//}
   //     }
   // }

    void UpdateAI(const uint32 uiDiff)
    {
    //    if (m_bEvade)
    //        if (m_uiEvadeTimer <= uiDiff)
    //        {
    //            //m_creature->AI()->ResetToHome();
				//int32 bp = 1000000;
				//m_creature->SetOwnerGuid(ObjectGuid());
				//m_creature->CastCustomSpell(m_creature, 27619, &bp, NULL, NULL, true);
    //            m_bEvade = false;
    //        }
    //        else
    //            m_uiEvadeTimer -= uiDiff;

		if (m_uiEncounter == TYPE_GUARD_SLIPKIK && !m_bFrozen)
		{
			if (m_uiUpdateTimer <= uiDiff)
			{ 
				m_uiUpdateTimer = 500;
				GameObject* trap = m_pInstance->GetSingleGameObjectFromStorage(GO_BROKEN_TRAP);
				if (trap && trap->HasFlag(GAMEOBJECT_FLAGS,GO_FLAG_INTERACT_COND) && m_creature->GetDistance(trap) < 6.0f)
				{
					m_bFrozen = true;

					m_creature->RemoveAllAuras();
					m_creature->DeleteThreatList();
					m_creature->CombatStop(true);
					m_creature->LoadCreatureAddon();

					m_creature->GetMotionMaster()->Clear(false, true);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_creature->addUnitState(UNIT_STAT_CAN_NOT_MOVE);
                    m_creature->addUnitState(UNIT_STAT_ROOT);
				}
			}
			else
				m_uiUpdateTimer -= uiDiff;
		}

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Knock Away
        if (m_uiKnockAwayTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY);
            m_uiKnockAwayTimer = urand(7000,9000);
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        // Shield Bash
        if (m_uiShieldBashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHIELD_BASH);
            m_uiShieldBashTimer = urand(6000,8000);
        }
        else
            m_uiShieldBashTimer -= uiDiff;

        // Shield Charge
        if (m_uiShieldChargeTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_SHIELD_CHARGE);
            m_uiShieldChargeTimer = urand(7000,9000);
        }
        else
            m_uiShieldChargeTimer -= uiDiff;

        // Strike
        if (m_uiStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);
            m_uiStrikeTimer = urand(4000,6000);
        }
        else
            m_uiStrikeTimer -= uiDiff;

        // Call for help - Mol'dar
        if (!m_bCalled && m_creature->GetEntry() == NPC_GUARD_MOLDAR && HealthBelowPct(75))
        {
            m_creature->CallForHelp(DEFAULT_VISIBILITY_INSTANCE);
            m_bCalled = true;
        }

        // Frenzy
        if (HealthBelowPct(40))
            DoCastSpellIfCan(m_creature, SPELL_FRENZY, CAST_AURA_NOT_PRESENT);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_guard(Creature* pCreature)
{
    return new boss_guardAI(pCreature);
}

bool HasHetchmanBuff(Player* pPlayer)
{
    if (pPlayer->HasAura(SPELL_MOLDARS_MOXIE, EFFECT_INDEX_0) || pPlayer->HasAura(SPELL_FENGUS_FEROCITY, EFFECT_INDEX_0) || pPlayer->HasAura(SPELL_SLIPKIKS_SAVY, EFFECT_INDEX_0))
        return true;

    return false;
}

bool GossipHello_boss_guard(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
		pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    switch (pCreature->GetEntry())
    {
        case NPC_GUARD_MOLDAR:
            if (!HasHetchmanBuff(pPlayer))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_GUARD_PLAYER_BUFF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_MOLDAR, pCreature->GetObjectGuid());
            break;
        case NPC_GUARD_FENGUS:
            if (!HasHetchmanBuff(pPlayer))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_GUARD_PLAYER_BUFF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_FENGUS, pCreature->GetObjectGuid());
            break;
        case NPC_GUARD_SLIPKIK:
            if (!HasHetchmanBuff(pPlayer))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_GUARD_PLAYER_BUFF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_SLIPKIK, pCreature->GetObjectGuid());
            break;
    }
    return true;
}

bool GossipSelect_boss_guard(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pCreature->CastSpell(pPlayer, SPELL_MOLDARS_MOXIE, true);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_MOLDAR_BUFF, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pCreature->CastSpell(pPlayer, SPELL_FENGUS_FEROCITY, true);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_FENGUS_BUFF, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pCreature->CastSpell(pPlayer, SPELL_SLIPKIKS_SAVY, true);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_SLIPKIK_BUFF, pCreature->GetObjectGuid());
            break;
    }
    return true;
}

void AddSC_boss_guard()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_guard";
    pNewscript->pGossipHello =  &GossipHello_boss_guard;
    pNewscript->pGossipSelect = &GossipSelect_boss_guard;
    pNewscript->GetAI = &GetAI_boss_guard;
    pNewscript->RegisterSelf();
}
