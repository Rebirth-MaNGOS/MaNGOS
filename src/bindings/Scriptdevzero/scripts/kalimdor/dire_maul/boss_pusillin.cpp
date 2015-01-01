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
SDComment:
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
    {-167.80f,-198.62f,-4.15f,0},
    {-132.68f,-344.60f,-4.03f,0},
    {113.72f,-363.17f,-4.15f,0},
    {109.19f,-450.66f,-3.27f,0},
    {87.13f,-450.92f,-2.73f,0}
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
            case 0:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
            case 1:
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
                pCreature->GetMotionMaster()->MovePoint(0, Move[0].x, Move[0].y, Move[0].z);
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                DoScriptText(SAY_PUSILLIN_2, pCreature);
                pCreature->GetMotionMaster()->MovePoint(0, Move[1].x, Move[1].y, Move[1].z);
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                DoScriptText(SAY_PUSILLIN_3, pCreature);
                pCreature->GetMotionMaster()->MovePoint(0, Move[2].x, Move[2].y, Move[2].z);
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                DoScriptText(SAY_PUSILLIN_4, pCreature);
                pCreature->GetMotionMaster()->MovePoint(0, Move[3].x, Move[3].y, Move[3].z);
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                DoScriptText(SAY_PUSILLIN_5, pCreature);
                float fX, fY, fZ;
                pCreature->GetPosition(fX, fY, fZ);
                pCreature->GetMotionMaster()->MovePoint(1, Move[4].x, Move[4].y, Move[4].z);
                for(uint8 i = 0; i < 2; ++i)
                    if (Creature* Imp = pCreature->SummonCreature(NPC_WILDSPAWN_IMP, fX+irand(-3,3), fY+irand(-3,3), fZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                        Imp->AI()->AttackStart(pPlayer);
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
