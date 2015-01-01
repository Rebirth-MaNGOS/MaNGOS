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
SDName: Blackfathom_Deeps
SD%Complete: 10
SDComment: Quest support: 6921,
SDCategory: Blackfathom Deeps
EndScriptData */

/* ContentData
go_blackfathom_altar
go_blackfathom_fire
go_fathom_stone
EndContentData */

#include "precompiled.h"
#include "blackfathom_deeps.h"

/*######
## go_blackfathom_altar
## go_blackfathom_fire
######*/

enum Spells
{
    SPELL_BLESSING_OF_BLACKFATHOM             = 8733,
    SPELL_BLACKFATHOM_TELEPORT                = 8735
};

static float north_spawns[3][3] =
{
	{-768.80f, -164.19f, -25.87f},
	{-769.01f, -174.560f, -25.87f},
	{-768.88f, -153.97f, -25.88f}
};

static float south_spawns[3][3] =
{
	{-868.06f, -164.04f, -25.88f},
	{-867.99f, -153.96f, -25.88f},
	{-868.26f, -174.47f, -25.87f}
};

bool GOUse_go_blackfathom_altar(Player* pPlayer, GameObject* pGo)
{
    if (!pPlayer->HasAura(SPELL_BLESSING_OF_BLACKFATHOM))
        pPlayer->CastSpell(pPlayer, SPELL_BLESSING_OF_BLACKFATHOM, true);

    if (pGo->GetEntry() == GO_ALTAR_OF_THE_DEEPS)
        pPlayer->CastSpell(pPlayer, SPELL_BLACKFATHOM_TELEPORT, true);

    return true;
}

//function only accept count of 2,3,4 and 8
void summon_fire_of_aku_mai_mobs(GameObject* summoner, int mob, int count)
{
	int SwitchSpawnCount;
	int t;

	if (count != 2 && count != 3 && count != 4 && count != 8)
		return;

	if (count == 3)
		SwitchSpawnCount = 1;
	else
		SwitchSpawnCount = count/2;

	for (int i = 0; i < count; ++i)
	{						
		if (i < SwitchSpawnCount)
		{
			 if(i > 2)
				 t = urand(0,2);
			 else
				 t = i;

			if (Creature* pTemp = summoner->SummonCreature(mob, north_spawns[t][0], north_spawns[t][1], north_spawns[t][2], 1.59f, TEMPSUMMON_DEAD_DESPAWN, 5000))
				pTemp->SetInCombatWithZone();
		}
		else
		{
			if((i-SwitchSpawnCount) > 2)
				 t = urand(0,2);
			 else
				 t = (i-SwitchSpawnCount);

			if (Creature* pTemp = summoner->SummonCreature(mob, south_spawns[t][0], south_spawns[t][1], south_spawns[t][2], 1.59f, TEMPSUMMON_DEAD_DESPAWN, 5000))
				pTemp->SetInCombatWithZone();
		}
	}
}


bool GOUse_go_blackfathom_fire(Player* /*pPlayer*/, GameObject* pGo)
{
    instance_blackfathom_deeps* m_pInstance = (instance_blackfathom_deeps*)pGo->GetInstanceData();
    if (m_pInstance)
	{
		pGo->SetGoState(GO_STATE_ACTIVE);
        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
		switch(pGo->GetEntry())
		{
			case GO_FIRE_OF_AKU_MAI_1 :
				summon_fire_of_aku_mai_mobs(pGo, FIRE_OF_AKU_MAI_MOB_MAKURA, 8);
					return true;
			case GO_FIRE_OF_AKU_MAI_2 :
				summon_fire_of_aku_mai_mobs(pGo, FIRE_OF_AKU_MAI_MOB_CRAB, 4);
					return true;
			case GO_FIRE_OF_AKU_MAI_3 :
				summon_fire_of_aku_mai_mobs(pGo, FIRE_OF_AKU_MAI_MOB_TURTLE, 3);
					return true;
			case GO_FIRE_OF_AKU_MAI_4 :
				summon_fire_of_aku_mai_mobs(pGo, FIRE_OF_AKU_MAI_MOB_ELEMENTAL, 2);
					return true;
		}
    }
    return false;
}

/*######
## go_fathom_stone
######*/

bool GOUse_go_fathom_stone(Player* pPlayer, GameObject* pGo)
{
    if (pGo->GetUseCount() == 0)
    {
		Position pos;
		pGo->GetClosePoint(pos.x, pos.y, pos.z, pPlayer->GetObjectBoundingRadius());    // possible crash :)
		pGo->SummonCreature(NPC_BARON_AQUANIS, pos.x, pos.y, pos.z, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
	}
	pGo->AddUse();
	return true;
}

/* AddSC */

void AddSC_blackfathom_deeps()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_blackfathom_altar";
    pNewScript->pGOUse = &GOUse_go_blackfathom_altar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_blackfathom_fire";
    pNewScript->pGOUse = &GOUse_go_blackfathom_fire;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_fathom_stone";
    pNewScript->pGOUse = &GOUse_go_fathom_stone;
    pNewScript->RegisterSelf();
}
