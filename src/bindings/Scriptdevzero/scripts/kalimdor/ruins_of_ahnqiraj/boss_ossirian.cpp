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
SDName: Boss_Ossirian
SD%Complete: 90
SDComment: rework crystals without summoning them
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "World.h"
#include "Weather.h"
#include "ruins_of_ahnqiraj.h"

enum Yells
{
    SAY_SURPREME1                 = -1509018,
    SAY_SURPREME2                 = -1509019,
    SAY_SURPREME3                 = -1509020,
    SAY_RAND_INTRO1               = -1509021,
    SAY_RAND_INTRO2               = -1509022,
    SAY_RAND_INTRO3               = -1509023,
    SAY_RAND_INTRO4               = -1509024,
    SAY_AGGRO                     = -1509025,
    SAY_SLAY                      = -1509026,
    SAY_DEATH                     = -1509027
};

enum Spells
{
    SPELL_WAR_STOMP               = 25188,
    SPELL_CURSE_OF_TONGUES        = 25195,
    SPELL_ENVELOPING_WINGS        = 25189,
    SPELL_STRENGTH_OF_OSSIRIAN    = 25176,
    SPELL_SUMMON_PLAYER           = 20477,
    SPELL_FIRE_WEAKNESS           = 25177,
    SPELL_FROST_WEAKNESS          = 25178,
    SPELL_NATURE_WEAKNESS         = 25180,
    SPELL_ARCANE_WEAKNESS         = 25181,
    SPELL_SHADOW_WEAKNESS         = 25183,
	SPELL_SAND_STORM			  = 25160,                // tornado spell
};

static Loc Crystal[]=
{
	{-9406.92f, 1955.86f, 85.55f, 0, 0},		// 1st confirmed
	{-9357.88f, 1931.86f, 85.55f, 0, 0},		// 2nd confirmed
	{-9401.97f, 1861.20f, 85.55f, 0, 0},		// 3rd confirmed
	{-9224.90f, 1820.36f, 85.55f, 0, 0},		// 4th confirmed
	{-9281.58f, 1887.36f, 85.55f, 0, 0},		// 5th confirmed
	{-9299.33f, 1750.32f, 85.55f, 0, 0},		// 6th confirmed
	{-9427.51f, 1789.18f, 85.55f, 0, 0},		// 7th confirmed
	{-9511.07f, 1862.75f, 85.55f, 0, 0},		// 8th confirmed
	{-9376.47f, 2008.02f, 85.55f, 0, 0},		// 9th confirmed
	{-9245.51f, 1980.58f, 85.55f, 0, 0},		// 10th not confirmed
};

struct MANGOS_DLL_DECL boss_ossirianAI : public ScriptedAI
{
    boss_ossirianAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData());

        Reset();
    }

    instance_ruins_of_ahnqiraj* m_pInstance;

    uint32 m_uiWarStompTimer;
    uint32 m_uiCurseOfTonguesTimer;
    uint32 m_uiEnvelopingWingsTimer;

    std::list<uint8> m_lCrystalPos;
    std::list<ObjectGuid> m_lCrystalList;

    void Reset()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_OSSIRIAN, NOT_STARTED);
            TornadoesVisibility(1);
        }

        DeleteCrystals();
        SpawnCrystal(0);

        m_uiWarStompTimer = urand(8000,12000);
        m_uiCurseOfTonguesTimer = urand(15000,20000);
        m_uiEnvelopingWingsTimer = urand(15000,20000);
    }

	void TornadoesVisibility(int Visible = 0)
	{
		instance_ruins_of_ahnqiraj* pInstance = dynamic_cast<instance_ruins_of_ahnqiraj*>(m_creature->GetInstanceData());
		{
			if (pInstance)
			{
				for (ObjectGuid guid : pInstance->GetTornadoes())
				{
					Creature* pCreature = m_creature->GetMap()->GetCreature(guid);
					if (pCreature)
					{
						if (Visible == 0)
						{
							if (!pCreature->isAlive())
								pCreature->Respawn();
							pCreature->setFaction(14);
							pCreature->SetVisibility(VISIBILITY_ON);
						}
						else if (Visible == 1)
						{
							pCreature->setFaction(35);
							pCreature->SetVisibility(VISIBILITY_OFF);
						}
					}
				}			
			}
		}
	}

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OSSIRIAN, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_STRENGTH_OF_OSSIRIAN);

		int rand = urand(1,9);
        SpawnCrystal(rand);

        uint32 zoneid = m_creature->GetZoneId();
        SetWeather(zoneid, WeatherType(3), 2);
        
		TornadoesVisibility(0);
    }
    
    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_OSSIRIAN, DONE);
			TornadoesVisibility(1);
        }

        DeleteCrystals();
    }

	void SpawnCrystal(int SpawnPoint = 0)
	{
		GameObject* pGO = m_creature->SummonGameObject(180619, 0, Crystal[SpawnPoint].x, Crystal[SpawnPoint].y, Crystal[SpawnPoint].z, 0.f, GO_STATE_READY, 70);
        if (pGO)
        {
            pGO->SetOwnerGuid(ObjectGuid());

            // Reset the anim progress once the crystal has spawned.
            pGO->SetGoAnimProgress(GO_ANIMPROGRESS_DEFAULT);

            m_lCrystalList.push_back(pGO->GetObjectGuid());
        }
        
        m_lCrystalPos.push_back(SpawnPoint);
	}

    void DeleteCrystals()
    {
        for (ObjectGuid guid : m_lCrystalList)
        {
            TemporaryGameObject* pGo = dynamic_cast<TemporaryGameObject*>(m_creature->GetMap()->GetGameObject(guid));
            if (pGo)
                pGo->Delete(true);
        }

        m_lCrystalPos.clear();
        m_lCrystalList.clear();
    }

	void SetResistances(int School = 0, int Value = 0)
	{
		switch(School)
		{
			case 0:
				m_creature->SetResistance(SPELL_SCHOOL_FIRE, Value);
				break;
			case 1:
				m_creature->SetResistance(SPELL_SCHOOL_FROST, Value);
				break;
			case 2:
				m_creature->SetResistance(SPELL_SCHOOL_SHADOW, Value);
				break;
			case 3:
				m_creature->SetResistance(SPELL_SCHOOL_ARCANE, Value);
				break;
			case 4:
				m_creature->SetResistance(SPELL_SCHOOL_NATURE, Value);
				break;
		}
	}

    void TeleportFarAwayPlayerBack(float /*range = 0.0f*/, bool alive = true)			// not working, should it?
    {
        Map* map = m_creature->GetMap();
        if(!map->IsDungeon()) return;

        Map::PlayerList const &PlayerList = map->GetPlayers();
        if(PlayerList.isEmpty())
            return;
        for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            //if(!i->getSource()->IsWithinDistInMap(m_creature, range))
            if(i->getSource()->GetAreaId() != AREAID_WATCHERS_TERRACE)
            {
                //if(m_creature->GetAreaId() != AREAID_WATCHERS_TERRACE)
                //    DoTeleportTo(Spawns[0][0], Spawns[0][1], Spawns[0][2], 0);
                float mX, mY, mZ, mO;
                m_creature->GetPosition(mX, mY, mZ);
                mO = m_creature->GetOrientation();
                
                if(alive)
					if(i->getSource()->isAlive())
						DoTeleportPlayer(i->getSource(), mX, mY, mZ+1, mO);
					if(!alive)
						DoTeleportPlayer(i->getSource(), mX, mY, mZ+1, mO);
            }
            if(i == PlayerList.end()) i = PlayerList.begin(); // make impossible reach end
        }
    }

	void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell)			// Remove Supreme if hit by a crystal
    {
        if (pSpell->Id == SPELL_FIRE_WEAKNESS || pSpell->Id == SPELL_FROST_WEAKNESS || pSpell->Id == SPELL_NATURE_WEAKNESS ||
			pSpell->Id == SPELL_ARCANE_WEAKNESS || pSpell->Id == SPELL_SHADOW_WEAKNESS)
		{
			m_creature->RemoveAurasDueToSpell(SPELL_STRENGTH_OF_OSSIRIAN);
			m_creature->SetSpeedRate(MOVE_RUN, 0.95f);
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If has not boost nor weakness  
        if (!m_creature->HasAura(SPELL_STRENGTH_OF_OSSIRIAN, EFFECT_INDEX_0) && !m_creature->HasAura(SPELL_FIRE_WEAKNESS) && !m_creature->HasAura(SPELL_FROST_WEAKNESS) &&
			!m_creature->HasAura(SPELL_NATURE_WEAKNESS) && !m_creature->HasAura(SPELL_ARCANE_WEAKNESS) && !m_creature->HasAura(SPELL_SHADOW_WEAKNESS))
        {
            m_creature->CastSpell(m_creature, SPELL_STRENGTH_OF_OSSIRIAN, true);
			m_creature->SetSpeedRate(MOVE_RUN, 1.3f);
            switch(urand(0,2))
            {
                case 0:
                    DoScriptText(SAY_SURPREME1, m_creature);
                    break;
                case 1:
                    DoScriptText(SAY_SURPREME2, m_creature);
                    break;
                case 2:
                    DoScriptText(SAY_SURPREME3, m_creature);
                    break;
            }
        }

		if (m_creature->HasAura(SPELL_FIRE_WEAKNESS))			// adjust resistance according to what debuff he has
			SetResistances(0,0);
		else
			SetResistances(0,1000);

		if (m_creature->HasAura(SPELL_FROST_WEAKNESS))
			SetResistances(1,0);
		else
			SetResistances(1,1000);

		if (m_creature->HasAura(SPELL_SHADOW_WEAKNESS))
			SetResistances(2,0);
		else
			SetResistances(2,1000);

		if (m_creature->HasAura(SPELL_ARCANE_WEAKNESS))
			SetResistances(3,0);
		else
			SetResistances(3,1000);

		if (m_creature->HasAura(SPELL_NATURE_WEAKNESS))
			SetResistances(4,0);
		else
			SetResistances(4,1000);

        // War Stom
        if (m_uiWarStompTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WAR_STOMP);
            m_uiWarStompTimer = urand(8000,10000);
        }
        else
            m_uiWarStompTimer -= uiDiff;

        // Curse of Tongues
        if (m_uiCurseOfTonguesTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_TONGUES);
            m_uiCurseOfTonguesTimer = urand(15000,20000);
        }
        else
            m_uiCurseOfTonguesTimer -= uiDiff;

        // Enveloping Wings
        if (m_uiEnvelopingWingsTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ENVELOPING_WINGS);
            m_uiEnvelopingWingsTimer = urand(15000,20000);
        }
        else
            m_uiEnvelopingWingsTimer -= uiDiff;

        TeleportFarAwayPlayerBack(40);			// probably not working

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ossirian(Creature* pCreature)
{
    return new boss_ossirianAI(pCreature);
}

struct MANGOS_DLL_DECL npc_ossirian_dummyAI : public ScriptedAI
{
    npc_ossirian_dummyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bHasCasted = false;
    }

    bool m_bHasCasted;

    void Reset()
    {
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_bHasCasted)
        {
            if (Creature* pOssirian = GetClosestCreatureWithEntry(m_creature, NPC_OSSIRIAN, 20.0f))
            {
                switch (urand(0,4))
                {
                    case 0:
                        m_creature->CastSpell(pOssirian, SPELL_FIRE_WEAKNESS, false);
                        break;
                    case 1:
                        m_creature->CastSpell(pOssirian, SPELL_FROST_WEAKNESS, false);
                        break;
                    case 2:
                        m_creature->CastSpell(pOssirian, SPELL_NATURE_WEAKNESS, false);
                        break;
                    case 3:
                        m_creature->CastSpell(pOssirian, SPELL_ARCANE_WEAKNESS, false);
                        break;
                    case 4:
                        m_creature->CastSpell(pOssirian, SPELL_SHADOW_WEAKNESS, false);
                        break;
                }
            }

            m_bHasCasted = true;
        }
    }
};

CreatureAI* GetAI_npc_ossirian_dummy(Creature* pCreature)
{
    return new npc_ossirian_dummyAI(pCreature);
}

bool GOUse_go_ossirian_crystal(Player* pPlayer, GameObject* pGo)
{
    if (!pPlayer)
        return false;

    if (pGo->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE + GO_FLAG_INTERACT_COND))
            return true;

    pGo->SummonCreature(7080, 
            pGo->GetPositionX(),
            pGo->GetPositionY(),
            pGo->GetPositionZ(),
            0.f,
            TEMPSUMMON_TIMED_DESPAWN,
            50000, true);

    // Spawn next Ossirian Crystal at random position
   instance_ruins_of_ahnqiraj* pInstance = dynamic_cast<instance_ruins_of_ahnqiraj*>(pPlayer->GetInstanceData()); 
   if (pInstance)
   {
       Creature* pOssirian = pInstance->GetSingleCreatureFromStorage(NPC_OSSIRIAN);
       if (pOssirian)
       {
           boss_ossirianAI* pAI = dynamic_cast<boss_ossirianAI*>(pOssirian->AI());
           if (pAI)
           {
               // Get all free spots by removing any spots that are already in use.
               std::vector<short> spotList;
               for (short index = 0; index < 10; ++index)
               {
                   // Check if we're at the current crystal.
                   if (pGo->GetPositionX() == Crystal[index].x && pGo->GetPositionY() == Crystal[index].y &&
                       pGo->GetPositionZ() == Crystal[index].z)
                   {
                       pAI->m_lCrystalPos.remove(index);
                       continue;
                   } // Check if the spot is taken already.
                   else if (std::find(pAI->m_lCrystalPos.begin(), pAI->m_lCrystalPos.end(), index) != pAI->m_lCrystalPos.end())
                   {
                       continue;
                   }

                   spotList.push_back(index);
               }

               if (!spotList.empty())
                   pAI->SpawnCrystal(spotList[urand(0, spotList.size() - 1)]);
               else // Something went wrong. Clear the crystal list and spawn a random one.
               {
                   pAI->m_lCrystalPos.clear();
                   pAI->SpawnCrystal(urand(0, 9));
               }
           }
       }
   }

    pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE + GO_FLAG_INTERACT_COND); // clicked crystal become unclickable
    pGo->SetGoState(GO_STATE_ACTIVE);
    pGo->SetRespawnTime(5); // despawn after 5 secs
    
    return true;
}

/*######
## npc_sand_vortex
######*/

struct MANGOS_DLL_DECL npc_sand_vortexAI : public ScriptedAI
{
    npc_sand_vortexAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        Reset();
    }

    void Reset()
    {
		if (!m_creature->HasAura(SPELL_SAND_STORM))
			m_creature->CastSpell(m_creature, SPELL_SAND_STORM, true);
    }

	void MoveInLineOfSight(Unit* /*pWho*/)
    {
        // Must to be empty to ignore aggro
    }

	void UpdateAI(const uint32 /*uiDiff*/)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
	}
};

CreatureAI* GetAI_npc_sand_vortex(Creature* pCreature)
{
    return new npc_sand_vortexAI(pCreature);
}

void AddSC_boss_ossirian()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_ossirian";
    pNewscript->GetAI = &GetAI_boss_ossirian;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_ossirian_crystal";
    pNewscript->pGOUse = &GOUse_go_ossirian_crystal;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
	pNewscript->Name = "npc_sand_vortex";
	pNewscript->GetAI = &GetAI_npc_sand_vortex;
	pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_ossirian_dummy";
    pNewscript->GetAI = &GetAI_npc_ossirian_dummy;
    pNewscript->RegisterSelf();
}
