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
SDName: Elemental Invasions
SD%Complete: 70
SDComment: find guid of adds that pat with the boss, all coordinates, array of array for which spawn array to choose(would make it a bit less code), verify timers for spells, make the event repeatable
SDCategory: 
EndScriptData */

#include "precompiled.h"

enum eInvasion
{
	// Rifts
	GOBJ_EARTH_RIFT = 179664,			// missing gobj? use same atm
	GOBJ_WATER_RIFT = 181650,	
	GOBJ_WIND_RIFT = 181651,
	GOBJ_FIRE_RIFT = 181652,	

	// Boss Rifts	
	GOBJ_EARTH_ELEMENTAL_RIFT = 179664,
	GOBJ_WATER_ELEMENTAL_RIFT = 179665,
	GOBJ_FIRE_ELEMENTAL_RIFT = 179666,
	GOBJ_WIND_ELEMENTAL_RIFT = 179667,
	

	// Adds
	NPC_WHIRLING_INVADER		 = 14455,
	NPC_WATERY_INVADER			 = 14458,
	NPC_BLAZING_INVADER			 = 14460,
	NPC_THUNDERING_INVADER		 = 14462,

	// Bossess
	BOSS_BARON_CHARR			 = 14461,
	BOSS_PRINCESS_TEMPESTRIA	 = 14457,
	BOSS_AVALANCHION			 = 14464,
	BOSS_THE_WINDREAVER			 = 14454,

	// Baron Charr yells
	YELL_SPAWN_CHARR             = -1720223,
	YELL_PAT_CHARR				 = -1720224,
	YELL_DEATH_CHARR             = -1720225,

	// Princess Tempestria yells
	YELL_SPAWN_TEMPESTRIA        = -1720229,
	YELL_PAT_TEMPESTRIA			 = -1720230,
	YELL_DEATH_TEMPESTRIA        = -1720231,

	// Avalanchion yells
	YELL_SPAWN_AVALANCHION       = -1720226,
	YELL_PAT_AVALANCHION		 = -1720227,
	YELL_DEATH_AVALANCHION       = -1720228,

	// The Windreaver yells
	YELL_SPAWN_THE_WINDREAVER    = -1720220,
	YELL_PAT_THE_WINDREAVER		 = -1720221,
	YELL_DEATH_THE_WINDREAVER    = -1720222,

	// Baron Charr spells
	SPELL_FLAME_BUFFET			 = 9574,
	SPELL_FIREBALL_VOLLEY		 = 15285,

	// Princess Tempestria spells
	SPELL_CONE_OF_COLD			 = 22746,
	SPELL_FROST_NOVA			 = 22645,
	SPELL_FROST_BOLT			 = 23102,
	SPELL_GEYSER				 = 10987,

	// Avalanchion spells
	SPELL_BOULDER				 = 23392,
	SPELL_GROUND_TREMOR			 = 6524,
	SPELL_TRAMPLE				 = 5568,

	// The Windreaver spells
	SPELL_CHAIN_LIGHTNING		 = 23106,
	SPELL_ENVELOPING_WINDS		 = 23103,
	SPELL_LIGHTNING_CLOUD		 = 23105,
	SPELL_SHOCK					 = 23104,
};

struct Loc
{
    float x, y, z;
};

static Loc BossRiftSpawnLoc[]=
{
	{0,0,0},
	{0,0,0},
	{0,0,0},
	{-6362.16f, 1490.92f, 6.96992f}	// The Windreaver
};

static Loc ElementalRiftSpawnLoc[]=
{
	{},
	{},
	{},
	{},
	{},
	{},
	{}
};

static Loc FireRiftSpawnLoc[]=
{
	{},
	{},
	{},
	{},
	{},
	{},
	{}
};

static Loc WaterRiftSpawnLoc[]=
{
	{},
	{},
	{},
	{},
	{},
	{},
	{}
};

static Loc EarthRiftSpawnLoc[]=
{
	{},
	{},
	{},
	{},
	{},
	{},
	{}
};

static Loc WindRiftSpawnLoc[]=
{
	{-6276.54f, 1545.05f, 3.6996f},
	{-6444.08f, 1502.04f, 6.69082f},
	{-6591.68f, 1504.70f, 5.49582f},
	{-6377.75f, 1381.48f, 4.27858f},
	{-6279.43f, 1353.65f, 12.2861f},
	{-6309.f, 1466.08f, 4.70567f}
};

static Loc BlazingInvaderSpawnLoc[]=
{
	{-9406.92f, 1955.86f, 85.55f}, // 1st rift
	{},
	{},

	{},	// 2nd rift
	{},
	{},

	{}, // 3rd rift
	{},
	{},

	{}, // 4th rift
	{},
	{},

	{}, // 5th rift
	{},
	{},

	{}, // 6th rift
	{},
	{}
};

static Loc WateryInvaderSpawnLoc[]=
{
	{-9406.92f, 1955.86f, 85.55f}, // 1st rift
	{},
	{},

	{},	// 2nd rift
	{},
	{},

	{}, // 3rd rift
	{},
	{},

	{}, // 4th rift
	{},
	{},

	{}, // 5th rift
	{},
	{},

	{}, // 6th rift
	{},
	{}
};

static Loc ThunderingInvaderSpawnLoc[]=
{
	{-9406.92f, 1955.86f, 85.55f}, // 1st rift
	{},
	{},

	{},	// 2nd rift
	{},
	{},

	{}, // 3rd rift
	{},
	{},

	{}, // 4th rift
	{},
	{},

	{}, // 5th rift
	{},
	{},

	{}, // 6th rift
	{},
	{}
};

static Loc WhirlingInvaderSpawnLoc[]=
{
	{-6303.96f, 1572.01f, 0.0361022f}, // 1st rift
	{-6291.42f, 1506.24f, 4.91244f},
	{-6325.56f, 1521.1f, 2.12703f},

	{-6396.81f, 1499.38f, 4.07937f}, // 2nd rift
	{-6425.88f, 1426.7f, -1.48203f},
	{-6501.24f, 1492.99f, 2.74427f},

	{-6527.58f, 1512.72f, 1.70818f}, // 3rd rift
	{-6545.98f, 1452.34f, 1.57142f},
	{-6547.71f, 1404.35f, 1.8712f},

	{-6399.91f, 1340.38f, 3.05505f}, // 4th rift
	{-6368.59f, 1447.91f, 2.51467f},
	{-6337.90f, 1367.57f, 2.65271f},

	{-6259.38f, 1307.90f, 15.807f}, // 5th rift
	{-6255.30f, 1363.72f, 15.8352f},
	{-6283.64f, 1391.6f, 10.1797f},

	{-6291.12f, 1448.76f, 9.27545f}, // 6th rift
	{-6316.f, 1486.09f, 3.78568f},
	{-6352.74f, 1471.69f, 4.46103f}
};

struct MANGOS_DLL_DECL boss_elemental_invasionsAI : public ScriptedAI
{
    boss_elemental_invasionsAI(Creature* pCreature) : ScriptedAI(pCreature)			// kör wait time på första wp lika lång som det ska va innan bossen spawn, så ser det ut som att den spawn från portal
	{
		m_creature->ApplySpellImmune(0, IMMUNITY_DISPEL, DISPEL_POISON, true);
				
		switch(m_creature->GetEntry())
        {
			case BOSS_BARON_CHARR:
				m_uiBossAddKind = NPC_BLAZING_INVADER;
				m_uiRiftKind = GOBJ_FIRE_RIFT;
				m_uiBossRiftKind = GOBJ_FIRE_ELEMENTAL_RIFT;
				m_uiBossRiftSpawnLoc = 0;
				m_uiNumberOfBossAdds = 3;
				m_uiHostileFaction = m_creature->getFaction();
				m_uiSpell1Entry = SPELL_FLAME_BUFFET;
				m_uiSpell2Entry = SPELL_FIREBALL_VOLLEY;
				m_uiSpell3Entry = 0;
				m_uiSpell4Entry = 0;
				m_uiSpell5Entry = 0;
				m_uiYellSpawn = YELL_SPAWN_CHARR;
				m_uiYellPat = YELL_PAT_CHARR;
				m_uiYellDeath = YELL_DEATH_CHARR;
				m_bEmoteText = false;
				break;
			case BOSS_PRINCESS_TEMPESTRIA:
				m_uiBossAddKind = NPC_WATERY_INVADER;
				m_uiRiftKind = GOBJ_WATER_RIFT;
				m_uiBossRiftKind = GOBJ_WATER_ELEMENTAL_RIFT;
				m_uiBossRiftSpawnLoc = 1;
				m_uiNumberOfBossAdds = 4;
				m_uiHostileFaction = m_creature->getFaction();
				m_uiSpell1Entry = 0;
				m_uiSpell2Entry = SPELL_GEYSER;
				m_uiSpell3Entry = SPELL_FROST_NOVA;
				m_uiSpell4Entry = SPELL_CONE_OF_COLD;
				m_uiSpell5Entry = SPELL_FROST_BOLT;
				m_uiYellSpawn = YELL_SPAWN_TEMPESTRIA;
				m_uiYellPat = YELL_PAT_TEMPESTRIA;
				m_uiYellDeath = YELL_PAT_TEMPESTRIA;
				m_bEmoteText = false;
				break;
			case BOSS_AVALANCHION:
				m_uiBossAddKind = NPC_THUNDERING_INVADER;
				m_uiRiftKind = GOBJ_EARTH_RIFT;
				m_uiBossRiftKind = GOBJ_EARTH_ELEMENTAL_RIFT;
				m_uiBossRiftSpawnLoc = 2;
				m_uiNumberOfBossAdds = 3;
				m_uiHostileFaction = m_creature->getFaction();
				m_uiSpell1Entry = 0;
				m_uiSpell2Entry = SPELL_GROUND_TREMOR;
				m_uiSpell3Entry = SPELL_TRAMPLE;
				m_uiSpell4Entry = SPELL_BOULDER;
				m_uiSpell5Entry = 0;
				m_uiYellSpawn = YELL_SPAWN_AVALANCHION;
				m_uiYellPat = YELL_PAT_AVALANCHION;
				m_uiYellDeath = YELL_DEATH_AVALANCHION;
				m_bEmoteText = false;
				break;
			case BOSS_THE_WINDREAVER:
				m_uiBossAddKind = NPC_WHIRLING_INVADER;
				m_uiRiftKind = GOBJ_WIND_RIFT;
				m_uiBossRiftKind = GOBJ_WIND_ELEMENTAL_RIFT;
				m_uiBossRiftSpawnLoc = 3;
				m_uiNumberOfBossAdds = 4;
				m_uiHostileFaction = 91;
				m_uiSpell1Entry = SPELL_SHOCK;
				m_uiSpell2Entry = SPELL_CHAIN_LIGHTNING;
				m_uiSpell3Entry = SPELL_LIGHTNING_CLOUD;
				m_uiSpell4Entry = SPELL_ENVELOPING_WINDS;
				m_uiSpell5Entry = 0;
				m_uiYellSpawn = YELL_SPAWN_THE_WINDREAVER;
				m_uiYellPat = YELL_PAT_THE_WINDREAVER;
				m_uiYellDeath = YELL_DEATH_THE_WINDREAVER;
				m_bEmoteText = true;
				break;
		}	
		// Handle event but stay hidden until its time for the boss to appear
		/*GetBossAdds();
		SetVisible(0);*/
		m_bInvasionInProgress = true;
		m_bInvaderRespawn = true;
		m_bRespawnBossAdds = false;
		m_uiHandleInvasionWaveTimer = 1000;
		Reset();
	}
    
	bool m_bInvasionInProgress;
	bool m_bInvaderRespawn;
	bool m_bRespawnBossAdds;
	bool m_bEmoteText;

	uint8 m_uiInvasionWave;
  
	uint32 m_uiHandleInvasionWaveTimer;

	std::list<ObjectGuid> m_lElementalRift;
	std::list<Creature*> m_lBossAddInvaders;
	GUIDList m_lBossInvaders;
	GUIDList m_lInvaders;

	uint32 m_uiBossAddKind;	
	uint8 m_uiNumberOfBossAdds;
	uint32 m_uiRiftKind;
	uint32 m_uiBossRiftKind;

	uint8 m_uiBossRiftSpawnLoc;
	uint8 m_uiHostileFaction;

	uint32 m_uiYellSpawn;
	uint32 m_uiYellPat;
	uint32 m_uiYellDeath;

	uint32 m_uiSpell1Entry;
	uint32 m_uiSpell2Entry;
	uint32 m_uiSpell3Entry;
	uint32 m_uiSpell4Entry;
	uint32 m_uiSpell5Entry;

	uint32 m_uiSpell1Timer;
	uint32 m_uiSpell2Timer;
	uint32 m_uiSpell3Timer;
	uint32 m_uiSpell4Timer;
	uint32 m_uiSpell5Timer;

	uint32 m_uiPatrolTextTimer;

    void Reset()
    {
		m_uiSpell1Timer = urand(1000, 5000);
		m_uiSpell2Timer = urand(4000, 8000);
		m_uiSpell3Timer = urand(9000, 15000);
		m_uiSpell4Timer = urand(13000, 16000);
		m_uiSpell5Timer = urand(1000, 2000);

		m_uiPatrolTextTimer = 10000;

		// Always try to respawn adds in case some died
		RespawnBossAdds();

        if (!m_creature->isActiveObject() && m_creature->isAlive())
            m_creature->SetActiveObjectState(true);
    }
    
	void GetBossAdds()
	{		
        GetCreatureListWithEntryInGrid(m_lBossAddInvaders, m_creature, m_uiBossAddKind, 20.f);

        if (!m_lBossAddInvaders.empty())
        {
            for (std::list<Creature*>::iterator itr = m_lBossAddInvaders.begin(); itr != m_lBossAddInvaders.end(); ++itr)
            {
				Creature* pBossInvader = *itr;
                if (pBossInvader)
                {
					pBossInvader->SetVisibility(VISIBILITY_OFF);
					pBossInvader->setFaction(35);
					pBossInvader->UpdateVisibilityAndView();
                }
            }
        }
	}

	void SetVisible(int Visible = 0)
    {
        if (Visible == 0)
        {			
			m_creature->SetVisibility(VISIBILITY_OFF);	
			m_creature->setFaction(35);
			m_creature->UpdateVisibilityAndView();
		}
		else
		{
			m_creature->SetVisibility(VISIBILITY_ON);
			m_creature->setFaction(m_uiHostileFaction);
			m_creature->UpdateVisibilityAndView();
			
			if(!m_bEmoteText)
				DoScriptText(m_uiYellSpawn,m_creature,NULL);
			else
				m_creature->MonsterEmoteToZone(m_uiYellSpawn,LANG_UNIVERSAL,NULL);

			// handle adds, make them appear
			if (!m_lBossAddInvaders.empty())
			{
				for (std::list<Creature*>::iterator itr = m_lBossAddInvaders.begin(); itr != m_lBossAddInvaders.end(); ++itr)
				{
					Creature* pBossInvader = *itr;
					if (pBossInvader)
					{
						pBossInvader->SetVisibility(VISIBILITY_ON);
						pBossInvader->setFaction(m_uiHostileFaction);
						pBossInvader->UpdateVisibilityAndView();
					}
				}
			}
		}
	}

	void RespawnBossAdds()
	{
		if(m_creature->GetVisibility() == VISIBILITY_ON)
		{
			if (!m_lBossAddInvaders.empty())
			{
				for (std::list<Creature*>::iterator itr = m_lBossAddInvaders.begin(); itr != m_lBossAddInvaders.end(); ++itr)
				{
					Creature* pBossInvader = *itr;
					if (pBossInvader && pBossInvader->isDead())
						pBossInvader->Respawn();
				}
			}
		}
	}

    void JustDied(Unit* /*pKiller*/)		// add 1day despawn timer to adds when boss dies
    {
		if(!m_bEmoteText)
			DoScriptText(m_uiYellDeath,m_creature,NULL);
		else
			m_creature->MonsterEmoteToZone(m_uiYellDeath,LANG_UNIVERSAL,NULL);
		uint32 respawn_time = urand(172800, 432000); // A random respawn time between 2 day and 5 days.
	
		m_creature->SetRespawnDelay(respawn_time);
		m_creature->SetRespawnTime(respawn_time);
		m_creature->SaveRespawnTime();

		m_bInvaderRespawn = false;

		if (!m_lBossAddInvaders.empty())
        {
            for (std::list<Creature*>::iterator itr = m_lBossAddInvaders.begin(); itr != m_lBossAddInvaders.end(); ++itr)
            {
				Creature* pBossInvader = *itr;
				// Despawn after 24h to make sure there's no left when the next invasion starts
                if (pBossInvader)
					pBossInvader->ForcedDespawn(10000);//(86400000);          
            }
        }

		for (GUIDList::iterator itr = m_lInvaders.begin(); itr != m_lInvaders.end(); itr++)
        {
            if (Creature* pInvader = m_creature->GetMap()->GetCreature(*itr))
			{			
				
                if (pInvader->isAlive())
				{
                    pInvader->ForcedDespawn(urand(10000,20000));		// make them appear a bit more randomly despawning
				}
				else
					pInvader->SetRespawnDelay(-10);
				pInvader->SetRespawnEnabled(false);
			}
        }

		RiftActions(2);
		// reset the rift guids and trash but not boss adds
		m_lElementalRift.clear();
		m_lInvaders.clear();
		//m_lBossInvaders.clear();
    }

	void SummonedCreatureJustDied(Creature* pSummoned)
    {		
		if(m_bInvaderRespawn)
		{
			pSummoned->SetRespawnEnabled(true);
			pSummoned->SetRespawnTime(600);
			pSummoned->SaveRespawnTime();
		}
		else
			pSummoned->SetRespawnEnabled(false);
    }

	void RiftActions(int Action = 0, int Pos = 0)
	{
		if(Action == 0)		// spawn a rift at position Pos
		{
			switch(m_uiRiftKind)
			{	
				case GOBJ_FIRE_RIFT:
				{
					GameObject* pRift = m_creature->SummonGameObject(m_uiRiftKind, 0, FireRiftSpawnLoc[Pos].x, FireRiftSpawnLoc[Pos].y, FireRiftSpawnLoc[Pos].z, 0.f, GO_STATE_ACTIVE);
					if (pRift)
						m_lElementalRift.push_back(pRift->GetObjectGuid());
					break;
				}
				case GOBJ_WATER_RIFT:
				{
					GameObject* pRift = m_creature->SummonGameObject(m_uiRiftKind, 0, WaterRiftSpawnLoc[Pos].x, WaterRiftSpawnLoc[Pos].y, WaterRiftSpawnLoc[Pos].z, 0.f, GO_STATE_ACTIVE);
					if (pRift)
						m_lElementalRift.push_back(pRift->GetObjectGuid());
					break;
				}
				case GOBJ_EARTH_RIFT:
				{
					GameObject* pRift = m_creature->SummonGameObject(m_uiRiftKind, 0, EarthRiftSpawnLoc[Pos].x, EarthRiftSpawnLoc[Pos].y, EarthRiftSpawnLoc[Pos].z, 0.f, GO_STATE_ACTIVE);
					if (pRift)
						m_lElementalRift.push_back(pRift->GetObjectGuid());
					break;
				}
				case GOBJ_WIND_RIFT:
				{
					GameObject* pRift = m_creature->SummonGameObject(m_uiRiftKind, 0, WindRiftSpawnLoc[Pos].x, WindRiftSpawnLoc[Pos].y, WindRiftSpawnLoc[Pos].z, 0.f, GO_STATE_ACTIVE);
					if (pRift)
						m_lElementalRift.push_back(pRift->GetObjectGuid());
					break;
				}	
			}
		}

		if(Action == 1)		// spawn the boss rift at position Pos
		{
			GameObject* pBossRift = m_creature->SummonGameObject(m_uiBossRiftKind, 0, BossRiftSpawnLoc[m_uiBossRiftSpawnLoc].x, 
				BossRiftSpawnLoc[m_uiBossRiftSpawnLoc].y, BossRiftSpawnLoc[m_uiBossRiftSpawnLoc].z, 0.f, GO_STATE_ACTIVE);
			if (pBossRift)
				m_lElementalRift.push_back(pBossRift->GetObjectGuid());
		}

		if(Action == 2)		// delete rifts
		{
			for (ObjectGuid guid : m_lElementalRift)
			{
				GameObject* pRift = dynamic_cast<TemporaryGameObject*>(m_creature->GetMap()->GetGameObject(guid));
				if (pRift)
					pRift->Delete();
			}
		}
	}

	void SpawnInvaders(int Position = 0, int MoveLoc = 0)
	{
		switch(m_uiBossAddKind)
		{			
			case NPC_BLAZING_INVADER:
			{
				// spawn invader and move it
				Creature* pInvader = m_creature->SummonCreature(m_uiBossAddKind, FireRiftSpawnLoc[Position].x,
					FireRiftSpawnLoc[Position].y,FireRiftSpawnLoc[Position].z,0.f, TEMPSUMMON_MANUAL_DESPAWN, 0);

				if(pInvader)
				{
					pInvader->GetMotionMaster()->MovePoint(1,BlazingInvaderSpawnLoc[MoveLoc].x,BlazingInvaderSpawnLoc[MoveLoc].y,BlazingInvaderSpawnLoc[MoveLoc].z);
					m_lInvaders.push_back(pInvader->GetObjectGuid());
				}
				break;
			}
			case NPC_WATERY_INVADER:
			{
				// spawn invader and move it
				Creature* pInvader = m_creature->SummonCreature(m_uiBossAddKind, WaterRiftSpawnLoc[Position].x,
					WaterRiftSpawnLoc[Position].y,WaterRiftSpawnLoc[Position].z,0.f, TEMPSUMMON_MANUAL_DESPAWN, 0);

				if(pInvader)	
				{
					pInvader->GetMotionMaster()->MovePoint(1,WateryInvaderSpawnLoc[MoveLoc].x,WateryInvaderSpawnLoc[MoveLoc].y,WateryInvaderSpawnLoc[MoveLoc].z);	
					m_lInvaders.push_back(pInvader->GetObjectGuid());
				}
				break;
			}
			case NPC_THUNDERING_INVADER:
			{
				// spawn invader and move it
				Creature* pInvader = m_creature->SummonCreature(m_uiBossAddKind, EarthRiftSpawnLoc[Position].x,
					EarthRiftSpawnLoc[Position].y,EarthRiftSpawnLoc[Position].z,0.f, TEMPSUMMON_MANUAL_DESPAWN, 0);

				if(pInvader)
				{
					pInvader->GetMotionMaster()->MovePoint(1,ThunderingInvaderSpawnLoc[MoveLoc].x,ThunderingInvaderSpawnLoc[MoveLoc].y,ThunderingInvaderSpawnLoc[MoveLoc].z);
					m_lInvaders.push_back(pInvader->GetObjectGuid());
				}
				break;
			}
			case NPC_WHIRLING_INVADER:
			{
				// spawn invader and move it
				Creature* pInvader = m_creature->SummonCreature(m_uiBossAddKind, WindRiftSpawnLoc[Position].x,
					WindRiftSpawnLoc[Position].y,WindRiftSpawnLoc[Position].z,0.f, TEMPSUMMON_MANUAL_DESPAWN, 0);

				if(pInvader)
				{
					pInvader->GetMotionMaster()->MovePoint(1,WhirlingInvaderSpawnLoc[MoveLoc].x,WhirlingInvaderSpawnLoc[MoveLoc].y,WhirlingInvaderSpawnLoc[MoveLoc].z);
					m_lInvaders.push_back(pInvader->GetObjectGuid());
				}
				break;
			}
		}			
	}

	void HandlePatText()
    {
		if(m_creature->GetVisibility() == VISIBILITY_ON)
		{
			if(!m_bEmoteText)
				DoScriptText(m_uiYellPat,m_creature,NULL);
			else
				m_creature->MonsterEmoteToZone(m_uiYellPat,LANG_UNIVERSAL,NULL);
		}
	}

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId)				// set spawn point for adds, not where they were summoned at the rift
    {
        if(uiPointId == 1)
        {
			float x, y, z;
			pSummoned->GetClosePoint(x, y, z, 1.0f, 2.0f);
			CreatureCreatePos pos(pSummoned->GetMap(), x, y, z, 0.f);
            pSummoned->SetSummonPoint(pos);
			pSummoned->GetMotionMaster()->MoveRandom();
		}
	}
			
	void HandleInvasionWave()
	{		/*  2 portaler spawnar. 2 mobs spawnar.
				(case 5)2 portaler existerar. 2 nya portaler spawnar. 4 mobs.
				(case 8)4 portaler existerar. 2 nya spawnar. 6 mobs.
				(case 12)6 portarler existerar. 6 mobs -> samtidigt spawnas stora portalen och laddar upp.
				bossen spawnar ur stora portalen.*/
		switch(m_uiInvasionWave)
		{
		case 1:
			GetBossAdds();
			SetVisible(0);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 2:			
			//spawn 1 rift
			RiftActions(0,0);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 3:
			//spawn 1 rift
			RiftActions(0,1);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 4:
			// spawn 1 mob from each rift 
			SpawnInvaders(0,0);
			SpawnInvaders(1,3);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 5:
			//spawn 2 rifts
			RiftActions(0,2);
			RiftActions(0,3);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 6:
			// spawn 1 mob from each rift 
			SpawnInvaders(2,6);
			SpawnInvaders(3,9);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 7:
			// spawn 1 mob from each rift 
			SpawnInvaders(0,1);
			SpawnInvaders(1,4);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 8:
			//spawn 2 rifts
			RiftActions(0,4);
			RiftActions(0,5);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 9:
			// spawn 1 mob from each rift 
			SpawnInvaders(4,12);
			SpawnInvaders(5,15);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 10:
			// spawn 1 mob from each rift 
			SpawnInvaders(0,2);
			SpawnInvaders(1,5);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 11:
			// spawn 1 mob from each rift 
			SpawnInvaders(2,7);
			SpawnInvaders(3,10);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 12:
			// spawn 1 mob from each rift 
			SpawnInvaders(4,13);
			SpawnInvaders(5,16);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 13:
			// spawn 1 mob from each rift 
			SpawnInvaders(2,8);
			SpawnInvaders(3,11);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 14:
			// spawn 1 mob from each rift x
			SpawnInvaders(4,14);
			SpawnInvaders(5,17);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 15:
			//spawn boss rift
			RiftActions(1,0);
			m_uiHandleInvasionWaveTimer = 5000;
			break;
		case 16:
			SetVisible(1);
			m_bInvasionInProgress = false;
			break;
		}
		m_uiInvasionWave++;
	}
    
	void Execute(uint32 uiSpellEntry)
    {
        Unit* pTarget = NULL;
        switch(uiSpellEntry)
        {
			// Only do these if in melee range
			case SPELL_CONE_OF_COLD:
			case SPELL_FROST_NOVA:
				pTarget = m_creature->getVictim();
				if(!m_creature->CanReachWithMeleeAttack(pTarget))
					pTarget = NULL;
				break;
            case SPELL_FLAME_BUFFET:
            case SPELL_FIREBALL_VOLLEY:			
			case SPELL_FROST_BOLT:
			case SPELL_GEYSER:			
			case SPELL_GROUND_TREMOR:
			case SPELL_TRAMPLE:						
                pTarget = m_creature->getVictim();
                break;
			case SPELL_BOULDER:
			case SPELL_SHOCK:
			case SPELL_CHAIN_LIGHTNING:
			case SPELL_LIGHTNING_CLOUD:
			case SPELL_ENVELOPING_WINDS:
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                break;
        }
        if (pTarget && uiSpellEntry != 0)
            DoCastSpellIfCan(pTarget, uiSpellEntry);
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if(m_bInvasionInProgress)
		{
			// Handle the event
			if (m_uiHandleInvasionWaveTimer < uiDiff)
				HandleInvasionWave();
			else
				m_uiHandleInvasionWaveTimer -= uiDiff;
		}
		else
		{
			// Handle the text while patroling
			if (m_uiPatrolTextTimer < uiDiff)
			{
				HandlePatText();
				m_uiPatrolTextTimer = 30000;
			}
			else
				m_uiPatrolTextTimer -= uiDiff;
		}

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
		
        // Spell 1
        if (m_uiSpell1Timer < uiDiff)
        {
            Execute(m_uiSpell1Entry);
            m_uiSpell1Timer = urand(5000, 10000);
        }
        else
            m_uiSpell1Timer -= uiDiff;

		// Spell 2
		if (m_uiSpell2Timer < uiDiff)
        {
            Execute(m_uiSpell2Entry);
            m_uiSpell2Timer = urand(8000, 12000);
        }
        else
            m_uiSpell2Timer -= uiDiff;

		// Spell 3
        if (m_uiSpell3Timer < uiDiff)
        {
            Execute(m_uiSpell3Entry);
            m_uiSpell3Timer = urand(10000, 15000);
        }
        else
            m_uiSpell3Timer -= uiDiff;

		// Spell 4
		if (m_uiSpell4Timer < uiDiff)
        {
            Execute(m_uiSpell4Entry);
            m_uiSpell4Timer = urand(12000, 20000);
        }
        else
            m_uiSpell4Timer -= uiDiff;

		// Spell 5 Frost bolt
		if (m_uiSpell5Timer < uiDiff)
        {
            Execute(m_uiSpell5Entry);
            m_uiSpell5Timer = urand(1000, 1500);
        }
        else
            m_uiSpell5Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_elemental_invasions(Creature* pCreature)
{
    return new boss_elemental_invasionsAI(pCreature);
}

void AddSC_elemental_invasions()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_elemental_invasions";
    pNewscript->GetAI = &GetAI_boss_elemental_invasions;
    pNewscript->RegisterSelf();
}
