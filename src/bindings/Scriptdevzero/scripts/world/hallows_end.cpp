#include "precompiled.h"
#include "simple_ai.h"
#include "TemporarySummon.h"
#include "CreatureEventAI.h"

enum
{
	CANDY_FOR_JESPER = 8311,
	CANDY_FOR_SPOOPS = 8312,

	FULL_VENDOR_LIST = 0,
	ABRIDGED_VENDOR_LIST = 15354
};

uint32 CandyVendor_VendorId(Player *pPlayer, Creature* /*pCreature*/)
{
	if (pPlayer->GetQuestRewardStatus(CANDY_FOR_JESPER) || pPlayer->GetQuestRewardStatus(CANDY_FOR_SPOOPS))
		return FULL_VENDOR_LIST;
	else
		return ABRIDGED_VENDOR_LIST;
}

//Xing for Y quest AI - these quests exist for both factions and make the player /emote something at an NPC to get a quest 
//item for the Candy For Jesper/Spoops quests
class candy_baronAI : public CreatureAI
{
private:
	uint32 m_EmoteId;
	uint32 m_QuestId;
	uint32 m_TextId;
public:
	candy_baronAI(Creature *pCreature, uint32 emoteId, uint32 questId, uint32 responseTextId) : CreatureAI(pCreature), m_EmoteId(emoteId), m_QuestId(questId), m_TextId(responseTextId)
	{
	}

	void ReceiveEmote(Player* pPlayer, uint32 uiEmote)
	{
		if (uiEmote == m_EmoteId)
		{
			if (pPlayer->GetQuestStatus(m_QuestId) == QUEST_STATUS_INCOMPLETE)
			{
				//Say something fawning at the player
				DoScriptText(m_TextId,m_creature,pPlayer);

				pPlayer->AreaExploredOrEventHappens(m_QuestId);
			}
		}
	}
};

enum
{
	FLEXING_FOR_NOUGAT_HORDE = 8359,
	INCOMING_GUMDROP_HORDE = 8358,
	CHICKEN_CLUCK_MINT_HORDE = 8354,
	DANCE_FOR_MARZIPAN_HORDE = 8360,

	FLEXING_FOR_NOUGAT_ALLY = 8356,
	INCOMING_GUMDROP_ALLY = 8355,
	CHICKEN_CLUCK_MINT_ALLY = 8353,
	DANCE_FOR_MARZIPAN_ALLY = 8357,

	TEXT_SO_STRONG = -1712001,
	TEXT_NINETY_NINE_PROBLEMS = -1712002,
	TEXT_CHUGGA_CHUGGA = -1712003,
	TEXT_BAWK_BAWK_BAWK = -1712004
};

//Inkeeper Gryshka - Flexing For Nougat
CreatureAI *InnkeeperGryshka_GetAI(Creature *pCreature)
{
	return new candy_baronAI(pCreature,TEXTEMOTE_FLEX,FLEXING_FOR_NOUGAT_HORDE,TEXT_SO_STRONG);
}

//Kali Remik - Incoming Gumdrup
CreatureAI *KaliRemik_GetAI(Creature *pCreature)
{
	return new candy_baronAI(pCreature,TEXTEMOTE_TRAIN,INCOMING_GUMDROP_HORDE,TEXT_CHUGGA_CHUGGA);
}

//Innkeeper Norman - Chicken Clucking for a Mint
CreatureAI *InnkeeperNorman_GetAI(Creature *pCreature)
{
	return new candy_baronAI(pCreature,TEXTEMOTE_CHICKEN,CHICKEN_CLUCK_MINT_HORDE,TEXT_BAWK_BAWK_BAWK);
}

//Innkeeper Pala - Dancing for Marzipan
CreatureAI *InnkeeperPala_GetAI(Creature *pCreature)
{
	return new candy_baronAI(pCreature,TEXTEMOTE_DANCE, DANCE_FOR_MARZIPAN_HORDE, TEXT_NINETY_NINE_PROBLEMS);
}

//Innkeeper Allison - Flexing for Nougat
CreatureAI *InnkeeperAllison_GetAI(Creature *pCreature)
{
	return new candy_baronAI(pCreature, TEXTEMOTE_FLEX, FLEXING_FOR_NOUGAT_ALLY, TEXT_SO_STRONG);
}

//Talvash Kissel - Incoming Gumdrop
CreatureAI *TalvashKissel_GetAI(Creature *pCreature)
{
	return new candy_baronAI(pCreature, TEXTEMOTE_TRAIN, INCOMING_GUMDROP_ALLY, TEXT_CHUGGA_CHUGGA);
}

//Innkeeper Firebrew - Chicken Clucking for a Mint
CreatureAI *InnkeeperFirebrew_GetAI(Creature *pCreature)
{
	return new candy_baronAI(pCreature, TEXTEMOTE_CHICKEN, CHICKEN_CLUCK_MINT_ALLY, TEXT_BAWK_BAWK_BAWK);
}

//Innkeeper Saelienne - Dancing for Marzipan
CreatureAI *InnkeeperSaelienne_GetAI(Creature *pCreature)
{
	return new candy_baronAI(pCreature, TEXTEMOTE_DANCE, DANCE_FOR_MARZIPAN_ALLY, TEXT_NINETY_NINE_PROBLEMS);
}

//Stink bomb cloud- remove on use
bool StinkBomb_Use(Player* /*pPlayer*/, GameObject *pGameObject)
{
	pGameObject->SetLootState(GO_JUST_DEACTIVATED);
	return true;
}

enum
{
	STAGE_30_MINUTE_WARNING 	= 0,
	STAGE_15_MINUTE_WARNING		= 1,
	STAGE_5_MINUTE_WARNING		= 2,
	STAGE_FESTIVAL_BEGINS		= 3,
	STAGE_SUMMON_SYLVANAS		= 4,
	STAGE_SYLVANAS_SPEECH_1		= 5,
	STAGE_SYLVANAS_SPEECH_2		= 6,
	STAGE_SYLVANAS_SPEECH_3		= 7,
	STAGE_SYLVANAS_SPEECH_4		= 8,
	STAGE_SYLVANAS_SPEECH_5		= 9,
	STAGE_SYLVANAS_SPEECH_END	= 10,
	STAGE_LIGHT_WICKERMAN		= 11,
	STAGE_DESPAWN_SYLVANAS		= 12,
	STAGE_RESET					= 13,

	ENTRY_SYLVANAS				= 15193,
	ENTRY_WICKERMAN				= 180433,
	ENTRY_EMBER					= 180437,
	ENTRY_GHOSTLY				= 22650,

	DURATION_ALL_NIGHT			= 39600000,

	TEXT_30_MINUTE_WARNING		= -1712005,
	TEXT_15_MINUTE_WARNING		= -1712006,
	TEXT_5_MINUTE_WARNING		= -1712007,
	TEXT_FESTIVAL_BEGINS		= -1712008,
	TEXT_SPEECH_1				= -1712009,
	TEXT_SPEECH_2				= -1712010,
	TEXT_SPEECH_3				= -1712011,
	TEXT_SPEECH_4				= -1712012,
	TEXT_SPEECH_5				= -1712013,
	TEXT_SPEECH_END				= -1712014
};

//Deathcaller Yanka's AI
class npc_deathcaller_yankaAI : public SimpleAI
{
private:
	//Script to check for the time and start when it's time
	uint8 m_ScriptStage;
	uint32 m_TimerTillNext;
	Creature *m_pSummonedSylvanas;
	uint32 m_ResetAnimProgress;

	void TimerTillNextWickerman()
	{
		//Figure out how long until the next wickerman festival we have- the wickerman
		//script starts at 7:30PM
		time_t currentTime = time(NULL);
		tm *localTime = localtime(&currentTime);
		
		//If we're already past 7:30PM, then add a day to the time - mktime() is supposed
		//to do stuff like carry extra days into the next month, extra months into the next year, etc.
		if (localTime->tm_hour > 19 || (localTime->tm_hour == 19 && localTime->tm_min >= 30))
			localTime->tm_mday = localTime->tm_mday + 1;

		localTime->tm_hour = 19;
		localTime->tm_min = 30;
		time_t targetTime = mktime(localTime);
		m_TimerTillNext = floor(difftime(targetTime,currentTime)*1000.0);
	}

	GameObject *FindWickerman()
	{
		//Find the wickerman within 100 meters
		GameObject *wickerman = NULL;
		AllGameObjectsWithEntryInRangeCheck go_check(m_creature,ENTRY_WICKERMAN, 100);
		MaNGOS::GameObjectSearcher<AllGameObjectsWithEntryInRangeCheck> checker(wickerman, go_check);
		Cell::VisitGridObjects(m_creature,checker,100);

		return wickerman;
	}

	void CreateEmber( float x, float y, float z)
	{
		GameObject* pGameObj = new GameObject;
		Map *map = m_creature->GetMap();

		if(!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), ENTRY_EMBER, map,
			x, y, z, 0, 0.0f, 0.0f, 0.0f, 0.0f, GO_ANIMPROGRESS_DEFAULT, GO_STATE_READY))
		{
			delete pGameObj;
			return;
		}

		pGameObj->SetRespawnTime(DURATION_ALL_NIGHT);
		pGameObj->SetOwnerGuid(m_creature->GetObjectGuid());

		pGameObj->Refresh();
	}

public:
	npc_deathcaller_yankaAI(Creature *pCreature) : SimpleAI(pCreature), m_ScriptStage(0), m_pSummonedSylvanas(NULL), m_ResetAnimProgress(100)
	{
		Reset();
	}

	virtual void Reset()
	{
		//Find and despawn sylvanas if she's hanging around
		Creature *sylvanas = NULL;
		AllCreaturesOfEntryInRangeCheck u_check(m_creature,ENTRY_SYLVANAS,100);
        MaNGOS::CreatureSearcher<AllCreaturesOfEntryInRangeCheck> checker(sylvanas, u_check);
        Cell::VisitGridObjects(m_creature,checker, 100);

		if (sylvanas)
			sylvanas->AddObjectToRemoveList();

		//Stop the wickerman burning
		GameObject *pWickerman = FindWickerman();
		if (pWickerman)
		{
			pWickerman->SetGoState(GO_STATE_READY);
			pWickerman->SetGoAnimProgress(m_ResetAnimProgress);
		}

		m_ResetAnimProgress = 100;

		//Do the 30 minute warning at 7:30PM
		m_ScriptStage = STAGE_30_MINUTE_WARNING;
		TimerTillNextWickerman();
	}

	virtual void UpdateAI(const uint32 diff)
	{
		if (m_TimerTillNext)
		{
			if (m_TimerTillNext < diff)
			{
				m_TimerTillNext = 0;
			} else
			{
				m_TimerTillNext -= diff;
				return;
			}
		}

		switch(m_ScriptStage)
		{
			case STAGE_30_MINUTE_WARNING:
				DoScriptText(TEXT_30_MINUTE_WARNING, m_creature);
				m_TimerTillNext = 900000;
				break;
			case STAGE_15_MINUTE_WARNING:
				DoScriptText(TEXT_15_MINUTE_WARNING, m_creature);
				m_TimerTillNext = 600000;
				break;
			case STAGE_5_MINUTE_WARNING:
				DoScriptText(TEXT_5_MINUTE_WARNING, m_creature);
				m_TimerTillNext = 300000;
				break;
			case STAGE_FESTIVAL_BEGINS:
				DoScriptText(TEXT_FESTIVAL_BEGINS, m_creature);
				m_TimerTillNext = 3000;
				break;
			case STAGE_SUMMON_SYLVANAS:
				m_pSummonedSylvanas = m_creature->SummonCreature(ENTRY_SYLVANAS,1733.65f,510.56f,40.8f,1.57f,TEMPSUMMON_MANUAL_DESPAWN,0);
				//m_pSummonedSylvanas->CastSpell(m_pSummonedSylvanas,ENTRY_GHOSTLY,false);
				//m_pSummonedSylvanas->SetSheath(SHEATH_STATE_RANGED);
				m_TimerTillNext = 5000;
				break;
			case STAGE_SYLVANAS_SPEECH_1:
				DoScriptText(TEXT_SPEECH_1, m_pSummonedSylvanas);
				m_TimerTillNext = 2000;
				break;
			case STAGE_SYLVANAS_SPEECH_2:
				DoScriptText(TEXT_SPEECH_2, m_pSummonedSylvanas);
				m_TimerTillNext = 9000;
				break;
			case STAGE_SYLVANAS_SPEECH_3:
				DoScriptText(TEXT_SPEECH_3, m_pSummonedSylvanas);
				m_TimerTillNext = 8000;
				break;
			case STAGE_SYLVANAS_SPEECH_4:
				DoScriptText(TEXT_SPEECH_4, m_pSummonedSylvanas);
				m_TimerTillNext = 13000;
				break;
			case STAGE_SYLVANAS_SPEECH_5:
				DoScriptText(TEXT_SPEECH_5, m_pSummonedSylvanas);
				m_TimerTillNext = 8000;
				break;
			case STAGE_SYLVANAS_SPEECH_END:
				DoScriptText(TEXT_SPEECH_END, m_pSummonedSylvanas);
				m_TimerTillNext = 8000;
				break;
			case STAGE_LIGHT_WICKERMAN:
				if (m_pSummonedSylvanas)
				{
					m_pSummonedSylvanas->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
	
					GameObject *wickerman = FindWickerman();
					if (wickerman)
					{
						wickerman->Use(m_pSummonedSylvanas);
						//The next time we reset, do the fire extinguish anim
						m_ResetAnimProgress = 0;
					}
				}

				m_TimerTillNext = 2000;
				break;
			case STAGE_DESPAWN_SYLVANAS:
				if (m_pSummonedSylvanas)
				{
					m_pSummonedSylvanas->AddObjectToRemoveList();
				}

				//Create the embers
				CreateEmber( 1741.67f, 511.25f, 39.57f);
				CreateEmber( 1743.97f, 505.03f, 40.86f);
				CreateEmber( 1736.3f, 514.68f, 38.87f);
				CreateEmber( 1730.27f, 514.9f, 39.18f);
				CreateEmber( 1723.12f, 512.68f, 39.17f);
				CreateEmber( 1720.16f, 507.29f, 40.06f);
				
				//Burn the whole night 
				m_TimerTillNext = DURATION_ALL_NIGHT;
				break;
			case STAGE_RESET:
				Reset();
				return;
		}

		m_ScriptStage++;
	}
};

CreatureAI *deathcaller_yanka_GetAI(Creature *pCreature)
{
	return new npc_deathcaller_yankaAI(pCreature);
}

//Wickerman Guardian AI - Normal EventAI, but cast Guardian Ember on death
enum
{
	ENTRY_GUARDIAN_EMBER = 25007
};

class wickerman_guardianAI : public CreatureEventAI
{
public:
	wickerman_guardianAI(Creature *pCreature) : CreatureEventAI(pCreature) {}
	
	void JustDied(Unit* /*pKiller*/)
	{
		Map *map = m_creature->GetMap();

		if (map == NULL)
			return;

		//Create an ember on our corpse if it's wickerman time
		GameObject *wickerman = NULL;
		AllGameObjectsWithEntryInRangeCheck go_check(m_creature,ENTRY_WICKERMAN, map->GetVisibilityDistance());
		MaNGOS::GameObjectSearcher<AllGameObjectsWithEntryInRangeCheck> checker(wickerman, go_check);
		Cell::VisitGridObjects(m_creature,checker,map->GetVisibilityDistance());

		if (wickerman == NULL)
			return;

		if (wickerman->GetGoState() != GO_STATE_READY)
		{
			//It's burning!
			m_creature->CastSpell(m_creature,ENTRY_GUARDIAN_EMBER,true);
		}
	}
};

CreatureAI *wickerman_guardian_GetAI(Creature *pCreature)
{
	return new wickerman_guardianAI(pCreature);
}

void AddSC_hallows_end()
{
	Script *pNewScript;

	//Candy vendors (only sell pumpkin treats when player complete's faction candy quest
	pNewScript = new Script;
	pNewScript->Name = "npc_candy_vendor";
	pNewScript->pVendorIdNPC = &CandyVendor_VendorId;
	pNewScript->RegisterSelf();

	//Innkeeper Gryshka (Flexing For Nougat)
	pNewScript = new Script;
	pNewScript->Name = "npc_innkeeper_gryshka";
	pNewScript->GetAI = &InnkeeperGryshka_GetAI;
	pNewScript->RegisterSelf();

	//Kali Remik (Incoming Gumdrop)
	pNewScript = new Script;
	pNewScript->Name = "npc_kali_remik";
	pNewScript->GetAI = &KaliRemik_GetAI;
	pNewScript->RegisterSelf();

	//Innkeeper Norman (Chicken Clucking for a Mint)
	pNewScript = new Script;
	pNewScript->Name = "npc_innkeeper_norman";
	pNewScript->GetAI = &InnkeeperNorman_GetAI;
	pNewScript->RegisterSelf();

	//Innkeeper Pala (Dancing for Marzipan)
	pNewScript = new Script;
	pNewScript->Name = "npc_innkeeper_pala";
	pNewScript->GetAI = &InnkeeperPala_GetAI;
	pNewScript->RegisterSelf();

	//Innkeeper Allison (Flexing for Nougat)
	pNewScript = new Script;
	pNewScript->Name = "npc_innkeeper_allison";
	pNewScript->GetAI = &InnkeeperAllison_GetAI;
	pNewScript->RegisterSelf();

	//Talvash Kissel (Incoming Gumdrop)
	pNewScript = new Script;
	pNewScript->Name = "npc_talvash_kissel";
	pNewScript->GetAI = &TalvashKissel_GetAI;
	pNewScript->RegisterSelf();

	//Innkeeper Firebrew (Chicken Clucking for a Mint)
	pNewScript = new Script;
	pNewScript->Name = "npc_inkeeper_firebrew";
	pNewScript->GetAI = &InnkeeperFirebrew_GetAI;
	pNewScript->RegisterSelf();

	//Innkeeper Saelienne (Dancing for Marzipan)
	pNewScript = new Script;
	pNewScript->Name = "npc_innkeeper_saelienne";
	pNewScript->GetAI = &InnkeeperSaelienne_GetAI;
	pNewScript->RegisterSelf();

	//Stink bomb cloud (remove from world on use script)
	pNewScript = new Script;
	pNewScript->Name = "stink_bomb_cloud";
	pNewScript->pGOUse = &StinkBomb_Use;
	pNewScript->RegisterSelf();

	//Deathcaller Yanka
	pNewScript = new Script;
	pNewScript->Name = "deathcaller_yanka";
	pNewScript->GetAI = &deathcaller_yanka_GetAI;
	pNewScript->RegisterSelf();

	//Wickerman guardian
	pNewScript = new Script;
	pNewScript->Name = "wickerman_guardian";
	pNewScript->GetAI = &wickerman_guardian_GetAI;
	pNewScript->RegisterSelf();
}
