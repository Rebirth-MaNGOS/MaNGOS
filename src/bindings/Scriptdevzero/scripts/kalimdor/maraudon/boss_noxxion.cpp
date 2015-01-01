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
SDName: Boss_Noxxion
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "precompiled.h"

enum eSpells
{
	SPELL_TOXICVOLLEY			= 21687,
	SPELL_UPPERCUT				= 22916
};

struct MANGOS_DLL_DECL boss_noxxionAI : public ScriptedAI
{
    boss_noxxionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bInvisible = false;
        Reset();
    }

    uint32 m_uiToxicVolleyTimer;
    uint32 m_uiUppercutTimer;
    uint32 m_uiAddsTimer;
    uint32 m_uiInvisibleTimer;
    bool m_bInvisible;

    void Reset()
    {
        if (m_bInvisible)
        {
            m_bInvisible = false;
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetVisibility(VISIBILITY_ON);
        }
        m_uiToxicVolleyTimer = 7000;
        m_uiUppercutTimer = 16000;
        m_uiAddsTimer = 19000;
        m_uiInvisibleTimer = 15000;                            //Too much too low?
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bInvisible && m_uiInvisibleTimer <= uiDiff)
        {
            // Become visible again
            m_bInvisible = false;
            m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            // Noxxion model
            //m_creature->SetDisplayId(11172);
            m_creature->SetVisibility(VISIBILITY_ON);
        }
		else if (m_bInvisible)
        {
            m_uiInvisibleTimer -= uiDiff;
            // Do nothing while invisible
            return;
        }

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Toxic Volley
        if (m_uiToxicVolleyTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_TOXICVOLLEY);
            m_uiToxicVolleyTimer = 9000;
        }
		else
            m_uiToxicVolleyTimer -= uiDiff;

        // Uppercut
        if (m_uiUppercutTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_UPPERCUT);
            m_uiUppercutTimer = 12000;
        }
		else
			m_uiUppercutTimer -= uiDiff;

        // Adds
        if (!m_bInvisible && m_uiAddsTimer <= uiDiff)
        {
            // Remove all auras
            m_creature->RemoveAllAuras();

            // Inturrupt any spell casting
            m_creature->InterruptNonMeleeSpells(false);
            m_creature->setFaction(35);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            // Invisible Model
            //m_creature->SetDisplayId(11686);
            m_creature->SetVisibility(VISIBILITY_OFF);

			for(uint8 itr = 0; itr < 5; ++itr)
			{
				if (Creature* pSummoned = DoSpawnCreature(13456, irand(-10,10), irand(-10,10), 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 90000))
					pSummoned->AI()->AttackStart(m_creature->getVictim());
			}

            m_bInvisible = true;
            m_uiInvisibleTimer = 15000;

            m_uiAddsTimer = 40000;
        }
		else
			m_uiAddsTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_noxxion(Creature* pCreature)
{
    return new boss_noxxionAI(pCreature);
}

void AddSC_boss_noxxion()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_noxxion";
    pNewscript->GetAI = &GetAI_boss_noxxion;
    pNewscript->RegisterSelf();
}
