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
SDName: Wetlands
SD%Complete: 80
SDComment: Quest support: 1249
SDCategory: Wetlands
EndScriptData */

/* ContentData
npc_tapoke_slim_jahn
npc_mikhail
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_tapoke_slim_jahn
######*/

enum
{
	SAY_SLIM_AGGRO              = -1720012,
    SAY_SLIM_DEFEAT             = -1720013,
    SAY_FRIEND_DEFEAT           = -1720014,
    SAY_SLIM_NOTES              = -1720015,
	SAY_END_BRAWL				= -1720019,

    QUEST_MISSING_DIPLO_PT11    = 1249,
    FACTION_ENEMY               = 168,
	//FACTION_NEUTRAL				= ,						// should actually be yellow and not hostile
    SPELL_STEALTH               = 1785,
    SPELL_CALL_FRIENDS          = 16457,                    //summons 1x friend
    NPC_SLIMS_FRIEND            = 4971,
    NPC_TAPOKE_SLIM_JAHN        = 4962,
	NPC_MIKHAIL					= 4963,
};

static const DialogueEntry aDiplomatDialogue[] =
{
    {SAY_SLIM_DEFEAT,           NPC_TAPOKE_SLIM_JAHN,   4000},
    {SAY_SLIM_NOTES,            NPC_TAPOKE_SLIM_JAHN,   12000},
	{SAY_END_BRAWL,				NPC_MIKHAIL,			12000},
    {0, 0, 0},
};

struct MANGOS_DLL_DECL npc_tapoke_slim_jahnAI : public npc_escortAI, private DialogueHelper
{
    npc_tapoke_slim_jahnAI(Creature* pCreature) : npc_escortAI(pCreature),
        DialogueHelper(aDiplomatDialogue) {Reset();}

    bool m_bFriendSummoned;
	bool m_bEventComplete;

    GUIDList lSlimFriends;
	ObjectGuid m_mikhail;

    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_bFriendSummoned = false;

        lSlimFriends.clear();
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 2:
                if (m_creature->HasStealthAura())
                    m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                SetRun();
                m_creature->setFaction(FACTION_ENEMY);
                break;
        }
    }

    void Aggro(Unit* /*pWho*/)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (HasEscortState(STATE_ESCORT_ESCORTING) && !m_bFriendSummoned && pPlayer)
        {
            for(uint8 i = 0; i < 3; ++i)
                //m_creature->CastSpell(m_creature, SPELL_CALL_FRIENDS, true);
                DoSpawnCreature(NPC_SLIMS_FRIEND, irand(-3,3), irand(-3,3), 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

			DoScriptText(SAY_SLIM_AGGRO, m_creature);
            m_bFriendSummoned = true;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_SLIMS_FRIEND)
        {
			lSlimFriends.push_back(pSummoned->GetObjectGuid());

            if (Player* pPlayer = GetPlayerForEscort())
            {
                pSummoned->setFaction(FACTION_ENEMY);
                pSummoned->AI()->AttackStart(pPlayer);
            }
			pSummoned->SetRespawnDelay(-10);			// make sure they won't respawn randomly
        }
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (pAttacker->IsHostileTo(m_creature))
            ScriptedAI::AttackedBy(pAttacker);
    }

    void DespawnSlimFriends()
    {
        if (!lSlimFriends.empty())
            for(GUIDList::iterator itr = lSlimFriends.begin(); itr != lSlimFriends.end(); ++itr)
                if (Creature* pSlimFriend = m_creature->GetMap()->GetCreature(*itr))
                {
                    if (pSlimFriend->isInCombat())
                        pSlimFriend->AI()->ResetToHome();
                    pSlimFriend->setFaction(35);
					pSlimFriend->ForcedDespawn(5000);
                }
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (GetPlayerForEscort())
        {
            GetPlayerForEscort()->GroupEventHappens(QUEST_MISSING_DIPLO_PT11, m_creature);
            DespawnSlimFriends();
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        // If new health will be lower than 20 percent, end fight and complete quest.
        if ((((m_creature->GetHealth() - uiDamage) * 100) / m_creature->GetMaxHealth()) <= 20)
        {
            if (Player* pPlayer = GetPlayerForEscort())
            {
                pPlayer->GroupEventHappens(QUEST_MISSING_DIPLO_PT11, m_creature);

                uiDamage = 0;

                DespawnSlimFriends();

                m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
                m_creature->RemoveAllAuras();
                m_creature->DeleteThreatList();
                m_creature->CombatStop(true);

				DoScriptText(SAY_FRIEND_DEFEAT, m_creature);
				
				StartNextDialogueText(SAY_SLIM_DEFEAT);
				if (Creature* pMikhail = GetClosestCreatureWithEntry(m_creature, NPC_MIKHAIL, 50.0f))
					m_mikhail = pMikhail->GetObjectGuid();

                SetRun(false);
            }
        }
    }

    void JustDidDialogueStep(int32 iEntry)
    {
		switch (iEntry)
        {
            case SAY_SLIM_NOTES:
            // despawn and respawn at inn
            m_creature->ForcedDespawn(5000);
            m_creature->SetRespawnDelay(2);
        }
    }

    Creature* GetSpeakerByEntry(uint32 uiEntry)
    {
        if (uiEntry == NPC_TAPOKE_SLIM_JAHN)
            return m_creature;
		if (uiEntry == NPC_MIKHAIL)
            return m_creature->GetMap()->GetCreature(m_mikhail);

        return NULL;
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        DialogueUpdate(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_tapoke_slim_jahn(Creature* pCreature)
{
    return new npc_tapoke_slim_jahnAI(pCreature);
}

/*######
## npc_mikhail
######*/

bool QuestAccept_npc_mikhail(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_MISSING_DIPLO_PT11)
    {
        Creature* pSlim = GetClosestCreatureWithEntry(pCreature, NPC_TAPOKE_SLIM_JAHN, 25.0f);

        if (!pSlim)
            return false;

        if (!pSlim->HasStealthAura())
            pSlim->CastSpell(pSlim, SPELL_STEALTH, true);

        if (npc_tapoke_slim_jahnAI* pEscortAI = dynamic_cast<npc_tapoke_slim_jahnAI*>(pSlim->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return false;
}

/*######
## AddSC
######*/

void AddSC_wetlands()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_tapoke_slim_jahn";
    pNewscript->GetAI = &GetAI_npc_tapoke_slim_jahn;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_mikhail";
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_mikhail;
    pNewscript->RegisterSelf();
}
