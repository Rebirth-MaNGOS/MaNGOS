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
SDName: Hillsbrad_Foothills
SD%Complete: 100
SDComment: Quest support: 0
SDCategory: Hillsbrad Foothils
EndScriptData */

/* ContentData
go_open_to_pass_your_rite
EndContentData */

#include "precompiled.h"

/*######
## go_open_to_pass_your_rite
######*/

enum
{
    NPC_MILTON_BEATS     = 13082,
    SPELL_CONJURE_MILTON = 21078,
};

bool GOUse_go_open_to_pass_your_rite(Player* pPlayer, GameObject* /*pGo*/)
{
    // Summon Milton Beats (randomly to 3 yards)
    pPlayer->CastSpell(pPlayer, SPELL_CONJURE_MILTON, true);

    return false;
}

/*######
## AddSC
######*/

void AddSC_hillsbrad_foothills()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "go_open_to_pass_your_rite";
    pNewscript->pGOUse = &GOUse_go_open_to_pass_your_rite;
    pNewscript->RegisterSelf();
}
