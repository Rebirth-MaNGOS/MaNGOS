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
SDName: Boss Marduk Blackpool
SD%Complete:
SDComment: 
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_CLEAVE                = 15284,
    SPELL_DEFILING_AURA         = 17695,
    SPELL_SHADOW_BOLT_VOLLEY    = 17228,
    SPELL_SHADOW_SHIELD         = 12040
};

struct MANGOS_DLL_DECL boss_marduk_blackpoolAI : public ScriptedAI
{
    boss_marduk_blackpoolAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiCleaveTimer;
    uint32 m_uiShadowBoltVolleyTimer;
    uint32 m_uiShadowShieldTimer;

    void Reset()
    {
        m_uiCleaveTimer = urand(2000,4000);
        m_uiShadowBoltVolleyTimer = urand(6000,10000);
        m_uiShadowShieldTimer = urand(15000,25000);
        m_creature->RemoveAllAuras();
    }

    void Aggro(Unit* pWho)
    {
        std::list<Creature*> m_lStudent;
        GetCreatureListWithEntryInGrid(m_lStudent, m_creature, NPC_SCHOLOMANCE_STUDENT, DEFAULT_VISIBILITY_INSTANCE);
        if (!m_lStudent.empty())
        {
            for(std::list<Creature*>::iterator itr = m_lStudent.begin(); itr != m_lStudent.end(); ++itr)
            {
                if ((*itr) && (*itr)->isAlive())
                {
                    (*itr)->setFaction(FACTION_HOSTILE);
                    (*itr)->SetInCombatWithZone();
                    (*itr)->AI()->AttackStart(pWho);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoCastSpellIfCan(m_creature, SPELL_DEFILING_AURA, CAST_AURA_NOT_PRESENT);

        // Cleave
        if (m_uiCleaveTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_CLEAVE);
            m_uiCleaveTimer = urand(4000,8000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Shadow Bolt Volley
        if (m_uiShadowBoltVolleyTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT_VOLLEY);
            m_uiShadowBoltVolleyTimer = urand(10000,12000);
        }
        else
            m_uiShadowBoltVolleyTimer -= uiDiff;

        // Shadow Shield
        if (m_uiShadowShieldTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_SHIELD);
            m_uiShadowShieldTimer = urand(35000,50000);
        }
        else
            m_uiShadowShieldTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_marduk_blackpool(Creature* pCreature)
{
    return new boss_marduk_blackpoolAI(pCreature);
}

void AddSC_boss_marduk_blackpool()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_marduk_blackpool";
    pNewscript->GetAI = &GetAI_boss_marduk_blackpool;
    pNewscript->RegisterSelf();
}
