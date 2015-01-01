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
SDName: Boss Lethtendris
SD%Complete:
SDComment:
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

enum Spells
{
    // Lethtendris
    SPELL_CURSE_OF_THORNS         = 16247,
    SPELL_CURSE_OF_TONGUES        = 13338,
    SPELL_ENLARGE                 = 22710,
    SPELL_IMMOLATE                = 12742,
    SPELL_SHADOW_BOLT_VOLLEY      = 17228,
    SPELL_BOLT_VOID               = 22709,

    // Pimgib
    SPELL_BLAST_WAVE              = 16046,
    SPELL_FIREBALL                = 15228,
    SPELL_FLAME_BUFFET            = 22713,
};

struct MANGOS_DLL_DECL boss_lethtendrisAI : public ScriptedAI
{
    boss_lethtendrisAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    bool m_bIsEnraged;
    uint32 m_uiCurseTimer;
    uint32 m_uiImmolateTimer;
    uint32 m_uiEnlargeTimer;
    uint32 m_uiShadowBoltVolleyTimer;
    uint32 m_uiBoltVoidTimer;

    void Reset()
    {
        m_bIsEnraged = false;
        m_uiCurseTimer = urand(3000,4000);
        m_uiImmolateTimer = urand(5000,7000);
        m_uiEnlargeTimer = 10000;
        m_uiShadowBoltVolleyTimer = urand(10000,12000);
        m_uiBoltVoidTimer = urand(7000,9000);
        if (m_pInstance)
		{
			Creature* pPimgib = m_pInstance->GetSingleCreatureFromStorage(NPC_PIMGIB);
			if (pPimgib && !pPimgib->isAlive())
				pPimgib->Respawn();
		}
        
    }

    void Aggro(Unit* /*pWho*/)
    {
        m_creature->CallForHelp(30.0f);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bIsEnraged)
	{
            if (m_uiEnlargeTimer <= uiDiff)
            {
                if (m_pInstance)
                    if (Creature* pPimgib = m_pInstance->GetSingleCreatureFromStorage(NPC_PIMGIB))
                    {
                        DoCastSpellIfCan(pPimgib, SPELL_ENLARGE);
                        pPimgib->AI()->AttackStart(m_creature->getVictim());
                    }

                m_bIsEnraged = true;
            }
            else
                m_uiEnlargeTimer -= uiDiff;
	}

        // Curses
        if (m_uiCurseTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), urand(0,1) ? SPELL_CURSE_OF_THORNS : SPELL_CURSE_OF_TONGUES);
            m_uiCurseTimer = 20000;
        }
        else
            m_uiCurseTimer -= uiDiff;

        // Immolate
        if (m_uiImmolateTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_IMMOLATE);
            m_uiImmolateTimer = urand(6000,8000);
        }
        else
            m_uiImmolateTimer -= uiDiff;

        // Shadow Bolt Volley
        if (m_uiShadowBoltVolleyTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT_VOLLEY);
            m_uiShadowBoltVolleyTimer = urand(8000,10000);
        }
        else
            m_uiShadowBoltVolleyTimer -= uiDiff;

        // Bolt Void
        if (m_uiBoltVoidTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_BOLT_VOID);
            m_uiBoltVoidTimer = urand(6000,8000);
        }
        else
            m_uiBoltVoidTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lethtendris(Creature* pCreature)
{
    return new boss_lethtendrisAI(pCreature);
}

struct MANGOS_DLL_DECL mob_pimgibAI : public ScriptedAI
{
    mob_pimgibAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiBlastWaveTimer;
    uint32 m_uiFireballTimer;
    uint32 m_uiFlameBuffetTimer;

    void Reset()
    {
        m_uiBlastWaveTimer = urand(3000,4000);
        m_uiFireballTimer = urand(5000,6000);
        m_uiFlameBuffetTimer = urand(7000,8000);
    }

    void Aggro(Unit* /*pWho*/)
    {
        m_creature->CallForHelp(30.0f);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Blast Wave
        if (m_uiBlastWaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLAST_WAVE);
            m_uiBlastWaveTimer = urand(7000,9000);
        }
        else
            m_uiBlastWaveTimer -= uiDiff;

        // Fireball
        if (m_uiFireballTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL);
            m_uiFireballTimer = urand(4000,6000);
        }
        else
            m_uiFireballTimer -= uiDiff;

        // Flame Buffet
        if (m_uiFlameBuffetTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FLAME_BUFFET);
            m_uiFlameBuffetTimer = urand(5000,7000);
        }
        else
            m_uiFlameBuffetTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_pimgib(Creature* pCreature)
{
    return new mob_pimgibAI(pCreature);
}

void AddSC_boss_lethtendris()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_lethtendris";
    pNewscript->GetAI = &GetAI_boss_lethtendris;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_pimgib";
    pNewscript->GetAI = &GetAI_mob_pimgib;
    pNewscript->RegisterSelf();
}
