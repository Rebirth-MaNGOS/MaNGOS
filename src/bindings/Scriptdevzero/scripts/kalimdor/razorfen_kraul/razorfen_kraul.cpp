/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Razorfen Kraul
SD%Complete: 100
SDComment: Quest 1144 Support, Ward Keepers for Ward passing support
SDCategory: Razorfen Kraul
EndScriptData */

/* ContentData
npc_death_head_ward_keeper
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "razorfen_kraul.h"

/*#########################
# Npc Willix The Importer #
##########################*/

enum Says
{
    SAY_READY                       = -1047000,
    SAY_POINT                       = -1047001,
    SAY_AGGRO1                      = -1047002,
    SAY_BLUELEAF                    = -1047003,
    SAY_DANGER                      = -1047004,
    SAY_BAD                         = -1047005,
    SAY_THINK                       = -1047006,
    SAY_SOON                        = -1047007,
    SAY_FINALY                      = -1047008,
    SAY_WIN                         = -1047009,
    SAY_END                         = -1047010
};

struct MANGOS_DLL_DECL npc_willix_the_importerAI : public npc_escortAI
{
    npc_willix_the_importerAI(Creature *pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
        case 3:
            m_creature->HandleEmote(EMOTE_STATE_POINT);
            DoScriptText(SAY_POINT, m_creature, pPlayer);
            break;
        case 4:
            m_creature->SummonCreature(NPC_RAGING_AGAMAR, 2137.66f, 1843.98f, 48.08f, 1.54f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            break;
        case 8:
            DoScriptText(SAY_BLUELEAF, m_creature, pPlayer);
            break;
        case 9:
            DoScriptText(SAY_DANGER, m_creature, pPlayer);
            break;
        case 13:
            DoScriptText(SAY_BAD, m_creature, pPlayer);
            break;
        case 14:
            m_creature->SummonCreature(NPC_RAGING_AGAMAR, 2078.91f, 1704.54f, 56.77f, 1.54f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            break;
        case 25:
            DoScriptText(SAY_THINK, m_creature, pPlayer);
            break;
        case 31:
            DoScriptText(SAY_SOON, m_creature, pPlayer);
            break;
        case 42:
            DoScriptText(SAY_FINALY, m_creature, pPlayer);
            break;
        case 43:
            m_creature->SummonCreature(NPC_RAGING_AGAMAR, 1956.43f, 1596.97f, 81.75f, 1.54f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            break;
        case 45:
            DoScriptText(SAY_WIN, m_creature, pPlayer);
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            if (pPlayer && pPlayer->GetTypeId() == TYPEID_PLAYER)
                pPlayer->GroupEventHappens(QUEST_WILLIX_THE_IMPORTER, m_creature);
            break;
        case 46:
            DoScriptText(SAY_END, m_creature, pPlayer);
            break;
        }
    }

    void Reset()
    {
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO1, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(m_creature);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (Player* pPlayer = GetPlayerForEscort())
            pPlayer->FailQuest(QUEST_WILLIX_THE_IMPORTER);
    }
};

bool QuestAccept_npc_willix_the_importer(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_WILLIX_THE_IMPORTER)
    {
        if (npc_willix_the_importerAI* pEscortAI = dynamic_cast<npc_willix_the_importerAI*>(pCreature->AI()))
			pEscortAI->Start(false, pPlayer, pQuest, true);
        pCreature->setFaction(113);
        DoScriptText(SAY_READY, pCreature, pPlayer);
    }

    return true;
}

CreatureAI* GetAI_npc_willix_the_importer(Creature* pCreature)
{
    return new npc_willix_the_importerAI(pCreature);
}

/*#############################
# Npc Deaths Head Ward Keeper #
##############################*/

enum Spells
{
    SPELL_QUILLBOAR_CHANNELING        = 7083
};

struct MANGOS_DLL_DECL npc_deaths_head_ward_keeperAI : public ScriptedAI
{
    npc_deaths_head_ward_keeperAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 QuillboarChanneling_Timer;

    void Reset()
    {
        QuillboarChanneling_Timer = 1500;
    }
    
    void Aggro(Unit* /*pWho*/)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->isAlive())
            return;

        if (QuillboarChanneling_Timer <= uiDiff)
        {
            if (m_creature->IsNonMeleeSpellCasted(false))
                m_creature->InterruptNonMeleeSpells(true);

			DoCastSpellIfCan(m_creature, SPELL_QUILLBOAR_CHANNELING);
            QuillboarChanneling_Timer = 1100;
        }
		else
			QuillboarChanneling_Timer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_deaths_head_ward_keeper(Creature* pCreature)
{
    return new npc_deaths_head_ward_keeperAI(pCreature);
}

void AddSC_razorfen_kraul()
{
    Script* pNewscript;
    
    pNewscript = new Script;
    pNewscript->Name = "npc_willix_the_importer";
    pNewscript->GetAI = &GetAI_npc_willix_the_importer;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_willix_the_importer;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "npc_deaths_head_ward_keeper";
    pNewscript->GetAI = &GetAI_npc_deaths_head_ward_keeper;
    pNewscript->RegisterSelf();
}
