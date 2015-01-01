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
SDName: Wailing Caverns
SD%Complete: 95
SDComment: Need to add skill usage for Disciple of Naralex
SDCategory: Wailing Caverns
EndScriptData */

/* ContentData
npc_disciple_of_naralex
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "wailing_caverns.h"

/*######
## npc_disciple_of_naralex
######*/

enum eEnums
{
    //say
    SAY_MAKE_PREPARATIONS         = -1043001,
    SAY_TEMPLE_OF_PROMISE         = -1043002,
    SAY_MUST_CONTINUE             = -1043003,
    SAY_BANISH_THE_SPIRITS        = -1043004,
    SAY_CAVERNS_PURIFIED          = -1043005,
    SAY_BEYOND_THIS_CORRIDOR      = -1043006,
    SAY_EMERALD_DREAM             = -1043007,
    SAY_MUTANUS_THE_DEVOURER      = -1043012,
    SAY_NARALEX_AWAKES            = -1043014,
    SAY_THANK_YOU                 = -1043015,
    SAY_FAREWELL                  = -1043016,
    SAY_ATTACKED                  = -1043017,

    //yell
    SAY_AT_LAST                   = -1043000,
    SAY_I_AM_AWAKE                = -1043013,

    //emote
    EMOTE_AWAKENING_RITUAL        = -1043008,
    EMOTE_TROUBLED_SLEEP          = -1043009,
    EMOTE_WRITHE_IN_AGONY         = -1043010,
    EMOTE_HORRENDOUS_VISION       = -1043011,

    //spell
    SPELL_MARK_OF_THE_WILD_RANK_2 = 5232,
    SPELL_SERPENTINE_CLEANSING    = 6270,
    SPELL_NARALEXS_AWAKENING      = 6271,
    SPELL_FLY_FORM                = 8153,
};

#define GOSSIP_ID_START_1       698  //Naralex sleeps again!
#define GOSSIP_ID_START_2       699  //The fanglords are dead!
#define GOSSIP_ITEM_NARALEX     "Let the ritual begin!"

struct MANGOS_DLL_DECL npc_disciple_of_naralexAI : public npc_escortAI
{
    npc_disciple_of_naralexAI(Creature *pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (instance_wailing_caverns*)pCreature->GetInstanceData(); 
        m_uiEventTimer = 0;
        m_uiEventPhase = 0;
        m_uiEventCurrent = 0;
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
    }

    instance_wailing_caverns* m_pInstance;

	uint32 m_uiEventTimer;
    uint32 m_uiEventPhase;
    uint32 m_uiEventCurrent;

    void WaypointReached(uint32 uiPointId)
    {
        if (!m_pInstance)
            return;

        switch (uiPointId)
        {
            case 4:
                m_uiEventPhase = 1;
                m_uiEventTimer = 100;
                m_uiEventCurrent = TYPE_NARALEX_PART1;
                m_pInstance->SetData(TYPE_NARALEX_PART1, IN_PROGRESS);
                break;
            case 5:
                DoScriptText(SAY_MUST_CONTINUE, m_creature);
                m_pInstance->SetData(TYPE_NARALEX_PART1, DONE);
                break;
            case 11:
                m_uiEventPhase = 1;
                m_uiEventTimer = 100;
                m_uiEventCurrent = TYPE_NARALEX_PART2;
                m_pInstance->SetData(TYPE_NARALEX_PART2, IN_PROGRESS);
                break;
            case 19:
                DoScriptText(SAY_BEYOND_THIS_CORRIDOR, m_creature);
                break;
            case 24:
                m_uiEventPhase = 1;
                m_uiEventTimer = 100;
                m_uiEventCurrent = TYPE_NARALEX_PART3;
                m_pInstance->SetData(TYPE_NARALEX_PART3, IN_PROGRESS);
                break;
        }
    }

    void Reset(){}

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_ATTACKED, m_creature, pWho);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_NARALEX_EVENT, FAIL);
            m_pInstance->SetData(TYPE_NARALEX_PART1, FAIL);
            m_pInstance->SetData(TYPE_NARALEX_PART2, FAIL);
            m_pInstance->SetData(TYPE_NARALEX_PART3, FAIL);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->IsHostileTo(m_creature))
            pSummoned->AI()->AttackStart(m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiEventCurrent != TYPE_NARALEX_PART3)
            npc_escortAI::UpdateAI(uiDiff);

        if (!m_pInstance)
            return;

        if (m_uiEventPhase)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                if (m_pInstance->GetData(m_uiEventCurrent) == IN_PROGRESS)
                {
                    switch (m_uiEventCurrent)
                    {
                        case TYPE_NARALEX_PART1:
                            if (m_uiEventPhase == 1)
                            {
								++m_uiEventPhase;
								m_uiEventTimer = 30000;
                                DoScriptText(SAY_TEMPLE_OF_PROMISE, m_creature);
                                m_creature->SummonCreature(NPC_DEVIATE_RAVAGER, -82.1763f, 227.874f, -93.3233f, 0, TEMPSUMMON_DEAD_DESPAWN, 5000);
                                m_creature->SummonCreature(NPC_DEVIATE_RAVAGER, -72.9506f, 216.645f, -93.6756f, 0, TEMPSUMMON_DEAD_DESPAWN, 5000);
								m_uiEventPhase = 0;
                            }
                            break;
                        case TYPE_NARALEX_PART2:
                            if (m_uiEventPhase == 1)
                            {
                                ++m_uiEventPhase;
                                DoScriptText(SAY_BANISH_THE_SPIRITS, m_creature);
                                DoCastSpellIfCan(m_creature, SPELL_SERPENTINE_CLEANSING);
                                m_uiEventTimer = 30000;
                                m_creature->SummonCreature(NPC_DEVIATE_VIPER, -61.5261f, 273.676f, -92.8442f, 0, TEMPSUMMON_DEAD_DESPAWN, 5000);
                                m_creature->SummonCreature(NPC_DEVIATE_VIPER, -58.4658f, 280.799f, -92.8393f, 0, TEMPSUMMON_DEAD_DESPAWN, 5000);
                                m_creature->SummonCreature(NPC_DEVIATE_VIPER, -50.002f,  278.578f, -92.8442f, 0, TEMPSUMMON_DEAD_DESPAWN, 5000);
                            }
                            else
                            if (m_uiEventPhase == 2)
                            {
                                DoScriptText(SAY_CAVERNS_PURIFIED, m_creature);
                                m_pInstance->SetData(TYPE_NARALEX_PART2, DONE);
                                m_creature->InterruptNonMeleeSpells(false);
                                m_creature->RemoveAurasDueToSpell(SPELL_SERPENTINE_CLEANSING);
                                m_creature->RemoveAllAuras();
								m_uiEventPhase = 0;
                            }
                            break;
                        case TYPE_NARALEX_PART3:
                            if (m_uiEventPhase == 1)
                            {
                                ++m_uiEventPhase;
                                m_uiEventTimer = 4000;
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                DoScriptText(SAY_EMERALD_DREAM, m_creature);
                            }
                            else
                            if (m_uiEventPhase == 2)
                            {
                                ++m_uiEventPhase;
                                m_uiEventTimer = 15000;
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                m_creature->InterruptNonMeleeSpells(false);
                                m_creature->RemoveAurasDueToSpell(SPELL_SERPENTINE_CLEANSING);
                                if (Creature* naralex = m_pInstance->GetSingleCreatureFromStorage(NPC_NARALEX))
                                    DoCastSpellIfCan(naralex, SPELL_NARALEXS_AWAKENING, CAST_TRIGGERED);
                                DoScriptText(EMOTE_AWAKENING_RITUAL, m_creature);
                            }
                            else
                            if (m_uiEventPhase == 3)
                            {
                                ++m_uiEventPhase;
                                m_uiEventTimer = 15000;
								if (Creature* naralex = m_pInstance->GetSingleCreatureFromStorage(NPC_NARALEX))
                                    DoScriptText(EMOTE_TROUBLED_SLEEP, naralex);
                                m_creature->SummonCreature(NPC_DEVIATE_MOCCASIN, 135.943f, 199.701f, -103.529f, 0, TEMPSUMMON_DEAD_DESPAWN, 15000);
                                m_creature->SummonCreature(NPC_DEVIATE_MOCCASIN, 151.08f,  221.13f,  -103.609f, 0, TEMPSUMMON_DEAD_DESPAWN, 15000);
                                m_creature->SummonCreature(NPC_DEVIATE_MOCCASIN, 128.007f, 227.428f, -97.421f, 0, TEMPSUMMON_DEAD_DESPAWN, 15000);
                            }
                            else
                            if (m_uiEventPhase == 4)
                            {
                                ++m_uiEventPhase;
                                m_uiEventTimer = 30000;
								if (Creature* naralex = m_pInstance->GetSingleCreatureFromStorage(NPC_NARALEX))
                                    DoScriptText(EMOTE_WRITHE_IN_AGONY, naralex);
                                m_creature->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 133.413f, 207.188f, -102.469f, 0, TEMPSUMMON_DEAD_DESPAWN, 15000);
                                m_creature->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 142.857f, 218.645f, -102.905f, 0, TEMPSUMMON_DEAD_DESPAWN, 15000);
                                m_creature->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 105.102f, 227.211f, -102.752f, 0, TEMPSUMMON_DEAD_DESPAWN, 15000);
                                m_creature->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 153.372f, 235.149f, -102.826f, 0, TEMPSUMMON_DEAD_DESPAWN, 15000);
                                m_creature->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 149.524f, 251.113f, -102.558f, 0, TEMPSUMMON_DEAD_DESPAWN, 15000);
                                m_creature->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 136.208f, 266.466f, -102.977f, 0, TEMPSUMMON_DEAD_DESPAWN, 15000);
                                m_creature->SummonCreature(NPC_NIGHTMARE_ECTOPLASM, 126.167f, 274.759f, -102.962f, 0, TEMPSUMMON_DEAD_DESPAWN, 15000);
                            }
                            else
                            if (m_uiEventPhase == 5)
                            {
                                ++m_uiEventPhase;
                                m_uiEventTimer = 3000;
                                if (Creature* naralex = m_pInstance->GetSingleCreatureFromStorage(NPC_NARALEX))
                                    DoScriptText(EMOTE_HORRENDOUS_VISION, naralex);
                                m_creature->SummonCreature(NPC_MUTANUS_THE_DEVOURER, 150.872f, 262.905f, -103.503f, 0, TEMPSUMMON_DEAD_DESPAWN, 300000);
                                DoScriptText(SAY_MUTANUS_THE_DEVOURER, m_creature);
                                m_pInstance->SetData(TYPE_MUTANUS_THE_DEVOURER, IN_PROGRESS);
                            }
                            else
                            if (m_uiEventPhase == 6 && m_pInstance->GetData(TYPE_MUTANUS_THE_DEVOURER) == DONE)
                            {
								m_creature->MonsterSay("6", 0, 0);
								++m_uiEventPhase;
								m_uiEventTimer = 3000;
								if (Creature* naralex = m_pInstance->GetSingleCreatureFromStorage(NPC_NARALEX))
								{
									m_creature->InterruptNonMeleeSpells(false);
									m_creature->RemoveAurasDueToSpell(SPELL_NARALEXS_AWAKENING);
									naralex->SetStandState(UNIT_STAND_STATE_STAND);
									DoScriptText(SAY_I_AM_AWAKE, naralex);
                                    
								}
								DoScriptText(SAY_NARALEX_AWAKES, m_creature);
                            }
                            else
                            if (m_uiEventPhase == 7)
                            {
                                ++m_uiEventPhase;
                                m_uiEventTimer = 6000;
                                if (Creature* naralex = m_pInstance->GetSingleCreatureFromStorage(NPC_NARALEX))
                                {
                                    naralex->SetFacingToObject(m_creature);
                                    naralex->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                                    DoScriptText(SAY_THANK_YOU, naralex, m_creature);
                                }
                            }
                            else
                            if (m_uiEventPhase == 8)
                            {
                                ++m_uiEventPhase;
                                m_uiEventTimer = 8000;
                                if (Creature* naralex = m_pInstance->GetSingleCreatureFromStorage(NPC_NARALEX))
                                {
                                    DoScriptText(SAY_FAREWELL, naralex);
                                    naralex->CastSpell(naralex, SPELL_FLY_FORM, false);
                                }
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                m_creature->CastSpell(m_creature, SPELL_FLY_FORM, false);
                            }
                            else
                            if (m_uiEventPhase == 9)
                            {
                                ++m_uiEventPhase;
                                m_uiEventTimer = 1500;
                                if (Creature* naralex = m_pInstance->GetSingleCreatureFromStorage(NPC_NARALEX))
                                    naralex->GetMotionMaster()->MovePoint(25, naralex->GetPositionX(), naralex->GetPositionY(), naralex->GetPositionZ());
                            }
                            else
                            if (m_uiEventPhase == 10)
                            {
                                ++m_uiEventPhase;
                                m_uiEventTimer = 1500;
                                if (Creature* naralex = m_pInstance->GetSingleCreatureFromStorage(NPC_NARALEX))
                                {
                                    naralex->GetMotionMaster()->MovePoint(0, 117.095512f, 247.107971f, -96.167870f);
                                    naralex->GetMotionMaster()->MovePoint(1, 90.388809f, 276.135406f, -83.389801f);
                                }
                                m_creature->GetMotionMaster()->MovePoint(26, 117.095512f, 247.107971f, -96.167870f);
                                m_creature->GetMotionMaster()->MovePoint(27, 144.375443f, 281.045837f, -82.477135f);
                            }
                            else
                            if (m_uiEventPhase == 11)
                            {
                                if (Creature* naralex = m_pInstance->GetSingleCreatureFromStorage(NPC_NARALEX))
                                    naralex->SetVisibility(VISIBILITY_OFF);
                                m_creature->SetVisibility(VISIBILITY_OFF);
                                m_pInstance->SetData(TYPE_NARALEX_PART3, DONE);
								m_uiEventPhase = 0;
                            }
                            break;
                    }
                }
            }
            else
                m_uiEventTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_disciple_of_naralex(Creature* pCreature)
{
    return new npc_disciple_of_naralexAI(pCreature);
}

bool GossipHello_npc_disciple_of_naralex(Player* pPlayer, Creature* pCreature)
{
    instance_wailing_caverns* m_pInstance = (instance_wailing_caverns*)pCreature->GetInstanceData();

    if (m_pInstance)
    {
        if (!pPlayer->HasAura(SPELL_MARK_OF_THE_WILD_RANK_2, EFFECT_INDEX_0))
            pCreature->CastSpell(pPlayer, SPELL_MARK_OF_THE_WILD_RANK_2, true);
        if ((m_pInstance->GetData(TYPE_LORD_COBRAHN) == DONE) && (m_pInstance->GetData(TYPE_LORD_PYTHAS) == DONE) &&
            (m_pInstance->GetData(TYPE_LADY_ANACONDRA) == DONE) && (m_pInstance->GetData(TYPE_LORD_SERPENTIS) == DONE))
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NARALEX, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->SEND_GOSSIP_MENU(GOSSIP_ID_START_2, pCreature->GetObjectGuid());

            if (!m_pInstance->GetData(TYPE_NARALEX_YELLED))
            {
                DoScriptText(SAY_AT_LAST, pCreature);
                m_pInstance->SetData(TYPE_NARALEX_YELLED, 1);
            }
        }
        else
        {
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_ID_START_1, pCreature->GetObjectGuid());
        }
    }
    return true;
}

bool GossipSelect_npc_disciple_of_naralex(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    instance_wailing_caverns* m_pInstance = (instance_wailing_caverns*)pCreature->GetInstanceData();

    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NARALEX_EVENT, IN_PROGRESS);

        DoScriptText(SAY_MAKE_PREPARATIONS, pCreature);

        pCreature->setFaction(250);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        
        if (npc_disciple_of_naralexAI* pEscortAI = dynamic_cast<npc_disciple_of_naralexAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer);
    }
    return true;
}

void AddSC_wailing_caverns()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_disciple_of_naralex";
    pNewScript->pGossipHello =  &GossipHello_npc_disciple_of_naralex;
    pNewScript->pGossipSelect = &GossipSelect_npc_disciple_of_naralex;
    pNewScript->GetAI = &GetAI_npc_disciple_of_naralex;
    pNewScript->RegisterSelf();
}
