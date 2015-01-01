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
SDName: Scarlet_Monastery
SD%Complete: 100
SDComment:
SDCategory: Scarlet Monastery
EndScriptData */

/* ContentData
at_sm_cathedral_entrance
EndContentData */

#include "precompiled.h"
#include "scarlet_monastery.h"

/*######
## at_sm_cathedral_entrance
######*/

bool AreaTrigger_at_sm_cathedral_entrance(Player* pPlayer, AreaTriggerEntry const* /*pAt*/)
{
    instance_scarlet_monastery* m_pInstance = (instance_scarlet_monastery*)pPlayer->GetInstanceData();

    if (!m_pInstance)
        return false;

    if (pPlayer->HasAura(SPELL_ASHBRINGER))
    {
        if (m_pInstance->GetData(TYPE_ASH_WELCOME_YELL) != DONE)
        {
			if (Creature* pMograine = m_pInstance->GetSingleCreatureFromStorage(NPC_MOGRAINE))
            {
                if (pMograine->isAlive())
                {
                    m_pInstance->SetData(TYPE_ASH_WELCOME_YELL, DONE);
                    pMograine->MonsterYellToZone(SAY_MOGRAINE_ENTER_INSTANCE, LANG_UNIVERSAL, NULL);
                }
            }
        }

		m_pInstance->HandleGameObject(GO_CHAPEL_DOOR, true);
    }
    return true;
}

/* AddSC */

void AddSC_scarlet_monastery()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name="at_sm_cathedral_entrance";
    pNewScript->pAreaTrigger = &AreaTrigger_at_sm_cathedral_entrance;
    pNewScript->RegisterSelf();
}
