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
SDName: Desolace
SD%Complete: 100
SDComment: Quest support: 1440, 5561, 6132
SDCategory: Desolace
EndScriptData */

/* ContentData
npc_aged_dying_ancient_kodo
npc_dalinda_malem
go_hand_of_iruxos_crystal
npc_melizza_brimbuzzle
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

enum
{
    SAY_SMEED_HOME_1                = -1000348,
    SAY_SMEED_HOME_2                = -1000349,
    SAY_SMEED_HOME_3                = -1000350,

    QUEST_KODO                      = 5561,

    NPC_SMEED                       = 11596,
    NPC_AGED_KODO                   = 4700,
    NPC_DYING_KODO                  = 4701,
    NPC_ANCIENT_KODO                = 4702,
    NPC_TAMED_KODO                  = 11627,

    SPELL_KODO_KOMBO_ITEM           = 18153,
    SPELL_KODO_KOMBO_PLAYER_BUFF    = 18172,                //spells here have unclear function, but using them at least for visual parts and checks
    SPELL_KODO_KOMBO_DESPAWN_BUFF   = 18377,
    SPELL_KODO_KOMBO_GOSSIP         = 18362

};

struct MANGOS_DLL_DECL npc_aged_dying_ancient_kodoAI : public ScriptedAI
{
    npc_aged_dying_ancient_kodoAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiDespawnTimer;

    void Reset()
    {
        m_uiDespawnTimer = 0;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetEntry() == NPC_SMEED)
        {
            if (m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                return;

            if (m_creature->IsWithinDistInMap(pWho, 10.0f))
            {
                switch(urand(0, 2))
                {
                    case 0: DoScriptText(SAY_SMEED_HOME_1, pWho); break;
                    case 1: DoScriptText(SAY_SMEED_HOME_2, pWho); break;
                    case 2: DoScriptText(SAY_SMEED_HOME_3, pWho); break;
                }

                //spell have no implemented effect (dummy), so useful to notify spellHit
                m_creature->CastSpell(m_creature,SPELL_KODO_KOMBO_GOSSIP,true);
            }
        }
    }

    void SpellHit(Unit* /*pCaster*/, SpellEntry const* pSpell)
    {
        if (pSpell->Id == SPELL_KODO_KOMBO_GOSSIP)
        {
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            m_uiDespawnTimer = 60000;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //timer should always be == 0 unless we already updated entry of creature. Then not expect this updated to ever be in combat.
        if (m_uiDespawnTimer && m_uiDespawnTimer <= diff)
        {
            if (!m_creature->getVictim() && m_creature->isAlive())
            {
                Reset();
                m_creature->SetDeathState(JUST_DIED);
                m_creature->Respawn();
                return;
            }
        } else m_uiDespawnTimer -= diff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_aged_dying_ancient_kodo(Creature* pCreature)
{
    return new npc_aged_dying_ancient_kodoAI(pCreature);
}

bool EffectDummyCreature_npc_aged_dying_ancient_kodo(Unit *pCaster, uint32 spellId, SpellEffectIndex effIndex, Creature *pCreatureTarget)
{
    //always check spellid and effectindex
    if (spellId == SPELL_KODO_KOMBO_ITEM && effIndex == EFFECT_INDEX_0)
    {
        //no effect if player/creature already have aura from spells
        if (pCaster->HasAura(SPELL_KODO_KOMBO_PLAYER_BUFF) || pCreatureTarget->getFaction() == 35)
            return true;

        if (pCreatureTarget->GetEntry() == NPC_AGED_KODO ||
            pCreatureTarget->GetEntry() == NPC_DYING_KODO ||
            pCreatureTarget->GetEntry() == NPC_ANCIENT_KODO)
        {
            pCaster->CastSpell(pCaster,SPELL_KODO_KOMBO_PLAYER_BUFF,true);

            pCreatureTarget->UpdateEntry(NPC_TAMED_KODO);
            //pCreatureTarget->CastSpell(pCreatureTarget,SPELL_KODO_KOMBO_DESPAWN_BUFF,false);

            if (pCreatureTarget->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
                pCreatureTarget->GetMotionMaster()->MoveIdle();

			pCreatureTarget->AI()->ResetToHome();
			pCreatureTarget->GetMotionMaster()->Clear();
            pCreatureTarget->GetMotionMaster()->MoveFollow(pCaster, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }

        //always return true when we are handling this spell and effect
        return true;
    }
    return false;
}

bool GossipHello_npc_aged_dying_ancient_kodo(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(SPELL_KODO_KOMBO_PLAYER_BUFF) && pCreature->getFaction() == 35)
    {
        //the expected quest objective
        pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetObjectGuid());

        pPlayer->RemoveAurasDueToSpell(SPELL_KODO_KOMBO_PLAYER_BUFF);
        pCreature->GetMotionMaster()->MoveIdle();
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_dalinda_malem
######*/

enum eDalindaMalem
{
	QUEST_RETURN_TO_VAHLARRIEL = 1440,
	DALINDA_SAY1               = -1000351,
	DALINDA_SAY2               = -1000352
};

struct MANGOS_DLL_DECL npc_dalinda_malemAI : public npc_escortAI
{
	npc_dalinda_malemAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

	void Reset()
	{
		if (HasEscortState(STATE_ESCORT_ESCORTING))
			return;
	}

    void JustStartedEscort()
    {
        if (GetPlayerForEscort())
            DoScriptText(DALINDA_SAY1, m_creature);
		    m_creature->SetFactionTemporary(124);			// added temp faction so mobs can hit her
    }

	void WaypointReached(uint32 uiPointId)
	{
		Player* pPlayer = GetPlayerForEscort();
		if (!pPlayer)
			return;

		switch(uiPointId)
		{
			case 20:
				DoScriptText(DALINDA_SAY2, m_creature);
				SetRun();
				break;
			case 24:
				m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
				pPlayer->GroupEventHappens(QUEST_RETURN_TO_VAHLARRIEL, m_creature);
				break;
		}
	}

	void JustDied(Unit* /*pKiller*/)
	{
		if (Player* pPlayer = GetPlayerForEscort())
			pPlayer->FailQuest(QUEST_RETURN_TO_VAHLARRIEL);
	}

	void UpdateAI(const uint32 uiDiff)
	{
		npc_escortAI::UpdateAI(uiDiff);

		//return since we have no target
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_dalinda_malem(Creature* pCreature)
{
	return new npc_dalinda_malemAI (pCreature);
}

bool QuestAccept_npc_dalinda_malem(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_RETURN_TO_VAHLARRIEL)
		if (npc_dalinda_malemAI* pEscortAI = dynamic_cast<npc_dalinda_malemAI*>(pCreature->AI()))
			pEscortAI->Start(false, pPlayer, pQuest, true);
	return true;
}

/*######
## go_hand_of_iruxos_crystal
######*/

bool GOUse_go_hand_of_iruxos_crystal(Player* /*pPlayer*/, GameObject* pGo)
{
    // Summon "Demon Spirit" (11876)
    pGo->SummonCreature(11876, pGo->GetPositionX()+2, pGo->GetPositionY()+2, pGo->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
    return false;
}

/*######
## npc_melizza_brimbuzzle
######*/

enum eMelizzaBrimbuzzle
{
    QUEST_GET_ME_OUT_OF_HERE        = 6132,
    NPC_MARAUDINE_WRANGLER          = 4655,     // Ambusher
    NPC_MARAUDINE_MAULER            = 4656,     // Ambusher
    NPC_BONEPAW_HYENA               = 4688,     // Ambusher
	NPC_HORNIZZ						= 6019,
    GO_MELIZZAS_CAGES               = 177706,

	SAY_MELIZZA_1					= -1720202,
	SAY_MELIZZA_2					= -1720203,
	SAY_MELIZZA_3					= -1720204,		// cheer emote?
	SAY_MELIZZA_4					= -1720205,
	SAY_MELIZZA_5					= -1720206,		// bye
	SAY_HORNIZZ_1					= -1720207,		// bye
};

struct MANGOS_DLL_DECL npc_melizza_brimbuzzleAI : public npc_escortAI
{
    npc_melizza_brimbuzzleAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

	bool m_bOutro;
	uint32 m_uiSpeechTimer;
	uint8 m_uiSpeechStep;

    void Reset()
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
            m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
    }

	/*void MoveInLineOfSight(Unit* pWho)		// only attack if the player is in combat, can't get it to work propperly
    {
        Player* pPlayer = GetPlayerForEscort();
        if (!pPlayer)
            return;
		if(pPlayer->isInCombat() && !m_creature->isInCombat() && pWho->isAttackingPlayer())
			m_creature->AI()->AttackStart(pWho);
    }*/

	void JustSummoned(Creature* pSummoned)
    {
		pSummoned->SetRespawnDelay(-10);			// make sure they won't respawn
    }

    void WaypointReached(uint32 uiPointId)
    {
        Player* pPlayer = GetPlayerForEscort();
        if (!pPlayer)
            return;

        switch(uiPointId)
        {
			case 1:
				DoScriptText(SAY_MELIZZA_1,m_creature,pPlayer);
				break;
            case 6:
                // 4x Maulers at Y
                m_creature->SummonCreature(NPC_MARAUDINE_MAULER, -1305.36f, 2625.21f, 111.61f, 1.01f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                m_creature->SummonCreature(NPC_MARAUDINE_MAULER, -1318.45f, 2672.05f, 111.86f, 6.17f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                m_creature->SummonCreature(NPC_MARAUDINE_MAULER, -1323.01f, 2677.04f, 112.66f, 6.17f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                m_creature->SummonCreature(NPC_MARAUDINE_MAULER, -1299.36f, 2640.15f, 111.36f, 0.86f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                break;
            case 24:
                // 3x Wranglers + Hyenas at Mountain exit
                m_creature->SummonCreature(NPC_MARAUDINE_WRANGLER, -1379.76f, 2409.82f, 88.90f, 2.01f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                m_creature->SummonCreature(NPC_MARAUDINE_WRANGLER, -1394.71f, 2410.82f, 88.72f, 1.39f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                m_creature->SummonCreature(NPC_MARAUDINE_WRANGLER, -1409.36f, 2413.23f, 88.49f, 1.06f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
				m_creature->SummonCreature(NPC_BONEPAW_HYENA, -1380.76f, 2408.82f, 88.90f, 2.01f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                m_creature->SummonCreature(NPC_BONEPAW_HYENA, -1398.98f, 2408.82f, 88.72f, 1.13f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
				m_creature->SummonCreature(NPC_BONEPAW_HYENA, -1410.36f, 2412.73f, 88.49f, 1.06f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                break;
            case 29:
                m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
                m_creature->SetFacingToObject(pPlayer);
				DoScriptText(SAY_MELIZZA_2,m_creature,NULL);
                pPlayer->GroupEventHappens(QUEST_GET_ME_OUT_OF_HERE, m_creature);
                SetRun(true);
                break;
			case 43:
				m_bOutro = true;
				m_uiSpeechStep = 1;
				m_uiSpeechTimer = 500;
				break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
		npc_escortAI::UpdateAI(uiDiff);
		//Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
		DoMeleeAttackIfReady();
	}

	void UpdateEscortAI(const uint32 uiDiff)
    {
		if (m_uiSpeechTimer && m_bOutro)							// handle RP at quest end
		{
			if (!m_uiSpeechStep)
				return;
		
			if (m_uiSpeechTimer <= uiDiff)
            {			
				Creature* pHornizz = GetClosestCreatureWithEntry(m_creature, NPC_HORNIZZ, 10.0f);
                switch(m_uiSpeechStep)
                {
					case 1:
						DoScriptText(SAY_MELIZZA_3,m_creature,NULL);
						m_creature->SetFacingTo(5.16f);
						m_uiSpeechTimer = 5000;
						break;
					case 2:
						DoScriptText(SAY_MELIZZA_4,m_creature,NULL);
                        m_uiSpeechTimer = 10000;
						break;
					case 3:
						DoScriptText(SAY_MELIZZA_5,m_creature,NULL);
						m_uiSpeechTimer = 3000;
						break;
					case 4:
						DoScriptText(SAY_HORNIZZ_1,pHornizz,NULL);
						m_uiSpeechTimer = 6000;
						break;
					case 5:
						m_bOutro = false;
						break;
                    /*default:
                        m_uiSpeechStep = 0;
                        return;*/
                }
                ++m_uiSpeechStep;
            }
            else
                m_uiSpeechTimer -= uiDiff;
		}
	}
};

CreatureAI* GetAI_npc_melizza_brimbuzzle(Creature* pCreature)
{
    return new npc_melizza_brimbuzzleAI(pCreature);
}

bool QuestAccept_npc_melizza_brimbuzzle(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_GET_ME_OUT_OF_HERE)
    {
        if (npc_melizza_brimbuzzleAI* pEscortAI = dynamic_cast<npc_melizza_brimbuzzleAI*>(pCreature->AI()))
        {
            pEscortAI->Start(false, pPlayer, pQuest);
            pCreature->setFaction(250);
            if (GameObject* GoCage = GetClosestGameObjectWithEntry(pCreature, GO_MELIZZAS_CAGES, 10.0f))
                GoCage->UseDoorOrButton();
        }
        else
            return false;
    }
    return true;
}

/*######
## Demon Portal
######*/

bool go_use_demon_portal(Player* /*pPlayer*/, GameObject* pGo)
{
	pGo->SummonCreature(11937, pGo->GetPositionX()+2, pGo->GetPositionY()+2, pGo->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
	return false;
}

/*************
** Magrami Ghost
******/

enum MagramiSpectre
{
    GAMEOBJECT_GHOST_MAGNET = 177746,
    SPELL_CHILLING_TOUCH = 12531,
    SPELL_CURSE_OF_THE_FALLEN_MAGRAM = 18159,
    MOB_MAGRAMI_SPECTRE = 11560,
    GHOST_VISUAL = 16713,
};

struct MANGOS_DLL_DECL npc_ghost_magnet_dummy : public ScriptedAI
{
    npc_ghost_magnet_dummy(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    Creature *pGhosts[4];
    float ghostPositions[4][4];
    bool m_summoned;
    uint32 m_initSummonCount;
    uint32 m_summonCount;
    uint32 m_magnetResetTimer;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->UpdateVisibilityAndView();
        m_initSummonCount = 0;
        m_summonCount = 0;
        m_magnetResetTimer = 90000;
        m_summoned = false;

        for(int i = 0; i < 4; i++)
        {
            ghostPositions[i][i] = 0.0f;
            pGhosts[i] = nullptr;
        }
    }

    bool GhostPositionAvailable(Creature *pGhost)
    {
            if(pGhost)
            {
                if(m_initSummonCount < 4)
                {
                    pGhosts[m_initSummonCount] = pGhost;
                
                    for(int i = 0; i < 4; i++)
                    {
                        switch(i)
                        {
                        case 1:
                            ghostPositions[m_initSummonCount][i] = pGhost->GetPositionX();
                            break;
                        case 2:
                            ghostPositions[m_initSummonCount][i] = pGhost->GetPositionY();
                            break;
                        case 3:
                            ghostPositions[m_initSummonCount][i] = pGhost->GetPositionZ();
                            break;
                        }
                    }
                    m_initSummonCount++;

                    return true;
                }
                else
                    return false;
            }

        return false;
    }

    void UpdateAI(const uint32 uiDiff)
    {   
        if(m_magnetResetTimer)
        {
            if(m_magnetResetTimer <= uiDiff)
            {
                m_initSummonCount = 0;
                m_magnetResetTimer = 0;
            }
            else
                m_magnetResetTimer -= uiDiff;
        }
    }

};

CreatureAI* GetAI_npc_ghost_magnet_dummy(Creature* pCreature)
{
    return new npc_ghost_magnet_dummy(pCreature);
}

struct MANGOS_DLL_DECL npc_magrim_spectre_dummy : public ScriptedAI
{
    npc_magrim_spectre_dummy(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    Creature *pGhostSummoned;
    uint32 m_ghostCleanupTimer;

    void Reset()
    {
        pGhostSummoned = nullptr;
        m_ghostCleanupTimer = 1000;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->UpdateVisibilityAndView();
        pGhostSummoned = m_creature->SummonCreature(MOB_MAGRAMI_SPECTRE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
    }

    void SummonedCreatureDespawn(Creature *pSummoned)
    {
        m_creature->SummonCreature(MOB_MAGRAMI_SPECTRE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
    }

    void SummonedCreatureJustDied(Creature *pSummoned)
    {
        m_creature->SummonCreature(MOB_MAGRAMI_SPECTRE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
    }

    void GhostCleanup()
    {
        bool first = true;
        std::list<Creature*> ghostList;
        GetCreatureListWithEntryInGrid(ghostList, m_creature, MOB_MAGRAMI_SPECTRE, 2.0f);

        if(!ghostList.empty())
        {
            for (std::list<Creature*>::iterator iter = ghostList.begin(); iter != ghostList.end(); ++iter)
			{
				if (*iter)
				{
					Creature* ghost = (*iter);
                    if(ghost)
                    {     
                        if(ghost->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE && !ghost->isDead())
                        {
                            if(!first)
                            {
                                ghost->ForcedDespawn();
                            }

                            first = false;
                        }
                    }
				}
			}
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_ghostCleanupTimer)
        {
            if(m_ghostCleanupTimer <= uiDiff)
            {
                GhostCleanup();
                m_ghostCleanupTimer = 1000;
            }
            else
                m_ghostCleanupTimer -= uiDiff;
        }
    }

};

CreatureAI* GetAI_npc_magrim_spectre_dummy(Creature* pCreature)
{
    return new npc_magrim_spectre_dummy(pCreature);
}

struct MANGOS_DLL_DECL mob_magrami_spectre : public ScriptedAI
{
    mob_magrami_spectre(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool trapDetected;
    bool hostile;
    bool summoned;
    uint32 m_spellCdChilling;
    uint32 m_spellCdCurse;
    uint32 m_summonedTiemr;
    float x,y,z, targetX, targetY, targetZ;

    void Reset()
    {
        x = m_creature->GetPositionX();
        y = m_creature->GetPositionY();
        z = m_creature->GetPositionZ();
        targetX = 0;
        targetY = 0;
        targetZ = 0;
        DoCast(m_creature, GHOST_VISUAL);
        //m_creature->setFaction(189);
		m_creature->setFaction(35);
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->UpdateVisibilityAndView();
        trapDetected = false;
        hostile = false;
        summoned = false;
        m_spellCdChilling = 0;
        m_spellCdCurse = 0;
        m_summonedTiemr = 0;
    }

	void Aggro(Unit* /*pTarget*/)
    {
		m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
    }

    void JustDied(Unit *pKiller)
    {
        if(summoned)
        {
            if(x && y && z && targetX && targetY && targetZ)
            {
                Creature *summonedGhost = m_creature->SummonCreature(MOB_MAGRAMI_SPECTRE, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                if(summonedGhost)
                {
                    mob_magrami_spectre *magramiAI = dynamic_cast<mob_magrami_spectre*>(summonedGhost->AI());

                    if(magramiAI)
                    {
                        magramiAI->trapDetected = true;	
						summonedGhost->setFaction(189);
                        summonedGhost->SetVisibility(VISIBILITY_ON);
                        summonedGhost->UpdateVisibilityAndView();
                        summonedGhost->SetSplineFlags(SPLINEFLAG_WALKMODE);
                        summonedGhost->SetSpeedRate(MOVE_WALK, 0.5f, true);
                        summonedGhost->GetMotionMaster()->MovePoint(0, targetX, targetY, targetZ, true);
                    }
                }
            }
        }
        ScriptedAI::JustDied(pKiller);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_summonedTiemr)
        {
            if(m_summonedTiemr <= uiDiff)
            {				
				m_creature->setFaction(189);
                m_creature->SetVisibility(VISIBILITY_ON);
                m_creature->UpdateVisibilityAndView();
                m_creature->SetSplineFlags(SPLINEFLAG_WALKMODE);
                m_creature->GetMotionMaster()->MovePoint(0, x, y, z, true);  
                m_summonedTiemr = 0;
            }
            else
                m_summonedTiemr -= uiDiff;
        }
        if(!hostile)
        {
            if(!trapDetected)
            {
                if(Creature *ghostHandler = GetClosestCreatureWithEntry(m_creature, 800010, 35.0f))
                {
                    if(npc_ghost_magnet_dummy *handlerAI = dynamic_cast<npc_ghost_magnet_dummy*>(ghostHandler->AI()))
                    {
                        if(handlerAI->GhostPositionAvailable(m_creature))
                        {
                            targetX = ghostHandler->GetPositionX();
                            targetY = ghostHandler->GetPositionY();
                            targetZ = ghostHandler->GetPositionZ();
							m_creature->setFaction(189);
                            m_creature->SetVisibility(VISIBILITY_ON);
                            m_creature->UpdateVisibilityAndView();
                            m_creature->SetSplineFlags(SPLINEFLAG_WALKMODE);
                            m_creature->SetSpeedRate(MOVE_WALK, 0.5f, true);
                            m_creature->GetMotionMaster()->MovePoint(0, ghostHandler->GetPositionX(), ghostHandler->GetPositionY(), ghostHandler->GetPositionZ(), true);
                            trapDetected = true;
                            summoned = true;
                        }   
                    }
                }
            }
            else
            {
                    if(GameObject *ghostMagnet = GetClosestGameObjectWithEntry(m_creature, GAMEOBJECT_GHOST_MAGNET, 2.0f))
                    {
                        if(ghostMagnet->GetRespawnTime() != 0)
                        {
                            m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            m_creature->setFaction(21);
                            hostile = true;
                            m_spellCdCurse = 2000;
                            m_spellCdChilling = urand(5000, 10000);
                        }
                    }
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_spellCdChilling)
        {
            if(m_spellCdChilling <= uiDiff)
            {
                DoCast(m_creature->getVictim(), SPELL_CHILLING_TOUCH);
                m_spellCdChilling = urand(5000, 10000);
            }
            else
                m_spellCdChilling -= uiDiff;
        }

        if(m_spellCdCurse)
        {
            if(m_spellCdChilling <= uiDiff)
            {
                DoCast(m_creature->getVictim(), SPELL_CURSE_OF_THE_FALLEN_MAGRAM);
                m_spellCdCurse = 0;
            }
            else
                m_spellCdCurse -= uiDiff;
        }
        
        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_mob_magrami_spectre(Creature* pCreature)
{
    return new mob_magrami_spectre(pCreature);
}

/*####
# npc_maurin_bonesplitter
####*/

enum
{
	MAURIN_SAY_1					= -1720060,
	MAURIN_SAY_2					= -1720061,
	MAURIN_SAY_3					= -1720062,

	JUGKAR_SAY_1					= -1720063,
	JUGKAR_SAY_2					= -1720064,

	AZRETHOC_SAY_1					= -1720065,
	AZRETHOC_SAY_2					= -1720066,
	AZRETHOC_SAY_3					= -1720067,
	
	NPC_AZRETHOC_IMAGE				= 5772,
	NPC_JUGKAR_IMAGE				= 5773,

	QUEST_ID_THE_CORRUPTER			= 1482,
};

struct MANGOS_DLL_DECL npc_maurin_bonesplitterAI : public npc_escortAI
{
    npc_maurin_bonesplitterAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }
	
	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;
	bool m_bOutro;

	ObjectGuid m_uiPlayerGUID;

    void Reset()
	{
		m_bOutro = false;
		m_uiSpeechStep = 1;
		m_uiSpeechTimer = 0;
		m_uiPlayerGUID.Clear();
	}

	void WaypointReached(uint32 uiPointId)
    {
	}

	void JustStartedEscort()
    {
    }

	void JustSummoned(Creature* pSummoned)
    {
		pSummoned->SetRespawnDelay(-10);			// make sure they won't respawn randomly
	}

	void StartOutro(ObjectGuid pPlayerGUID)
	{
		if (!pPlayerGUID)
            return;

        m_uiPlayerGUID = pPlayerGUID;
		Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID);

		m_bOutro = true; 
		m_uiSpeechTimer = 4000;
		m_uiSpeechStep = 1;
		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
		DoScriptText(MAURIN_SAY_1, m_creature, pPlayer);
	}

	void UpdateAI(const uint32 uiDiff)
    {
		npc_escortAI::UpdateAI(uiDiff);

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}

	void UpdateEscortAI(const uint32 uiDiff)
    {
		if (m_uiSpeechTimer && m_bOutro)							// handle RP at quest end
		{
			if (!m_uiSpeechStep)
				return;
		
			if (m_uiSpeechTimer <= uiDiff)
            {			
				Creature* pAzrethoc = GetClosestCreatureWithEntry(m_creature, NPC_AZRETHOC_IMAGE, 10.0f);
				Creature* pJugkar = GetClosestCreatureWithEntry(m_creature, NPC_JUGKAR_IMAGE, 10.0f);
                switch(m_uiSpeechStep)
                {
					case 1:
						DoScriptText(MAURIN_SAY_2, m_creature);
                        m_uiSpeechTimer = 8000;
						break;
					case 2:
						m_creature->SummonCreature(NPC_JUGKAR_IMAGE,-1151.72f,1888.701f,88.86f,5.07f,TEMPSUMMON_TIMED_DESPAWN, 48000, true);		// summon jugkar
						m_uiSpeechTimer = 3000;
						break;
					case 3:
						DoScriptText(JUGKAR_SAY_1, pJugkar);
						m_uiSpeechTimer = 6000;
						break;
					case 4:
						m_creature->SummonCreature(NPC_AZRETHOC_IMAGE,-1153.79f,1893.51f,88.86f,5.07f,TEMPSUMMON_TIMED_DESPAWN, 30000, true);		// summon demon
						m_uiSpeechTimer = 3000;
						break;
					case 5:
						DoScriptText(AZRETHOC_SAY_1, pAzrethoc);
						m_uiSpeechTimer = 5000;
						break;
					case 6:
						DoScriptText(AZRETHOC_SAY_2, pAzrethoc);
						m_uiSpeechTimer = 8000;
						break;
					case 7:
						DoScriptText(AZRETHOC_SAY_3, pAzrethoc);
						m_uiSpeechTimer = 10000;
						break;
					case 8:
						DoScriptText(MAURIN_SAY_3, m_creature);
						m_uiSpeechTimer = 6000;
						break;
					case 9:
						DoScriptText(JUGKAR_SAY_2, pJugkar);
						m_uiSpeechTimer = 8000;
						break;
					case 10:
						m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						m_bOutro = false;
						break;
                    /*default:
                        m_uiSpeechStep = 0;
                        return;*/
                }
                ++m_uiSpeechStep;
            }
            else
                m_uiSpeechTimer -= uiDiff;
		}
	}
};

CreatureAI* GetAI_npc_maurin_bonesplitter(Creature* pCreature)
{
    return new npc_maurin_bonesplitterAI(pCreature);
}

bool OnQuestRewarded_npc_maurin_bonesplitter(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_THE_CORRUPTER)
    {
		if (npc_maurin_bonesplitterAI* pEscortAI = dynamic_cast<npc_maurin_bonesplitterAI*>(pCreature->AI()))
			pEscortAI->StartOutro(pPlayer->GetObjectGuid());
	}
	return true;
}

/*######
## npc_cork_gizelton
######*/

enum
{
//     SAY_CORK_AMBUSH1        = -1001191,
//     SAY_CORK_AMBUSH2        = -1001192,
//     SAY_CORK_AMBUSH3        = -1001193,
//     SAY_CORK_END            = -1001194,
//     SAY_RIGGER_AMBUSH1      = -1001195,
//     SAY_RIGGER_AMBUSH2      = -1001196,
//     SAY_RIGGER_END          = -1001197,

    NPC_CORK_GIZELTON       = 11625,
    NPC_RIGGER_GIZELTON     = 11626,
    NPC_VENDOR_TRON         = 12245,
    NPC_SUPER_SELLER        = 12246,

    NPC_KOLKAR_WAYLAYER     = 12976,
    NPC_KOLKAR_AMBUSHER     = 12977,

    NPC_DOOMWARDER          = 4677,
    NPC_LESSER_INFERNAL     = 4676,
    NPC_NETHER_SORCERESS    = 4684,

    QUEST_BODYGUARD_TO_HIRE = 5821,
    QUEST_GIZELTON_CARAVAN  = 5943,
};

struct SummonLocation
{
    float m_fX, m_fY, m_fZ;
};

static const SummonLocation aAmbushLocsBodyGuard[12] =
{
    // Quest QUEST_BODYGUARD_TO_HIRE
    {-969.05f, 1174.91f, 90.39f},       // First ambush
    {-985.71f, 1173.95f, 91.02f},
    {-983.01f, 1192.88f, 90.01f},
    {-965.51f, 1193.58f, 92.15f},
    {-1147.83f, 1180.87f, 91.38f},      // Second ambush
    {-1163.96f, 1183.72f, 93.79f},
    {-1160.97f, 1201.36f, 93.15f},
    {-1146.20f, 1199.75f, 91.37f},
    {-1277.78f, 1218.56f, 109.30f},     // Third ambush
    {-1292.65f, 1221.28f, 109.99f},
    {-1289.25f, 1239.20f, 108.79f},
    {-1272.91f, 1234.39f, 108.14f},
};

static const SummonLocation aAmbushLocsGizelton[9] =
{
    // Quest QUEST_GIZELTON_CARAVAN
    {-1823.7f, 2060.88f, 62.0925f},     // First ambush
    {-1814.46f, 2060.13f, 62.4916f},
    {-1814.87f, 2080.6f, 63.6323f},
    {-1782.92f, 1942.55f, 60.2205f},    // Second ambush
    {-1786.5f, 1926.05f, 59.7502f},
    {-1805.74f, 1942.77f, 60.791f},
    {-1677.56f, 1835.67f, 58.9269f},    // Third ambush
    {-1675.66f, 1863.0f, 59.0008f},
    {-1692.31f, 1862.69f, 58.9553f},
};

static const uint32 AmbushersBodyguard[4] = { NPC_KOLKAR_WAYLAYER, NPC_KOLKAR_AMBUSHER, NPC_KOLKAR_WAYLAYER, NPC_KOLKAR_AMBUSHER };
static const uint32 AmbushersGizleton[3] = { NPC_NETHER_SORCERESS, NPC_LESSER_INFERNAL, NPC_DOOMWARDER };

struct npc_cork_gizeltonAI : public ScriptedAI
{
    npc_cork_gizeltonAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_playerGuid;
    uint8 uiQuestStatus;

    void Reset()
    {
        uiQuestStatus = 0;
        m_creature->GetMotionMaster()->Clear();
    }

    void GetPlayer(ObjectGuid pPlayerGUID)
    {
         if (!pPlayerGUID)
            return;

        m_playerGuid = pPlayerGUID;
    }

    // Custom function to handle event ambushes
    void DoAmbush(uint32 uiQuestId, uint8 uiAmbushPoint)
    {
        uiAmbushPoint--;
        switch (uiQuestId)
        {
            case QUEST_BODYGUARD_TO_HIRE:
                // Summon 2 NPC_KOLKAR_WAYLAYER and 2 NPC_KOLKAR_AMBUSHER
                for (uint8 i = 0; i < 4; ++i)
                {
                    float fX, fY, fZ;
                    m_creature->GetRandomPoint(aAmbushLocsBodyGuard[i + 4 * uiAmbushPoint].m_fX, aAmbushLocsBodyGuard[i + 4 * uiAmbushPoint].m_fY, aAmbushLocsBodyGuard[i + 4 * uiAmbushPoint].m_fZ, 7.0f, fX, fY, fZ);
                    m_creature->SummonCreature(AmbushersBodyguard[i], fX, fY, fZ, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
                }
                break;
            case QUEST_GIZELTON_CARAVAN:
                // Summon 1 NPC_NETHER_SORCERESS, 1 NPC_LESSER_INFERNAL and 1 NPC_DOOMWARDER
                for (uint8 i = 0; i < 3; ++i)
                {
                    float fX, fY, fZ;
                    m_creature->GetRandomPoint(aAmbushLocsGizelton[i + 3 * uiAmbushPoint].m_fX, aAmbushLocsGizelton[i + 3 * uiAmbushPoint].m_fY, aAmbushLocsGizelton[i + 3 * uiAmbushPoint].m_fZ, 7.0f, fX, fY, fZ);
                    m_creature->SummonCreature(AmbushersGizleton[i], fX, fY, fZ, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
                }
                break;
        }

        return;
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != WAYPOINT_MOTION_TYPE)
            return;
        
        // Handles all flag removals and such
        switch (uiPointId)
        {        
            case 1:
            {
                m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
                
                if (Creature* pRigger = GetClosestCreatureWithEntry(m_creature, NPC_RIGGER_GIZELTON, 100.0f))
                    pRigger->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
                break;
            }      
            case 55:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break; 
            case 56:
                m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_creature->MonsterYell("So sorry to leave a customer but we have places to go and people to swindle. We will be back sometime later today. Good-bye!", LANG_UNIVERSAL, nullptr);
                break;
            case 76:
            {
                // Pause WP
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->MonsterYell("I am looking for some bodyguards that would like to protect the Gizelton Caravan. We are stopped on the road east of Kormek's Hut, north of Kolkar Centaur Village.", LANG_UNIVERSAL, nullptr);
                break;
            }
            case 77:            
                m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                break;
            case 196:
            {
                // Paus movement here
                if (Creature* pRigger = GetClosestCreatureWithEntry(m_creature, NPC_RIGGER_GIZELTON, 100.0f))
                    pRigger->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
            }
            case 197:
            {
                if (Creature* pRigger = GetClosestCreatureWithEntry(m_creature, NPC_RIGGER_GIZELTON, 100.0f))
                {
                    pRigger->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);               
                    pRigger->MonsterYell("Time for the Gizelton Caravan to head on out! We\'ll be back soon but if you cannot wait, head north to Kormek's Hut. We open shop in about an hour..", LANG_UNIVERSAL, nullptr);
                }
                break;
            }
            case 208:
            {
                // Paus movement here
                if (Creature* pRigger = GetClosestCreatureWithEntry(m_creature, NPC_RIGGER_GIZELTON, 100.0f))
                {
                    pRigger->MonsterYell("This is Rigger Gizelton asking for assistance escorting my caravan past Mannoroc Coven. I'm on the road east of Shadowprey village.", LANG_UNIVERSAL, nullptr);
                    pRigger->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
                }
                break;
            }
            case 209:
            {
                if (Creature* pRigger = GetClosestCreatureWithEntry(m_creature, NPC_RIGGER_GIZELTON, 100.0f))
                    pRigger->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);                
                break;
            }
        }

        // No player assigned as quest taker: abort to avoid summoning adds
        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);
        if (!pPlayer)
            return;
                   
            // Not corrected below points to -1 due to it happening after wp is passed
        if (pPlayer->GetQuestStatus(QUEST_BODYGUARD_TO_HIRE) == QUEST_STATUS_INCOMPLETE)
        {            
            switch (uiPointId)
            {
                case 77:
                    uiQuestStatus = 1;
                // First Ambush
                case 96:
                    m_creature->MonsterSay("Hey, you call yourself a body guard? Get to work and protect us...", LANG_UNIVERSAL, nullptr);
//                     DoScriptText(SAY_CORK_AMBUSH1, m_creature);
                    DoAmbush(QUEST_BODYGUARD_TO_HIRE, 1);
                    break;
                // Second Ambush
                case 103:
                    m_creature->MonsterSay("Mister body guard, are you going to earn your money or what?", LANG_UNIVERSAL, nullptr);
//                     DoScriptText(SAY_CORK_AMBUSH2, m_creature);
                    DoAmbush(QUEST_BODYGUARD_TO_HIRE, 2);
                    break;
                // Third Ambush
                case 111:
                    m_creature->MonsterSay("You're fired! <Cough...Cork clears throat.> I mean, help!", LANG_UNIVERSAL, nullptr);
//                     DoScriptText(SAY_CORK_AMBUSH3, m_creature);
                    DoAmbush(QUEST_BODYGUARD_TO_HIRE, 3);
                    break;
                case 116:
                    m_creature->MonsterSay("Blast those stupid centaurs! Sigh - well, it seems you kept your bargain. Up the road you shall find Smeed Scrabblescrew, he has your money.", LANG_UNIVERSAL, nullptr);
//                     DoScriptText(SAY_CORK_END, m_creature);
                    // Award quest credit
                    if (pPlayer)
                        pPlayer->GroupEventHappens(QUEST_BODYGUARD_TO_HIRE, m_creature);
                    // Remove player to avoid adds being spawned again next turn
                    m_playerGuid.Clear();
                    uiQuestStatus = 0;
                    m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    break;
                default:
                    m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                    break;
            }
        }
        // The second escort quest is also handled by NPC Cork though it is given by NPC Rigger
        else if (pPlayer->GetQuestStatus(QUEST_GIZELTON_CARAVAN) == QUEST_STATUS_INCOMPLETE)
        {
            switch (uiPointId)
            {
                case 209:
                    uiQuestStatus = 2;                    
                    // First Ambush
                case 218:
                    if (Creature* pRigger = GetClosestCreatureWithEntry(m_creature, NPC_RIGGER_GIZELTON, 100.0f))
                        pRigger->MonsterSay("Eeck! Demons hungry for the kodos!", LANG_UNIVERSAL, nullptr);
//                         DoScriptText(SAY_RIGGER_AMBUSH1, pRigger);
                    DoAmbush(QUEST_GIZELTON_CARAVAN, 1);
                    break;
                    // Second Ambush
                case 225:
                    if (Creature* pRigger = GetClosestCreatureWithEntry(m_creature, NPC_RIGGER_GIZELTON, 100.0f))
                         pRigger->MonsterSay("What am I paying you for? The kodos are nearly dead!", LANG_UNIVERSAL, nullptr);
//                         DoScriptText(SAY_RIGGER_AMBUSH2, pRigger);
                    DoAmbush(QUEST_GIZELTON_CARAVAN, 2);
                    break;
                    // Third Ambush
                case 235:
                    if (Creature* pRigger = GetClosestCreatureWithEntry(m_creature, NPC_RIGGER_GIZELTON, 100.0f))
                         pRigger->MonsterSay("Eeck! Demons hungry for the kodos!", LANG_UNIVERSAL, nullptr);
//                         DoScriptText(SAY_RIGGER_AMBUSH1, pRigger);
                    DoAmbush(QUEST_GIZELTON_CARAVAN, 3);
                    break;
                case 241:
                    if (Creature* pRigger = GetClosestCreatureWithEntry(m_creature, NPC_RIGGER_GIZELTON, 100.0f))
                        pRigger->MonsterSay("Wow! We did it... not sure why we thought we needed the likes of you. Nevertheless, speak with Smeed Srablescrew; he will give you your earnings!", LANG_UNIVERSAL, nullptr);
//                         DoScriptText(SAY_RIGGER_END, pRigger);
                    // Award quest credit
                    if (pPlayer)
                        pPlayer->GroupEventHappens(QUEST_GIZELTON_CARAVAN, m_creature);
                    // Remove player to avoid adds being spawned again next turn
                    m_playerGuid.Clear();
                    uiQuestStatus = 0;
                    m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    break;
                default:
                    m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                    break;                    
            }
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        // By default: summoned for the two escort quests will attack
        // So we want to add a special case to avoid the two summoned NPC vendors to also attack
        if (pSummoned->GetEntry() != NPC_VENDOR_TRON && pSummoned->GetEntry() != NPC_SUPER_SELLER)
            pSummoned->AI()->AttackStart(m_creature);
        
        pSummoned->SetRespawnEnabled(false);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (Creature* pRigger = GetClosestCreatureWithEntry(m_creature, NPC_RIGGER_GIZELTON, 100.0f))
            pRigger->ForcedDespawn();
        
        // Despawn Kodos
        std::list<Creature*> m_lKodos;
        GetCreatureListWithEntryInGrid(m_lKodos, m_creature, 11564, 30.f);

        if (!m_lKodos.empty())
        {
            for(std::list<Creature*>::iterator itr = m_lKodos.begin(); itr != m_lKodos.end(); ++itr)
            {
                if ((*itr) && (*itr)->isAlive())
                    (*itr)->ForcedDespawn();
            }
        }
        
        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);
        if (!pPlayer)
            return;

        // Handle all players in group (if they took quest)
        if (Group* pGroup = pPlayer->GetGroup())
        {
            for (GroupReference* pRef = pGroup->GetFirstMember(); pRef != nullptr; pRef = pRef->next())
            {
                if (Player* pMember = pRef->getSource())
                {
                    if (pMember->GetQuestStatus(QUEST_BODYGUARD_TO_HIRE) == QUEST_STATUS_INCOMPLETE)
                        pMember->FailQuest(QUEST_BODYGUARD_TO_HIRE);
                    if (pMember->GetQuestStatus(QUEST_GIZELTON_CARAVAN) == QUEST_STATUS_INCOMPLETE)
                        pMember->FailQuest(QUEST_GIZELTON_CARAVAN);
                }
            }
        }
        else
        {
            if (pPlayer->GetQuestStatus(QUEST_BODYGUARD_TO_HIRE) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(QUEST_BODYGUARD_TO_HIRE);
            if (pPlayer->GetQuestStatus(QUEST_GIZELTON_CARAVAN) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(QUEST_GIZELTON_CARAVAN);
        }
    }
};

CreatureAI* GetAI_npc_cork_gizelton(Creature* pCreature)
{
    return new npc_cork_gizeltonAI(pCreature);
}

bool QuestAccept_npc_cork_gizelton(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_BODYGUARD_TO_HIRE)
    {
        if (pPlayer->GetTeam() == ALLIANCE)
            pCreature->SetFactionTemporary(FACTION_ESCORT_A_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);

        if (pPlayer->GetTeam() == HORDE)
            pCreature->SetFactionTemporary(FACTION_ESCORT_H_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);

        if (npc_cork_gizeltonAI* pCork = dynamic_cast<npc_cork_gizeltonAI*>(pCreature->AI()))  
            pCork->GetPlayer(pPlayer->GetObjectGuid());
    }
    return true;
}

/*######
## npc_rigger_gizelton
######*/

struct npc_rigger_gizeltonAI : public ScriptedAI
{
    npc_rigger_gizeltonAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() {}
    
    void JustDied(Unit* /*pKiller*/)
    {
        if (Creature* pCork = GetClosestCreatureWithEntry(m_creature, NPC_CORK_GIZELTON, 100.0f))
            pCork->ForcedDespawn();
        
        // Despawn Kodos
        std::list<Creature*> m_lKodos;
        GetCreatureListWithEntryInGrid(m_lKodos, m_creature, 11564, 30.f);

        if (!m_lKodos.empty())
        {
            for(std::list<Creature*>::iterator itr = m_lKodos.begin(); itr != m_lKodos.end(); ++itr)
            {
                if ((*itr) && (*itr)->isAlive())
                    (*itr)->ForcedDespawn();
            }
        }
    }
};

CreatureAI* GetAI_npc_rigger_gizelton(Creature* pCreature)
{
    return new npc_rigger_gizeltonAI(pCreature);
}

bool QuestAccept_npc_rigger_gizelton(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_GIZELTON_CARAVAN)
    {
        if (pPlayer->GetTeam() == ALLIANCE)
            pCreature->SetFactionTemporary(FACTION_ESCORT_A_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);

        if (pPlayer->GetTeam() == HORDE)
            pCreature->SetFactionTemporary(FACTION_ESCORT_H_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);

        // Now the quest is accepted, tell NPC Cork what player took it so it can handle quest credit/failure
        // because NPC Cork will handle both escort quests
        if (npc_cork_gizeltonAI* pCork = dynamic_cast<npc_cork_gizeltonAI*>(pCreature->AI()))
            pCork->GetPlayer(pPlayer->GetObjectGuid());
    }

    return true;
}

/*######
## AddSC
######*/

void AddSC_desolace()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_aged_dying_ancient_kodo";
    pNewScript->GetAI = &GetAI_npc_aged_dying_ancient_kodo;
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_npc_aged_dying_ancient_kodo;
    pNewScript->pGossipHello = &GossipHello_npc_aged_dying_ancient_kodo;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "npc_dalinda_malem";
    pNewScript->GetAI = &GetAI_npc_dalinda_malem;
	pNewScript->pQuestAcceptNPC = &QuestAccept_npc_dalinda_malem;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_hand_of_iruxos_crystal";
    pNewScript->pGOUse = &GOUse_go_hand_of_iruxos_crystal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_melizza_brimbuzzle";
    pNewScript->GetAI = &GetAI_npc_melizza_brimbuzzle;
	pNewScript->pQuestAcceptNPC = &QuestAccept_npc_melizza_brimbuzzle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_demon_portal";
    pNewScript->pGOUse = &go_use_demon_portal;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "mob_magrami_spectre";
    pNewScript->GetAI = &GetAI_mob_magrami_spectre;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "npc_ghost_magnet_dummy";
    pNewScript->GetAI = &GetAI_npc_ghost_magnet_dummy;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "npc_magrim_spectre_dummy";
    pNewScript->GetAI = &GetAI_npc_magrim_spectre_dummy;
    pNewScript->RegisterSelf();
	
	pNewScript = new Script;
    pNewScript->Name = "npc_maurin_bonesplitter";
    pNewScript->GetAI = &GetAI_npc_maurin_bonesplitter;
    pNewScript->pQuestRewardedNPC = &OnQuestRewarded_npc_maurin_bonesplitter;
    pNewScript->RegisterSelf();   
    
    pNewScript = new Script;
    pNewScript->Name = "npc_cork_gizelton";
    pNewScript->GetAI = &GetAI_npc_cork_gizelton;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_cork_gizelton;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rigger_gizelton";
    pNewScript->GetAI = &GetAI_npc_rigger_gizelton;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_rigger_gizelton;
    pNewScript->RegisterSelf();
}
