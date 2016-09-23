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
SDName: Zulfarrak
SD%Complete: 75
SDComment:
SDCategory: Zul'Farrak
EndScriptData */

/* ContentData
npc_sergeant_bly
npc_weegli_blastfuse
npc_raven
npc_oro_eyegouge
npc_murta_grimgut
go_troll_cage
go_gong_of_zulfarrak
go_shallow_grave
go_table_theka
at_zumrah
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "zulfarrak.h"

/*######
## npc_sergeant_bly
######*/

enum eSergeantBly
{
    SPELL_SHIELD_BASH       = 11972,
    SPELL_REVENGE           = 12170,

    SAY_BLY                 = -1209002
};

#define GOSSIP_BLY "[PH] In that case, i will take my reward!"

struct MANGOS_DLL_DECL npc_sergeant_blyAI : public npc_escortAI
{
    npc_sergeant_blyAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_creature->SetAggroRangeOverride(0.3f);
        m_pInstance = (instance_zulfarrak*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulfarrak* m_pInstance;

    uint32 m_uiShieldBash_Timer;
    uint32 m_uiRevenge_Timer;    //this is wrong, spell should never be used unless m_creature->getVictim() dodge, parry or block attack. Mangos support required.
    bool bEscaped, CanWalk;

    void Reset()
    {
        if (bEscaped && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == IN_PROGRESS)
        {
            m_creature->GetMotionMaster()->MovePoint(1000, 1883.12f, 1272.03f, 42.f, true);
            SetEscortPaused(true);
        }

        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;
        m_uiShieldBash_Timer = 5000;
        m_uiRevenge_Timer = 8000;

        m_creature->setFaction(FACTION_FRIENDLY);

        bEscaped = false;
        CanWalk = true;
    }

    void Aggro(Unit* /*pVictim*/)
    {
        m_creature->CallForHelp(40.0f);
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 4:
                CanWalk = false;
                break;
            case 6:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_creature->SetOrientation(1.58f);
                CanWalk = false;
                break;
            case 1000:
                m_creature->GetMotionMaster()->MovePoint(1001, 1883.12f, 1271.03f, 42.f, true);
                break;
            case 1001:
                SetEscortPaused(false);
                break;
        }
    }

    void WaypointStart(uint32 uiPointId)
    {
        if (uiPointId == 5)
        {
            DoScriptText(SAY_BLY, m_creature);
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
    }

    void ChangeCrewFactionToHostile(Unit* pUnit = 0)
    {
		if (Creature* pRaven = m_pInstance->GetSingleCreatureFromStorage(NPC_RAVEN))
        {
            pRaven->InterruptNonMeleeSpells(false);
            pRaven->setFaction(FACTION_HOSTILE);
            if (pUnit)
                pRaven->AI()->AttackStart(pUnit);
        }
        if (Creature* pOro = m_pInstance->GetSingleCreatureFromStorage(NPC_ORO_EYEGOUGE))
        {
            pOro->InterruptNonMeleeSpells(false);
            pOro->setFaction(FACTION_HOSTILE);
            if (pUnit)
                pOro->AI()->AttackStart(pUnit);
        }
        if (Creature* pMurta = m_pInstance->GetSingleCreatureFromStorage(NPC_MURTA_GRIMGUT))
        {
            pMurta->InterruptNonMeleeSpells(false);
            pMurta->setFaction(FACTION_HOSTILE);
            if (pUnit)
                pMurta->AI()->AttackStart(pUnit);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanWalk)
            npc_escortAI::UpdateAI(uiDiff);

        if (!bEscaped && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == IN_PROGRESS)
        {
            bEscaped = true;
            SetIgnorePathing(true);
            Start(true);
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP); // Escort system remove this flag, so we add him back
        }

		if (!CanWalk && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == SPECIAL){
            CanWalk = true;
			SetRun(false); }

        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiShieldBash_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHIELD_BASH);
            m_uiShieldBash_Timer = 15000;
        }
        else m_uiShieldBash_Timer -= uiDiff;

        if (m_uiRevenge_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_REVENGE);
            m_uiRevenge_Timer = 10000;
        }
        else m_uiRevenge_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_sergeant_bly(Creature* pCreature)
{
    return new npc_sergeant_blyAI(pCreature);
}

bool GossipHello_npc_sergeant_bly(Player* pPlayer, Creature* pCreature)
{
    if (instance_zulfarrak* m_pInstance = (instance_zulfarrak*)pCreature->GetInstanceData())
    {
        if (m_pInstance->GetData(TYPE_PYRAMIDE) == DONE)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BLY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
			pPlayer->SEND_GOSSIP_MENU(1517, pCreature->GetObjectGuid());
        }
        else if (m_pInstance->GetData(TYPE_PYRAMIDE) == IN_PROGRESS)
            pPlayer->SEND_GOSSIP_MENU(1516, pCreature->GetObjectGuid());
        else
            pPlayer->SEND_GOSSIP_MENU(1515, pCreature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_npc_sergeant_bly(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->setFaction(FACTION_HOSTILE);
        ((npc_sergeant_blyAI*)pCreature->AI())->AttackStart(pPlayer);
        if (npc_sergeant_blyAI* pCreatureAI = dynamic_cast<npc_sergeant_blyAI*>(pCreature->AI()))
            pCreatureAI->ChangeCrewFactionToHostile(pPlayer);
    }
    return true;
}

/*######
## npc_weegli_blastfuse
######*/

enum eWeegli
{
    SPELL_BOMB             = 8858,
    SPELL_GOBLIN_LAND_MINE = 21688,
    SPELL_SHOOT            = 6660,
    SPELL_WEEGLIS_BARREL   = 10772,

    SAY_WEEGLI             = -1209000,
    SAY_WEEGLI2            = -1209001
};

#define GOSSIP_WEEGLI "[PH] Please blow up the door."

struct MANGOS_DLL_DECL npc_weegli_blastfuseAI : public npc_escortAI
{
    npc_weegli_blastfuseAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_creature->SetAggroRangeOverride(0.3f);
        m_pInstance = (instance_zulfarrak*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulfarrak* m_pInstance;
    uint32 m_uiBomb_Timer;
    uint32 m_uiShoot_Timer;
    uint32 uiLastPointId;
    bool bEscaped, CanWalk;

    void Reset()
    {
        if (bEscaped && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == IN_PROGRESS)
        {
            m_creature->GetMotionMaster()->MovePoint(1000, 1881.48f, 1272.32f, 42.f, true);
            SetEscortPaused(true);
        }

        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_uiBomb_Timer = urand(5000,7000);
        m_uiShoot_Timer = urand(1000,2000);
        uiLastPointId = 0;
        bEscaped = false;
        CanWalk = true;
    }

    void Aggro(Unit *)
    {
        //DoCastSpellIfCan(m_creature, SPELL_GOBLIN_LAND_MINE);
    }

    void WaypointReached(uint32 uiPointId)
    {
        uiLastPointId = uiPointId;
        switch(uiPointId)
        {
            case 3:
                SetRun();
                break;
            case 4:
                DoScriptText(SAY_WEEGLI, m_creature);
                break;
            case 7:
                CanWalk = false;
                m_creature->SetOrientation(4.53f);
                SetRun(false);
                break;
            case 10:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                CanWalk = false;
                //SetEscortPaused(true);
                break;
            case 13:
                if (m_pInstance)
                    m_pInstance->SetData(TYPE_WEEGLI, DONE);
                DoCastSpellIfCan(m_creature, SPELL_WEEGLIS_BARREL, CAST_FORCE_CAST);
                //m_creature->HandleEmote(EMOTE_ONESHOT_KNEEL); // Weegli planting Bomb
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                SetRun();
                break;
            case 15:
                m_creature->SetFacingTo(4.71f);
                break;
            case 1000:
                m_creature->GetMotionMaster()->MovePoint(1001, 1881.48f, 1271.32f, 42.f, true);
                break;
            case 1001:
                SetEscortPaused(false);
                break;
        }
    }

    void WaypointStart(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 8:
                m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
            case 10:
                SetRun();
                break;
            case 14:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
        }
    }

    void ContinueWeegliBombEvent() {CanWalk = true;}

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanWalk)
            npc_escortAI::UpdateAI(uiDiff);

        if (!bEscaped && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == IN_PROGRESS)
        {
            bEscaped = true;
            SetIgnorePathing(true);
            Start(true);
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }

        if (!CanWalk && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == SPECIAL)
			if (uiLastPointId == 7) {
                CanWalk = true;
				SetRun(false); }

        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Bomb timer
        if (m_uiBomb_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BOMB);
            m_uiBomb_Timer = 10000;
        }
        else m_uiBomb_Timer -= uiDiff;

        // Shoot and meele attack
        if (m_creature->GetDistance(m_creature->getVictim()) > 5.0f)
        {
            if (m_uiShoot_Timer <= uiDiff)
            {
                m_creature->SetSheath(SHEATH_STATE_RANGED);
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOOT);
                m_uiShoot_Timer = 2000;
            }
            else m_uiShoot_Timer -= uiDiff;
        }
        else
        {
            m_creature->SetSheath(SHEATH_STATE_MELEE);
            DoMeleeAttackIfReady();
        }
    }
};
CreatureAI* GetAI_npc_weegli_blastfuse(Creature* pCreature)
{
    return new npc_weegli_blastfuseAI(pCreature);
}

bool GossipHello_npc_weegli_blastfuse(Player* pPlayer, Creature* pCreature)
{
    if (instance_zulfarrak* m_pInstance = (instance_zulfarrak*)pCreature->GetInstanceData())
    {
        if (m_pInstance->GetData(TYPE_PYRAMIDE) == DONE && m_pInstance->GetData(TYPE_WEEGLI) == NOT_STARTED)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_WEEGLI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->SEND_GOSSIP_MENU(1514, pCreature->GetObjectGuid()); //if event can proceed to end
        }
        else if (m_pInstance->GetData(TYPE_PYRAMIDE) == IN_PROGRESS)
            pPlayer->SEND_GOSSIP_MENU(1513, pCreature->GetObjectGuid()); //if event are in progress
        else if (m_pInstance->GetData(TYPE_PYRAMIDE) == DONE && m_pInstance->GetData(TYPE_WEEGLI) == DONE)
            pPlayer->SEND_GOSSIP_MENU(1512, pCreature->GetObjectGuid()); //if pyramide event is done and also Weegli's event is done
        else
            pPlayer->SEND_GOSSIP_MENU(1511, pCreature->GetObjectGuid()); //if event not started
    }
    return true;
}

bool GossipSelect_npc_weegli_blastfuse(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        DoScriptText(SAY_WEEGLI2, pCreature);
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        //here we make him run to door, set the charge and run away off to nowhere
        if (npc_weegli_blastfuseAI* pEscortAI = dynamic_cast<npc_weegli_blastfuseAI*>(pCreature->AI()))
            pEscortAI->ContinueWeegliBombEvent();
    }
    return true;
}

/*######
## npc_raven
######*/

enum eRavenZulFarrak
{
    SPELL_DUAL_WIELD = 674,
    SPELL_BACKSTAB = 7159,
    SPELL_GOUGE = 12540
};

struct MANGOS_DLL_DECL npc_ravenAI : public npc_escortAI
{
    npc_ravenAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_creature->SetAggroRangeOverride(0.3f);
        m_pInstance = (instance_zulfarrak*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulfarrak* m_pInstance;
    uint32 m_uiBackstab_Timer;
    uint32 m_uiGouge_Timer;
    bool bEscaped, CanWalk;

    void Reset()
    {
        if (bEscaped && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == IN_PROGRESS)
        {
            m_creature->GetMotionMaster()->MovePoint(1000, 1884.74f, 1272.25f, 42.f, true);
            SetEscortPaused(true);
        }

        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;
        m_uiBackstab_Timer = 3000;
        m_uiGouge_Timer = 5000;
        bEscaped = false;
        CanWalk = true;
    }

    void WaypointReached(uint32 uiPointId)
    {
        if (uiPointId == 4)
            CanWalk = false;
        else if (uiPointId == 7)
            m_creature->SetOrientation(2.27f);
        else if (uiPointId == 1000)
            m_creature->GetMotionMaster()->MovePoint(1001, 1884.74f, 1271.25f, 42.f, true);
        else if (uiPointId == 1001)
            SetEscortPaused(false);

    }

    void Aggro(Unit *)
    {
        DoCastSpellIfCan(m_creature, SPELL_DUAL_WIELD);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanWalk)
            npc_escortAI::UpdateAI(uiDiff);

        if (!bEscaped && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == IN_PROGRESS)
        {
            bEscaped = true;
            SetIgnorePathing(true);
            Start(true);
        }

		if (!CanWalk && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == SPECIAL){
            CanWalk = true;
			SetRun(false); }

        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Backstab timer
        if (m_uiBackstab_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BACKSTAB);
            m_uiBackstab_Timer = 12000;
        }
            else m_uiBackstab_Timer -= uiDiff;

        // Gouge timer
        if (m_uiBackstab_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_GOUGE);
            m_uiBackstab_Timer = 9000;
        }
            else m_uiBackstab_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_raven(Creature* pCreature)
{
    return new npc_ravenAI(pCreature);
}

/*######
## npc_oro_eyegouge
######*/

enum eOroEyegouge
{
    SPELL_CURSE_OF_WEAKNESS = 12741,
    SPELL_IMMOLATE          = 11962,
    SPELL_RAIN_OF_FIRE      = 11990,
    SPELL_SHADOW_BOLT       = 9613
};

struct MANGOS_DLL_DECL npc_oro_eyegougeAI : public npc_escortAI
{
    npc_oro_eyegougeAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_creature->SetAggroRangeOverride(0.3f);
        m_pInstance = (instance_zulfarrak*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulfarrak* m_pInstance;
    uint32 m_uiImmolate_Timer;
    uint32 m_uiRainOfFire_Timer;
    uint32 m_uiShadowBolt_Timer;
    bool bEscaped, CanWalk;

    void Reset()
    {
        if (bEscaped && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == IN_PROGRESS)
        {
            m_creature->GetMotionMaster()->MovePoint(1000, 1886.14f, 1271.83f, 42.f, true);
            SetEscortPaused(true);
        }

        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;
        m_uiImmolate_Timer = urand(5000,7000);
        m_uiRainOfFire_Timer = urand(8000,10000);
        m_uiShadowBolt_Timer = 0;

        bEscaped = false;
        CanWalk = true;
    }

    void WaypointReached(uint32 uiPointId)
    {
        if (uiPointId == 3)
            CanWalk = false;
        else if (uiPointId == 7)
            m_creature->SetOrientation(5.07f);
        else if (uiPointId == 1000)
            m_creature->GetMotionMaster()->MovePoint(1001, 1886.14f, 1270.83f, 42.f, true);
        else if (uiPointId == 1001)
            SetEscortPaused(false);
    }

    void Aggro(Unit* pVictim)
    {
        DoCastSpellIfCan(pVictim, SPELL_CURSE_OF_WEAKNESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanWalk)
            npc_escortAI::UpdateAI(uiDiff);

        if (!bEscaped && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == IN_PROGRESS)
        {
            bEscaped = true;
            SetIgnorePathing(true);
            Start(true);
        }

		if (!CanWalk && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == SPECIAL) {
            CanWalk = true;
			SetRun(false); }

        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Shadow Bolt timer
        if (m_uiShadowBolt_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT);
            m_uiShadowBolt_Timer = 3500;
        }
        else m_uiShadowBolt_Timer -= uiDiff;

        // Immolate timer
        if (m_uiImmolate_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_IMMOLATE);
            m_uiImmolate_Timer = urand(23000,27000);
        }
        else m_uiImmolate_Timer -= uiDiff;

        // Rain of Fire timer
        if (m_uiRainOfFire_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_RAIN_OF_FIRE);
            m_uiRainOfFire_Timer = urand(15000,18250);
        }
        else m_uiRainOfFire_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_oro_eyegouge(Creature* pCreature)
{
    return new npc_oro_eyegougeAI(pCreature);
}

/*######
## npc_murta_grimgut
######*/

enum eMurtaGrimgut
{
    SPELL_HEAL              = 11642,
    SPELL_HOLY_SMITE        = 9734,
    SPELL_POWER_WORD_SHIELD = 11974,
    SPELL_RENEW             = 11640
};

struct MANGOS_DLL_DECL npc_murta_grimgutAI : public npc_escortAI
{
    npc_murta_grimgutAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_creature->SetAggroRangeOverride(0.3f);
        m_pInstance = (instance_zulfarrak*)pCreature->GetInstanceData();
        Reset();
    }

    instance_zulfarrak* m_pInstance;
    uint32 m_uiHeal_Timer;
    uint32 m_uiHolySmite_Timer;
    uint32 m_uiRenew_Timer;
    bool bEscaped, CanWalk;

    void Reset()
    {
        if (bEscaped && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == IN_PROGRESS)
        {
            m_creature->GetMotionMaster()->MovePoint(1000, 1888.70f, 1272.02f, 42.f, true);
            SetEscortPaused(true);
        }

        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;


        m_uiHeal_Timer = urand(2000,5000);
        m_uiHolySmite_Timer = 0;
        m_uiRenew_Timer = 0;
        bEscaped = false;
        CanWalk = true;
    }

    void WaypointReached(uint32 uiPointId)
    {
        if (uiPointId == 4)
            CanWalk = false;
        else if (uiPointId == 1000)
            m_creature->GetMotionMaster()->MovePoint(1001, 1888.70f, 1271.02f, 42.f, true);
        else if (uiPointId == 1001)
            SetEscortPaused(false);
    }

    void Aggro(Unit *)
    {
        DoCastSpellIfCan(m_creature, SPELL_POWER_WORD_SHIELD);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanWalk)
            npc_escortAI::UpdateAI(uiDiff);

        if (!bEscaped && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == IN_PROGRESS)
        {
            bEscaped = true;
            SetIgnorePathing(true);
            Start(true);
        }

		if (!CanWalk && m_pInstance && m_pInstance->GetData(TYPE_PYRAMIDE) == SPECIAL) {
            CanWalk = true;
			SetRun(false); }

        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Heal timer
        if (m_uiHeal_Timer <= uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(40.0f, 41000))
                DoCastSpellIfCan(pTarget, SPELL_HEAL);
            m_uiHeal_Timer = urand(45000,60000);
        }
            else m_uiHeal_Timer -= uiDiff;

        // Holy Smite timer
        if (m_uiHolySmite_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_HOLY_SMITE);
            m_uiHolySmite_Timer = urand(8000,12000);
        }
            else m_uiHolySmite_Timer -= uiDiff;

        // Renew timer
        if (m_uiRenew_Timer <= uiDiff)
        {
            if (Unit* pTarget = DoSelectLowestHpFriendly(40.0f, 2500))
                DoCastSpellIfCan(pTarget, SPELL_RENEW);
            m_uiRenew_Timer = urand(45000,60000);
        }
            else m_uiRenew_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_murta_grimgut(Creature* pCreature)
{
    return new npc_murta_grimgutAI(pCreature);
}

/*######
## go_troll_cage
######*/

bool GOUse_go_troll_cage(Player* /*pPlayer*/, GameObject* pGo)
{
    if (instance_zulfarrak* m_pInstance = (instance_zulfarrak*)pGo->GetInstanceData())
    {
        if (m_pInstance->GetData(TYPE_PYRAMIDE) == NOT_STARTED)
            m_pInstance->SetData(TYPE_PYRAMIDE, IN_PROGRESS);
    }
    return false;
}

/*######
## go_gong_of_zulfarrak
######*/

bool GOUse_go_gong_of_zulfarrak(Player* pPlayer, GameObject* pGo)
{
    instance_zulfarrak* m_pInstance = (instance_zulfarrak*)pGo->GetInstanceData();

    if (m_pInstance && m_pInstance->GetData(TYPE_GAHZRILLA) != DONE)
    {
        if (pPlayer->HasItemCount(ITEM_MALLET_OF_ZULFARRAK, 1, false))
        {
            m_pInstance->SetData(TYPE_GAHZRILLA, DONE);
            pGo->SummonCreature(NPC_GAHZRILLA, 1667.87f, 1190.42f, -2.86999f, 3.92935f, TEMPSUMMON_DEAD_DESPAWN, 30000);
            return true;
        }
    }
    return false;
}

////128403

/*######
## go_shallow_grave
######*/

enum eShallowGrave
{
    NPC_ZOMBIE            = 7286,
    NPC_DEAD_HERO         = 7276,
    ZOMBIE_CHANCE         = 65,
    DEAD_HERO_CHANCE      = 10
};

bool GOUse_go_shallow_grave(Player* /*pPlayer*/, GameObject* pGo)
{
    // randomly summon a zombie or dead hero the first time a grave is used
    if (pGo->GetUseCount() == 0)
    {
        uint32 randomchance = urand(0,100);
        if (randomchance < ZOMBIE_CHANCE)
            pGo->SummonCreature(NPC_ZOMBIE, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
        else if ((randomchance-=ZOMBIE_CHANCE) < DEAD_HERO_CHANCE)
            pGo->SummonCreature(NPC_DEAD_HERO, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
    }
    pGo->AddUse();
    return false;
}

/*######
## boss_theka_the_martyr
######*/

enum eThekaTheMartyr
{
    SPELL_FEVERED_PLAGUE  = 8600,
    SPELL_THEKA_TRANSFORM = 11089,

    NPC_SCARAB            = 7269
};

struct MANGOS_DLL_DECL boss_theka_the_martyrAI : public ScriptedAI
{
    boss_theka_the_martyrAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiFeveredPlagueTimer;
    uint32 m_uiAggroScarabsTimer;
    bool bImmuned;

    void Reset()
    {
        m_uiFeveredPlagueTimer = 2000;
        m_uiAggroScarabsTimer = 10000;
        bImmuned = false;
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        // If Theka's new health will be 30 percent or lesser, cast Theka Transform
        if (!bImmuned && ((((m_creature->GetHealth()-uiDamage)*100))/m_creature->GetMaxHealth()) <= 30)
        {
            uiDamage = 0;
            DoCastSpellIfCan(m_creature, SPELL_THEKA_TRANSFORM, CAST_FORCE_CAST);
            bImmuned = true;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Aggro Scarabs
        if (bImmuned && m_creature->HasAura(SPELL_THEKA_TRANSFORM))
        {
            if (m_uiAggroScarabsTimer < uiDiff)
            {
                std::list<Creature*> lScarabs;
                GetCreatureListWithEntryInGrid(lScarabs, m_creature, NPC_SCARAB, 30.0f);        // these radius is probably wrong
                uint8 uiScarabsAggroed = 0;

                if (!lScarabs.empty())
                {
                    for(std::list<Creature*>::iterator itr = lScarabs.begin(); itr != lScarabs.end(); ++itr)
                        if ((*itr) && (*itr)->isAlive() && !(*itr)->getVictim() && (*itr)->IsWithinLOSInMap(m_creature))
                        {
                            (*itr)->AI()->AttackStart(m_creature->getVictim());
                            if (++uiScarabsAggroed == 5)
                                break;
                        }
                }
                else
                    debug_log("SD0: Zul'Farrak: Scarab list for aggro of boss Theka The Martyr is empty. Skipping..");

                m_uiAggroScarabsTimer = 15000;
            }
            else
                m_uiAggroScarabsTimer -= uiDiff;
        }

        // Fevered Plague
        if (m_uiFeveredPlagueTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_FEVERED_PLAGUE);
            m_uiFeveredPlagueTimer = 10000;
        }
        else
            m_uiFeveredPlagueTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_theka_the_martyr(Creature* pCreature)
{
    return new boss_theka_the_martyrAI(pCreature);
}

/*######
## go_table_theka
######*/

enum eTableTheka
{
    GOSSIP_TABLE_THEKA = 1653,
    QUEST_SPIDER_GOD = 2936
};

bool GOUse_go_table_theka(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_SPIDER_GOD) == QUEST_STATUS_INCOMPLETE)
        pPlayer->AreaExploredOrEventHappens(QUEST_SPIDER_GOD);

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TABLE_THEKA, pGo->GetObjectGuid());

    return true;
}

/*######
## at_zumrah
######*/

enum eZumrah
{
    NPC_ZUMRAH                = 7271,
    ZUMRAH_HOSTILE_FACTION    = 37
};

/*######
## boss_gahz_rilla
######
##SQL##
update creature_template set ScriptName = "boss_gahz_rilla"
where entry = 7273;
*/

enum GAHZ_RILLA
{
	SPELL_EISZAPFEN				= 11131,
	SPELL_SCHMETTERSCHLAG		= 11902,
	SPELL_EINFRIEREN			= 11836
};

struct MANGOS_DLL_DECL boss_gahz_rillaAI : public ScriptedAI
{
    boss_gahz_rillaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulfarrak*)pCreature->GetInstanceData();
        Reset();
    }

	 instance_zulfarrak* m_pInstance;

	 uint32 zapfen_timer;
	 uint32 schmetter_timer;
	 uint32 eis_timer;

    void Reset()
    {
		zapfen_timer = urand(5000,10000);
		schmetter_timer = urand(25000,40000);
		eis_timer = urand(12000,25000);
    }

	void UpdateAI(const uint32 uiDiff)
    {
        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (zapfen_timer < uiDiff)
		{
			if(DoCastSpellIfCan(m_creature->getVictim(),SPELL_EISZAPFEN) == CAST_OK)
				zapfen_timer = urand(5000,10000);
		}
		else
			zapfen_timer -= uiDiff;

		if (schmetter_timer < uiDiff)
		{
			if(DoCastSpellIfCan(m_creature,SPELL_SCHMETTERSCHLAG) == CAST_OK)
				schmetter_timer = urand(25000,40000);
		}
		else
			schmetter_timer -= uiDiff;

		if (eis_timer < uiDiff)
		{
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
			{
				if(DoCastSpellIfCan(pTarget,SPELL_EINFRIEREN) == CAST_OK)
					eis_timer = urand(12000,25000);
			}
		}
		else
			eis_timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gahz_rilla(Creature* pCreature)
{
    return new boss_gahz_rillaAI(pCreature);
}

bool AreaTrigger_at_zumrah(Player* pPlayer, AreaTriggerEntry const* /*pAt*/)
{
    Creature* Zumrah = GetClosestCreatureWithEntry(pPlayer, NPC_ZUMRAH, 30);

    if (!Zumrah)
        return false;

    Zumrah->setFaction(ZUMRAH_HOSTILE_FACTION);
    Zumrah->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    Zumrah->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    return true;
}

void AddSC_zulfarrak()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_sergeant_bly";
    pNewscript->GetAI = &GetAI_npc_sergeant_bly;
    pNewscript->pGossipHello =  &GossipHello_npc_sergeant_bly;
    pNewscript->pGossipSelect = &GossipSelect_npc_sergeant_bly;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_weegli_blastfuse";
    pNewscript->GetAI = &GetAI_npc_weegli_blastfuse;
    pNewscript->pGossipHello =  &GossipHello_npc_weegli_blastfuse;
    pNewscript->pGossipSelect = &GossipSelect_npc_weegli_blastfuse;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_raven";
    pNewscript->GetAI = &GetAI_npc_raven;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_oro_eyegouge";
    pNewscript->GetAI = &GetAI_npc_oro_eyegouge;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_murta_grimgut";
    pNewscript->GetAI = &GetAI_npc_murta_grimgut;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_troll_cage";
    pNewscript->pGOUse = &GOUse_go_troll_cage;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_gong_of_zulfarrak";
    pNewscript->pGOUse = &GOUse_go_gong_of_zulfarrak;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_shallow_grave";
    pNewscript->pGOUse = &GOUse_go_shallow_grave;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "boss_theka_the_martyr";
    pNewscript->GetAI = &GetAI_boss_theka_the_martyr;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_table_theka";
    pNewscript->pGOUse = &GOUse_go_table_theka;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "at_zumrah";
    pNewscript->pAreaTrigger = &AreaTrigger_at_zumrah;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "boss_gahz_rilla";
    pNewscript->GetAI = &GetAI_boss_gahz_rilla;
    pNewscript->RegisterSelf();
}
