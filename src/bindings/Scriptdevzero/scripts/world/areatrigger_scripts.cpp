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
SDName: Areatrigger_Scripts
SD%Complete: 100
SDComment: PvP Barracks ports, Quest support: 6681
SDCategory: Areatrigger
EndScriptData */

/* ContentData
at_pvp_barracks
at_ravenholdt
EndContentData */

#include "precompiled.h"

/*######
## at_pvp_barracks
######*/

#define BARRACKS_ALLIANCE  "Champion's Hall"
#define BARRACKS_HORDE  "Hall of Legends"

enum eBarracks
{
    AT_CHAMPIONS_HALL   = 2532,
    AT_HALL_OF_LEGENDS  = 2527,
    HONOR_RANK_REQUIRED = 10,
};

bool AreaTrigger_at_pvp_barracks(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->GetHonorRankInfo().rank < HONOR_RANK_REQUIRED)
    {
        pPlayer->GetSession()->SendAreaTriggerMessage("Only officers are allowed to enter %s. %s, come back later when you earn more honor!", pAt->id == AT_CHAMPIONS_HALL ? BARRACKS_ALLIANCE : BARRACKS_HORDE, pPlayer->GetName());
        return false;
    }

    switch(pAt->id)
    {
        case AT_CHAMPIONS_HALL:
            pPlayer->TeleportTo(449, -0.299116f, 4.39156f, -0.255884f, 1.54805f, TELE_TO_NOT_UNSUMMON_PET);
            break;
        case AT_HALL_OF_LEGENDS:
            pPlayer->TeleportTo(450, 221.322f, 74.4933f, 25.7195f, 0.484836f, TELE_TO_NOT_UNSUMMON_PET);
            break;
    }
    return true;
}

/*######
## at_ravenholdt
######*/

enum eRavenhold
{
    QUEST_MANOR_RAVENHOLDT  = 6681,
    NPC_RAVENHOLDT          = 13936
};

bool AreaTrigger_at_ravenholdt(Player* pPlayer, AreaTriggerEntry const* /*pAt*/)
{
    if (pPlayer->GetQuestStatus(QUEST_MANOR_RAVENHOLDT) == QUEST_STATUS_INCOMPLETE)
        pPlayer->KilledMonsterCredit(NPC_RAVENHOLDT);

    return false;
}

void AddSC_areatrigger_scripts()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "at_pvp_barracks";
    pNewscript->pAreaTrigger = &AreaTrigger_at_pvp_barracks;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "at_ravenholdt";
    pNewscript->pAreaTrigger = &AreaTrigger_at_ravenholdt;
    pNewscript->RegisterSelf();
}
