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
SDName: Boss_Celebras_the_Cursed
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "precompiled.h"
#include "maraudon.h"

enum Spells
{
    SPELL_WRATH                 = 21807,
    SPELL_ENTANGLING_ROOTS      = 12747,
    SPELL_CORRUPT_FORCES        = 21968,
    SPELL_TWISTED_TRANQUILITY   = 21793,

	NPC_CELEBRAS_GHOST			= 13716
};

struct MANGOS_DLL_DECL boss_celebras_the_cursedAI : public ScriptedAI
{
    boss_celebras_the_cursedAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_maraudon*)pCreature->GetInstanceData();
        Reset();
    }
    
    instance_maraudon* m_pInstance;

    uint32 m_uiWrathTimer;
    uint32 m_uiEntanglingRootsTimer;
    uint32 m_uiCorruptForcesTimer;
    uint32 m_uiTwistedTranquilityTimer;

    void Reset()
    {
        m_uiWrathTimer = 0;
        m_uiEntanglingRootsTimer = 2000;
        m_uiCorruptForcesTimer = 20000;
        m_uiTwistedTranquilityTimer = 10000;
    }
    
    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CELEBRAS_THE_CURSED, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CELEBRAS_THE_CURSED, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CELEBRAS_THE_CURSED, DONE);

		//m_creature->SummonCreature(NPC_CELEBRAS_GHOST,m_creature->GetPositionX()+3,m_creature->GetPositionY()+3,m_creature->GetPositionZ()+1,0,TEMPSUMMON_TIMED_DESPAWN,1800000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Wrath
        if (m_uiWrathTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_WRATH);
            m_uiWrathTimer = 8000;
        }
		else
			m_uiWrathTimer -= uiDiff;

        // Entangling Roots
        if (m_uiEntanglingRootsTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ENTANGLING_ROOTS);
            m_uiEntanglingRootsTimer = 10000;
        }
		else
			m_uiEntanglingRootsTimer -= uiDiff;

        // Corrupt Forces
        if (m_uiCorruptForcesTimer < uiDiff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCastSpellIfCan(m_creature, SPELL_CORRUPT_FORCES);
            m_uiCorruptForcesTimer = 12000;
        }
		else
			m_uiCorruptForcesTimer -= uiDiff;
        
        // Twisted Tranquility
        if (m_uiTwistedTranquilityTimer < uiDiff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_TWISTED_TRANQUILITY);
            m_uiTwistedTranquilityTimer = 15000;
        }
		else
			m_uiTwistedTranquilityTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_celebras_the_cursed(Creature* pCreature)
{
    return new boss_celebras_the_cursedAI(pCreature);
}

void AddSC_boss_celebras_the_cursed()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_celebras_the_cursed";
    pNewscript->GetAI = &GetAI_boss_celebras_the_cursed;
    pNewscript->RegisterSelf();
}
