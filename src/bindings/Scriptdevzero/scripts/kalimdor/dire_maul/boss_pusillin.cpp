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
SDName: Boss Pusillin
SD%Complete:
SDComment: TODO, make Imps on Pusillin Chase Event attack when they spawn.
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

#define GOSSIP_PUSILLIN_PLAYER_1 "A Game? Are you crazy?"
#define GOSSIP_PUSILLIN_PLAYER_2 "Why you little..."
#define GOSSIP_PUSILLIN_PLAYER_3 "Mark my words, I will catch you, imp. And when I do!"
#define GOSSIP_PUSILLIN_PLAYER_4 "DIE!"
#define GOSSIP_PUSILLIN_PLAYER_5 "Prepare to meet your maker."

enum ePusillin
{
    GOSSIP_PUSILLIN_1         = 6877,
    GOSSIP_PUSILLIN_2         = 6878,
    GOSSIP_PUSILLIN_3         = 6879,
    GOSSIP_PUSILLIN_4         = 6880,
    GOSSIP_PUSILLIN_5         = 6881,

    SAY_PUSILLIN_1            = -1429000,
    SAY_PUSILLIN_2            = -1429001,
    SAY_PUSILLIN_3            = -1429002,
    SAY_PUSILLIN_4            = -1429003,
    SAY_PUSILLIN_5            = -1429004,

    SPELL_BLAST_WAVE          = 22424,
    SPELL_FIRE_BLAST          = 14145,
    SPELL_FIREBALL            = 15228,
    SPELL_SPIRIT_OF_RUNN_TUM  = 22735,
};

static Loc Move[]=
{
   // {-167.80f,-198.62f,-4.15f,0},
    {-132.68f,-344.60f,-4.03f,0},			//1st corner
	{-149.8f,-278.3f,-4.14f,0},				//2nd gossip spot
	{-143.68f,-354.0f,-4.12f,0},				//Corner
	{113.72f,-363.17f,-4.15f,0},			//3rd gossip spot
	{110.88f,-536.06f,-11.07f,0},				//1st corner moving to lower floor
	{86.25f,-546.15f,-13.02f,0},				//2nd corner
	{59.61f,-551.01f,-17.41f,0},				//3rd corner
	{49.43f,-570.39f,-20.29f,0},				//4th corner
	{51.12f,-633.68f,-25.12f,0},			// 4th gossip spot
	{34.63f,-689.64f,-25.16f,0},				//1st corner on bottom floor
	{2.09f,-698.57f,-25.16f,0},					//2nd corner
	{7.25f,-666.43f,-12.64f,0},					//1st on boss floor
	{17.52f,-701.77f,-12.64,0},					//2nd on boss floor
	{5.53f,-710.65f,-12.64f,0},					//in corner to make boss turn around
	{6.95f,-709.65f,-12.64f,0},				//5th gossip spot
	{7.95f,-708.65f,-12.64f,0},					//Spawn boss spot
};

struct MANGOS_DLL_DECL boss_pusillinAI : public ScriptedAI
{
    boss_pusillinAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    uint32 m_uiBlastWaveTimer;
    uint32 m_uiFireBlastTimer;
    uint32 m_uiFireballTimer;

    void Reset()
    {
        m_uiBlastWaveTimer = urand(3000,4000);
        m_uiFireBlastTimer = urand(4000,6000);
        m_uiFireballTimer = urand(6000,7000);

        m_creature->setFaction(FACTION_FRIENDLY);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_PUSILLIN, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
    }
	
    void MovementInform(uint32 /*uiMotionType*/, uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 1:
				 m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				break;
			case 2:
				m_creature->GetMotionMaster()->MovePoint(3, Move[3].x, Move[3].y, Move[3].z);
				return;
			case 3:
				 m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				break;
			case 4:
				m_creature->GetMotionMaster()->MovePoint(5, Move[5].x, Move[5].y, Move[5].z);
				return;
			case 5:
				m_creature->GetMotionMaster()->MovePoint(6, Move[6].x, Move[6].y, Move[6].z);
				return;
			case 6:
				m_creature->GetMotionMaster()->MovePoint(7, Move[7].x, Move[7].y, Move[7].z);
				return;
			case 7:
				m_creature->GetMotionMaster()->MovePoint(8, Move[8].x, Move[8].y, Move[8].z);
				return;
			case 8:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				break;
			case 9:
				m_creature->GetMotionMaster()->MovePoint(10, Move[10].x, Move[10].y, Move[10].z, false);		//Doesn't use MMaps
				return;
			case 10:
				m_creature->GetMotionMaster()->MovePoint(11, Move[11].x, Move[11].y, Move[11].z);
				return;
			case 11:
				m_creature->GetMotionMaster()->MovePoint(12, Move[12].x, Move[12].y, Move[12].z);
				return;
			case 12:
				m_creature->GetMotionMaster()->MovePoint(13, Move[13].x, Move[13].y, Move[13].z);
				return;
			case 13:
				m_creature->GetMotionMaster()->MovePoint(14, Move[14].x, Move[14].y, Move[14].z);
				break;
			case 14:
				m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				break;
            case 15:
                m_creature->CastSpell(m_creature, SPELL_SPIRIT_OF_RUNN_TUM, false);
                m_creature->setFaction(FACTION_HOSTILE);
                m_creature->SetInCombatWithZone();
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Blast Wave
        if (m_uiBlastWaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLAST_WAVE);
            m_uiBlastWaveTimer = urand(6000,8000);
        }
        else
            m_uiBlastWaveTimer -= uiDiff;

        // Fire Blast
        if (m_uiFireBlastTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIRE_BLAST);
            m_uiFireBlastTimer = urand(7000,10000);
        }
        else
            m_uiFireBlastTimer -= uiDiff;

        // Fireball
        if (m_uiFireballTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL);
            m_uiFireballTimer = urand(5000,6000);
        }
        else
            m_uiFireballTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_pusillin(Creature* pCreature)
{
    return new boss_pusillinAI(pCreature);
}

bool GossipHello_boss_pusillin(Player* pPlayer, Creature* pCreature)
{
    if (instance_dire_maul* m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData())
    {
        switch (m_pInstance->GetData(TYPE_PUSILLIN))
        {
            case 0:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PUSILLIN_PLAYER_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
				pPlayer->SEND_GOSSIP_MENU(GOSSIP_PUSILLIN_1, pCreature->GetObjectGuid());
                break;
            case 1:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PUSILLIN_PLAYER_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_PUSILLIN_2, pCreature->GetObjectGuid());
                break;
            case 2:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PUSILLIN_PLAYER_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_PUSILLIN_3, pCreature->GetObjectGuid());
                break;
            case 3:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PUSILLIN_PLAYER_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_PUSILLIN_4, pCreature->GetObjectGuid());
                break;
            case 4:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PUSILLIN_PLAYER_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_PUSILLIN_5, pCreature->GetObjectGuid());
                break;
            default:
                pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
                break;
        }
    }
    return true;
}

bool GossipSelect_boss_pusillin(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (instance_dire_maul* m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData())
    {
        m_pInstance->SetData(TYPE_PUSILLIN, m_pInstance->GetData(TYPE_PUSILLIN) + 1);
        pCreature->UpdateSpeed(MOVE_RUN, true);
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                DoScriptText(SAY_PUSILLIN_1, pCreature);
                pCreature->GetMotionMaster()->MovePoint(1, Move[1].x, Move[1].y, Move[1].z);
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                DoScriptText(SAY_PUSILLIN_2, pCreature);
                pCreature->GetMotionMaster()->MovePoint(2, Move[2].x, Move[2].y, Move[2].z);
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                DoScriptText(SAY_PUSILLIN_3, pCreature);
                pCreature->GetMotionMaster()->MovePoint(4, Move[4].x, Move[4].y, Move[4].z);
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                DoScriptText(SAY_PUSILLIN_4, pCreature);
                pCreature->GetMotionMaster()->MovePoint(9, Move[9].x, Move[9].y, Move[9].z);
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                DoScriptText(SAY_PUSILLIN_5, pCreature);
                float fX, fY, fZ;
                //pCreature->GetPosition(fX, fY, fZ);
                pCreature->GetMotionMaster()->MovePoint(15, Move[15].x, Move[15].y, Move[15].z);
				pCreature->SummonCreature(NPC_WILDSPAWN_IMP, 6.95f,-709.65f,-12.64f,1.88f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
				pCreature->SummonCreature(NPC_WILDSPAWN_IMP, 25.19f,-693.73f,-12.64f,3.68f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
				pCreature->SummonCreature(NPC_WILDSPAWN_IMP, 24.12f,-697.13f,-12.64f,4.01f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
				pCreature->SummonCreature(NPC_WILDSPAWN_IMP, 17.33f,-665.37f,-12.64f,4.71f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
				pCreature->SummonCreature(NPC_WILDSPAWN_IMP, 6.43f,-669.18f,-12.64f,5.33f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);				
				if(Creature* Imp = m_pInstance->GetSingleCreatureFromStorage(NPC_WILDSPAWN_IMP))
					{
						Imp->AI()->AttackStart(pPlayer);						//Not working atm, increased aggro range in DB instead.
					}
                break;
        }
    }
    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

void AddSC_boss_pusillin()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_pusillin";
    pNewscript->GetAI = &GetAI_boss_pusillin;
    pNewscript->pGossipHello =  &GossipHello_boss_pusillin;
    pNewscript->pGossipSelect = &GossipSelect_boss_pusillin;
    pNewscript->RegisterSelf();
}
