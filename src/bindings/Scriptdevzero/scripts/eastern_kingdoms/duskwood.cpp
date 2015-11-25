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
SDName: Duskwood
SD%Complete:
SDComment: Quest support: A daughter's love, outro. Translation to Ello, stitches event.
SDCategory: Duskwood
EndScriptData */

/* ContentData
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_town_crier
######*/

struct MANGOS_DLL_DECL npc_town_crierAI : public ScriptedAI							// used for zone yells on stitches event and dodds and paige
{
    npc_town_crierAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        if (!m_creature->isActiveObject() && m_creature->isAlive())
            m_creature->SetActiveObjectState(true);
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_town_crier(Creature* pCreature)
{
    return new npc_town_crierAI(pCreature);
}

/*######
## npc_morgan_ladimore
######*/

enum
{
	SAY_1 = -1720010,
	SAY_2 = -1720011,
};

struct MANGOS_DLL_DECL npc_morgan_ladimoreAI : public ScriptedAI
{
    npc_morgan_ladimoreAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	uint32 m_uiSpeechTimer;
    uint8 m_uiSpeechStep;

    void Reset()
    {
		m_creature->SetStandState(UNIT_STAND_STATE_STAND);
		m_uiSpeechTimer = 0;
        m_uiSpeechStep = 1;
    }

    void UpdateAI(const uint32 uiDiff)					// handle Rp at end of A daughter's love quest, it's a hack but can't do it in DB
    {
		if (!m_uiSpeechStep)
            return;

        if (m_uiSpeechTimer < uiDiff)
        {
            switch(m_uiSpeechStep)
            {
                case 1:
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    m_uiSpeechTimer = 3000;
                    break;
                case 2:
                    DoScriptText(SAY_1, m_creature);
                    m_uiSpeechTimer = 5000;
                    break;
				case 3:
					DoScriptText(SAY_2, m_creature);
                    m_uiSpeechTimer = 3000;
                    break;
				case 4:
					m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
					m_uiSpeechStep = 0;
					return;
                default:
                    m_uiSpeechStep = 0;
                    return;
            }
            ++m_uiSpeechStep;
        }
        else
            m_uiSpeechTimer -= uiDiff;
    }

};

CreatureAI* GetAI_npc_morgan_ladimore(Creature* pCreature)
{
    return new npc_morgan_ladimoreAI(pCreature);
}

/*######
## mob_stitches
######*/

enum
{
	YELL_SPAWN							= -1720020,
	YELL_AGGRO							= -1720021,
	YELL_GUARD_DOWN						= -1720022,
	YELL_STITCHES_DOWN					= -1720023,

	SPELL_AURA_OF_ROT					= 3106,

	NPC_WATCHER_DODDS					= 888,			// Crossroads group
	NPC_WATCHER_PAIGE					= 499,
	NPC_WATCHER_BLOMBERG				= 1000,
	NPC_WATCHER_HUTCHINS				= 1001,

	NPC_WATCHER_CUTFORD					= 1436,

	NPC_WATCHER_SELKIN					= 1100,			// rotting orchard group
	NPC_WATCHER_GELWIN					= 1099,
	NPC_WATCHER_MERANT					= 1098,
	NPC_WATCHER_THAYER					= 1101,
	
	NPC_WATCHER_CORWIN					= 1204,
	NPC_WATCHER_SARYS					= 1203,

	NPC_TOWN_CRIER						= 468,
	NPC_DUMMY_TOWN_CRIER				= 800468,
	NPC_STITCHES						= 412,
};

struct Loc
{
    float x, y, z, o;
};

static Loc aXRoadSpawnLoc[]= 
{
	{-10907.0f, -387.0f, 40.76f, 1.365f},
	{-10901.0f, -388.0f, 40.76f, 1.345f},
	{-10903.0f, -391.0f, 40.93f, 1.345f},			// move loc for dodds
	{-10905.0f, -391.45f, 40.93f, 1.345f},			// move loc for paige
};

static Loc aDarkshireSpawnLoc[]= 
{
	{-10696.0f, -1178.0f, 26.76f, 2.57f},		// right
	{-10698.0f, -1181.0f, 26.96f, 2.57f},
	{-10699.0f, -1183.0f, 26.93f, 2.57f},
	{-10701.0f, -1185.0f, 26.96f, 2.57f},		// left
};
static uint32 aFirstWatcherID[]=
{
	NPC_WATCHER_BLOMBERG,
	NPC_WATCHER_HUTCHINS,
};

static uint32 aWatcherID[]=
{
	NPC_WATCHER_SELKIN,
	NPC_WATCHER_GELWIN,
	NPC_WATCHER_MERANT,
	NPC_WATCHER_THAYER,
};

struct MANGOS_DLL_DECL mob_stitchesAI : public ScriptedAI
{
    mob_stitchesAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		DoScriptText(YELL_SPAWN, m_creature);			// yell on spawn
		m_bFirstGuardDown = false;

        Reset();
    }

	uint32 m_uiAuraofRotTimer;
	uint32 m_uiYellTimer;

	bool m_bFirstGuardDown;

    void Reset()
    {
        if (!m_creature->isActiveObject() && m_creature->isAlive())
            m_creature->SetActiveObjectState(true);

		m_uiYellTimer = 0;
		m_uiAuraofRotTimer = urand(5000, 12000);
    }

	void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(YELL_AGGRO, m_creature);
    }

	void JustSummoned(Creature* pSummoned)
    {
		pSummoned->SetRespawnDelay(-10);			// make sure they won't respawn randomly
		if (pSummoned->GetEntry() == NPC_DUMMY_TOWN_CRIER)
			DoScriptText(YELL_STITCHES_DOWN, pSummoned);
	}

	void JustDied(Unit* /*pKiller*/)
    {
		m_creature->SummonCreature(NPC_DUMMY_TOWN_CRIER, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 1000);
	}
	void SummonedCreatureJustDied(Creature* pSummoned)
    {
		if (pSummoned->GetEntry() == NPC_WATCHER_BLOMBERG || NPC_WATCHER_HUTCHINS)
			if (!m_bFirstGuardDown)
				HandleYell(YELL_GUARD_DOWN);
	}

	void MovementInform(uint32 /*uiMotiontype*/, uint32 uiPointId)
	{
		switch(uiPointId)
        {
			case 12:			// spawn cutford who runs all the way from darkshire to meet the guards at crossroads
				m_creature->SummonCreature(NPC_WATCHER_CUTFORD, aDarkshireSpawnLoc[1].x, aDarkshireSpawnLoc[1].y, aDarkshireSpawnLoc[1].z, aDarkshireSpawnLoc[1].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
				break;
			case 14:			// move guards from the camp up to the road
				if (Creature* pDodds = GetClosestCreatureWithEntry(m_creature, NPC_WATCHER_DODDS, 100.0f))		// not working, even tho they have activeobject
					pDodds->GetMotionMaster()->MovePoint(1,-10903.0f, -391.0f, 40.93f);

				if (Creature* pPaige = GetClosestCreatureWithEntry(m_creature, NPC_WATCHER_PAIGE, 100.0f))		// not working, even tho they have activeobject
					pPaige->GetMotionMaster()->MovePoint(1,-10905.0f, -391.45f, 40.93f);

				break;
			case 15:			// turn them the right way, they wouldn't move if it were in case 14
				if (Creature* pDodds = GetClosestCreatureWithEntry(m_creature, NPC_WATCHER_DODDS, 100.0f))
					pDodds->SetFacingTo(1.345f);

				if (Creature* pPaige = GetClosestCreatureWithEntry(m_creature, NPC_WATCHER_PAIGE, 100.0f))
					pPaige->SetFacingTo(1.345f);

				break;
			case 16:			// spawn the guards at crossroads
				for(uint8 i = 0; i < 2; ++i)
					m_creature->SummonCreature(aFirstWatcherID[i], aXRoadSpawnLoc[i].x, aXRoadSpawnLoc[i].y, aXRoadSpawnLoc[i].z, aXRoadSpawnLoc[i].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
				
				break;
			case 25:			// spawn the guards who run out from darkshire to rotting orchard
				for(uint8 i = 0; i < 4; ++i)
					m_creature->SummonCreature(aWatcherID[i], aDarkshireSpawnLoc[i].x, aDarkshireSpawnLoc[i].y, aDarkshireSpawnLoc[i].z, aDarkshireSpawnLoc[i].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000, true);
				break;
			case 55:			// spawn the last set of guards, just outside darkshire
				m_creature->SummonCreature(NPC_WATCHER_CORWIN, -10314.09f, -1137.0f, 21.99f, 2.41f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
				m_creature->SummonCreature(NPC_WATCHER_SARYS, -10310.0f, -1133.0f, 22.25f, 2.41f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000, true);
				break;
		}
	}

	void HandleYell(uint32 uiYellId = 0)
	{		
		{	
			if (uiYellId == YELL_GUARD_DOWN && !m_bFirstGuardDown)
			{
				Creature* pCrier = GetClosestCreatureWithEntry(m_creature, NPC_DUMMY_TOWN_CRIER, 50.0f); 			// get the dummy at the camp

				if (pCrier)
					DoScriptText(YELL_GUARD_DOWN, pCrier);
				m_bFirstGuardDown = true;
			}
		}
	}

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		// Aura of Rot
        if (m_uiAuraofRotTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_AURA_OF_ROT);
            m_uiAuraofRotTimer = urand(5000,12000);
        }
        else
            m_uiAuraofRotTimer -= uiDiff;

        DoMeleeAttackIfReady();
	}
};

/*####
# npc_ello_ebonlock
####*/

enum
{
	ELLO_RANDOM_SAY_1				 = -1720058,

    QUEST_ID_TRANSLATION_TO_ELLO	 = 252,
};

struct MANGOS_DLL_DECL npc_ello_ebonlockeAI : public npc_escortAI
{
    npc_ello_ebonlockeAI(Creature* pCreature) : npc_escortAI(pCreature) 
	{ 
		Reset(); 
		m_uiRandomSayTimer = urand(20000,30000);
	}
	
	uint32 m_uiRandomSayTimer;
	uint32 m_uiStitchesTimer;
	bool m_bStitches;
	bool m_bSay;

    void Reset()
	{
        if (!m_creature->isActiveObject() && m_creature->isAlive())
            m_creature->SetActiveObjectState(true);
	}

	void WaypointReached(uint32 uiPointId)
    {
	}

	void JustStartedEscort()
    {
    }

	void JustSummoned(Creature* pSummoned)
    {
		pSummoned->SetRespawnDelay(-10);			// make sure he won't respawn randomly
	}

	void SpawnStitches()
	{
		if (m_bStitches)					// If he gets to the last WP he'll despawn, so no need to force despawn
		{
			m_creature->SummonCreature(NPC_STITCHES, -10290.2f, 72.7811f, 38.8811f, 4.8015f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 180000, true);
			m_bStitches = false; 
			m_uiStitchesTimer = 1800000;		// 30 min before he can be summoned again
		}
	}

	void Aggro(Unit* /*pWho*/)				// no random say while in combat
    {
        m_bSay = false;
    }

	void JustReachedHome()
	{
		m_bSay = true;
		m_uiRandomSayTimer = urand(20000,120000);
	}

	void UpdateAI(const uint32 uiDiff)
    {
		npc_escortAI::UpdateAI(uiDiff);

		if (m_uiStitchesTimer < uiDiff)
			m_bStitches = true;
        else
            m_uiStitchesTimer -= uiDiff;

		if (m_bSay && m_uiRandomSayTimer < uiDiff)
		{
			DoScriptText(ELLO_RANDOM_SAY_1, m_creature);
			m_uiRandomSayTimer = 120000;
		}

        else
            m_uiRandomSayTimer -= uiDiff;

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}

	void UpdateEscortAI(const uint32 uiDiff)
    {
	}
};

CreatureAI* GetAI_npc_ello_ebonlocke(Creature* pCreature)
{
    return new npc_ello_ebonlockeAI(pCreature);
}

bool OnQuestRewarded_npc_ello_ebonlocke(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_TRANSLATION_TO_ELLO)
    {
		if (npc_ello_ebonlockeAI* pEscortAI = dynamic_cast<npc_ello_ebonlockeAI*>(pCreature->AI()))
			pEscortAI->SpawnStitches();
	}
	return true;
}

CreatureAI* GetAI_mob_stitches(Creature* pCreature)
{
    return new mob_stitchesAI(pCreature);
}

enum
{
    TWILIGHT_CORRUPTER_AGGRO = -1720121,
    SOUL_CORRUPTION          = 25805,
    CREATURE_OF_NIGHTMARE          = 25806,
    SPELL_LEVEL_UP      = 24312,
    QUEST_THE_NIGHTMARES_CORRUPTION = 8735,
    TWILIGHT_CORRUPTER = 15625,
};

struct MANGOS_DLL_DECL mob_twilight_corrupter : public ScriptedAI
{
    mob_twilight_corrupter(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uicorruption_timer;
    uint32 m_uinightmare_timer;
    uint32 m_uikill_counter;

    void Reset()
    {
        m_uicorruption_timer = urand(5000,15000);
        m_uinightmare_timer = urand(20000, 30000);
        m_uikill_counter = 0;
    }

	void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(TWILIGHT_CORRUPTER_AGGRO, m_creature);
    }

    void EnterCombat(Unit* /*pWho*/)
    {
        m_uikill_counter = 0;
    }

    void KilledUnit(Unit* pVictim)
    {
        if(pVictim && pVictim->GetTypeId() == TYPEID_PLAYER)
        {
            m_creature->GenericTextEmote(std::string("Twilight Corrupter squeezes the last bit of life out of " + std::string(pVictim->GetName()) + " and swallows their soul.").c_str(), nullptr, false);
            
            ++m_uikill_counter;

            if(m_uikill_counter == 3)
            {
                DoCastSpellIfCan(m_creature, SPELL_LEVEL_UP, CAST_TRIGGERED);
                m_creature->SetLevel(m_creature->getLevel() + 1);
                m_uikill_counter = 0;
            }
        }
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uicorruption_timer)
        {
            if(m_uicorruption_timer <= uiDiff)
            {
                if(Unit *pVictim = m_creature->getVictim())
                    DoCastSpellIfCan(pVictim, SOUL_CORRUPTION);

                m_uicorruption_timer = urand(5000,15000);
            }
            else
                m_uicorruption_timer -= uiDiff;
        }

        if(m_uinightmare_timer)
        {
            if(m_uinightmare_timer <= uiDiff)
            {
                if(Unit *pVictim = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCastSpellIfCan(pVictim, CREATURE_OF_NIGHTMARE);

                m_uinightmare_timer = urand(20000, 30000);
            }
            else
                m_uinightmare_timer -= uiDiff;
        }
		DoMeleeAttackIfReady();
    }	
};

/*######
## mob_commander_felstrom
######*/

enum eFelstrom
{
	SPELL_FELSTROM_RESURRECTION = 3488
};

struct MANGOS_DLL_DECL mob_commander_felstromAI : public ScriptedAI						
{
    mob_commander_felstromAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}
	bool m_bDidCast;
    void Reset()
    {
		m_bDidCast = false;		
    }

	void SpellHit(Unit* pCaster, SpellEntry const* pSpell)
    {
        if (pSpell->Id == SPELL_FELSTROM_RESURRECTION && pCaster == m_creature)
        {
			m_creature->SetLootRecipient(NULL);
			m_creature->SetOwnerGuid(m_creature->GetObjectGuid());				// no owner means no exp
		}
	}

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if(HealthBelowPct(11) && !m_bDidCast)
		{
			DoCastSpellIfCan(m_creature,SPELL_FELSTROM_RESURRECTION);
			m_bDidCast = true;
		}

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_twilight_corrupter(Creature* pCreature)
{
    return new mob_twilight_corrupter(pCreature);
}

CreatureAI* GetAI_mob_commander_felstrom(Creature* pCreature)
{
    return new mob_commander_felstromAI(pCreature);
}

bool AreaTrigger_at_Twilight_Grove(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->GetQuestStatus(QUEST_THE_NIGHTMARES_CORRUPTION) == QUEST_STATUS_INCOMPLETE)
    {
        Creature* corrupter = GetClosestCreatureWithEntry(pPlayer, TWILIGHT_CORRUPTER, 10000);

        if(!corrupter && pPlayer)
        {
            corrupter = pPlayer->SummonCreature(TWILIGHT_CORRUPTER, -10337, -494, 51.32, 2.68, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
            corrupter->MonsterWhisper(std::string("Come, " + std::string(pPlayer->GetName()) + ". See what the nightmare brings.").c_str(), pPlayer, false);
        }
    }
    return false;
}

void AddSC_duskwood()
{
    Script* pNewscript;

	pNewscript = new Script;
	pNewscript->Name = "npc_town_crier";
	pNewscript->GetAI = &GetAI_npc_town_crier;
	pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_morgan_ladimore";
    pNewscript->GetAI = &GetAI_npc_morgan_ladimore;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
	pNewscript->Name = "mob_stitches";
	pNewscript->GetAI = &GetAI_mob_stitches;
	pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_ello_ebonlocke";
    pNewscript->GetAI = &GetAI_npc_ello_ebonlocke;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_ello_ebonlocke;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
	pNewscript->Name = "mob_twilight_corrupter";
	pNewscript->GetAI = &GetAI_mob_twilight_corrupter;
	pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "at_Twilight_Grove";
    pNewscript->pAreaTrigger = &AreaTrigger_at_Twilight_Grove;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
	pNewscript->Name = "mob_commander_felstrom";
	pNewscript->GetAI = &GetAI_mob_commander_felstrom;
	pNewscript->RegisterSelf();
}
