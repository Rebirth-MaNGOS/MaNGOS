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
SDName: npc_building_mount_guard
SD%Complete: 100
SDComment: Invisible npc which dismount players
SDCategory: NPCs
EndScriptData */

#include "precompiled.h"

enum Spells
{
    SPELL_TRAVEL_FORM       = 783,
    SPELL_GHOST_WOLF        = 2645,
    SPELL_WARSONG_FLAG      = 23333,
    SPELL_SILVERWING_FLAG   = 23335,
    SPELL_DIVINE_SHIELD     = 642
};

struct MANGOS_DLL_DECL npc_building_mount_guardAI : public Scripted_NoMovementAI
{
    npc_building_mount_guardAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) {}

    void Reset() {}

    void MoveInLineOfSight(Unit *pWho)
    {
        if (pWho->GetTypeId() != TYPEID_PLAYER || !m_creature->IsWithinDistInMap(pWho, 10.0f) || !m_creature->IsWithinLOSInMap(pWho))
            return;

        Player* pPlayer = (Player*)pWho;

        if (pPlayer->IsMounted())
        {
            pPlayer->Unmount();
            pPlayer->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
        }

        if (pPlayer->HasAura(SPELL_TRAVEL_FORM, EFFECT_INDEX_0))
            pPlayer->RemoveAurasDueToSpell(SPELL_TRAVEL_FORM);

        if (pPlayer->HasAura(SPELL_GHOST_WOLF, EFFECT_INDEX_0))
            pPlayer->RemoveAurasDueToSpell(SPELL_GHOST_WOLF);

        if (!pPlayer->InBattleGround())
            return;

        if (pPlayer->HasAura(SPELL_WARSONG_FLAG, EFFECT_INDEX_0) || pPlayer->HasAura(SPELL_SILVERWING_FLAG, EFFECT_INDEX_0))
        {
            if (pPlayer->HasAura(SPELL_DIVINE_SHIELD, EFFECT_INDEX_0))
            {
                pPlayer->RemoveAurasDueToSpell(SPELL_DIVINE_SHIELD);

                if (BattleGround *bg = pPlayer->GetBattleGround())
                    bg->EventPlayerDroppedFlag(pPlayer);
            }
        }
    }
};

CreatureAI* GetAI_npc_building_mount_guard(Creature* pCreature)
{
    return new npc_building_mount_guardAI(pCreature);
}

void AddSC_npc_building_mount_guard()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_building_mount_guard";
    pNewscript->GetAI = &GetAI_npc_building_mount_guard;
    pNewscript->RegisterSelf();
}
