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
SDName: Boss_Dathrohan_Balnazzar
SD%Complete: 95
SDComment:
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

enum
{
    SAY_AGGRO                       = -1329017,
    SAY_TRANSFORM                   = -1329018,
    SAY_DEATH                       = -1329019,

    //Dathrohan spells
    SPELL_CRUSADERS_HAMMER          = 17286,
    SPELL_CRUSADER_STRIKE           = 17281,
    SPELL_HOLY_STRIKE               = 17284,

    //Transform
    SPELL_BALNAZZAR_TRANSFORM       = 17288,                //restore full HP/mana, trigger spell Balnazzar Transform Stun

    //Balnazzar spells
    SPELL_SHADOW_SHOCK              = 17399,
    SPELL_MIND_BLAST                = 17287,
    SPELL_PSYCHIC_SCREAM            = 13704,
    SPELL_SLEEP                     = 12098,
    SPELL_MIND_CONTROL              = 15690
};

struct MANGOS_DLL_DECL boss_dathrohan_balnazzarAI : public ScriptedAI
{
    boss_dathrohan_balnazzarAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool m_bTransformed;
    uint32 m_uiCrusadersHammerTimer;
    uint32 m_uiCrusaderStrikeTimer;
    uint32 m_uiMindBlastTimer;
    uint32 m_uiHolyStrikeTimer;
    uint32 m_uiShadowShockTimer;
    uint32 m_uiPsychicScreamTimer;
    uint32 m_uiDeepSleepTimer;
    uint32 m_uiMindControlTimer;

    void Reset()
    {
        m_bTransformed = false;
		SetEquipmentSlots(true);
        m_uiCrusadersHammerTimer = urand(3000,6000);
        m_uiCrusaderStrikeTimer = urand(9000,11000);
        m_uiMindBlastTimer = urand(1000,2000);
        m_uiHolyStrikeTimer = urand(14000,17000);
        m_uiShadowShockTimer = urand(4000,6000);
        m_uiPsychicScreamTimer = urand(9000,10000);
        m_uiDeepSleepTimer = urand(12000,14000);
        m_uiMindControlTimer = urand(17000,19000);
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustReachedHome()
    {
        if (m_creature->GetEntry() == NPC_BALNAZZAR)
            m_creature->UpdateEntry(NPC_DATHROHAN);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoScriptText(SAY_DEATH, m_creature);

        // it appears that dynflags are not properly assigned after calling UpdateEntry(), do it manually
        m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE | UNIT_DYNFLAG_TAPPED);      
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // START NOT TRANSFORMED
        if (!m_bTransformed)
        {
            // Mind Blast spell
            if (m_uiMindBlastTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_BLAST);
                m_uiMindBlastTimer = urand(10000,15000);
            }
            else
                m_uiMindBlastTimer -= uiDiff;

            // Crusader's Hammer spell
            if (m_uiCrusadersHammerTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_CRUSADERS_HAMMER);
                m_uiCrusadersHammerTimer = urand(10000,12000);
            }
            else
                m_uiCrusadersHammerTimer -= uiDiff;

            // Crusader Strike spell
            if (m_uiCrusaderStrikeTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_CRUSADER_STRIKE);
                m_uiCrusaderStrikeTimer = urand(10000,15000);
            }
            else
                m_uiCrusaderStrikeTimer -= uiDiff;

            // Holy Strike spell
            if (m_uiHolyStrikeTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_HOLY_STRIKE);
                m_uiHolyStrikeTimer = urand(10000,15000);
            }
            else
                m_uiHolyStrikeTimer -= uiDiff;

            // Balnazzar Transform
            if (HealthBelowPct(40))
            {
                //restore hp, mana and stun
                if (DoCastSpellIfCan(m_creature, SPELL_BALNAZZAR_TRANSFORM, CAST_INTERRUPT_PREVIOUS + CAST_TRIGGERED) == CAST_OK)
                {
                    m_creature->UpdateEntry(NPC_BALNAZZAR);
					SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_UNEQUIP);		// remove his sword
                    DoScriptText(SAY_TRANSFORM, m_creature);
                    m_bTransformed = true;
                }
            }
        }
        else
        {
            // Mind Blast spell
            if (m_uiMindBlastTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_BLAST);
                m_uiMindBlastTimer = urand(8000,12000);
            }
            else
                m_uiMindBlastTimer -= uiDiff;

            // Shadow Shock spell
            if (m_uiShadowShockTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_SHOCK);
                m_uiShadowShockTimer = urand(12000,15000);
            }
            else
                m_uiShadowShockTimer -= uiDiff;

            // Psychic Scream spell
            if (m_uiPsychicScreamTimer <= uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                    DoCastSpellIfCan(pTarget, SPELL_PSYCHIC_SCREAM);

                m_uiPsychicScreamTimer = urand(14000,18000);
            }
            else
                m_uiPsychicScreamTimer -= uiDiff;

            // Sleep spell
            if (m_uiDeepSleepTimer <= uiDiff)
            {
                if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                    DoCastSpellIfCan(pTarget, SPELL_SLEEP);

                m_uiDeepSleepTimer = urand(10000,14000);
            }
            else
                m_uiDeepSleepTimer -= uiDiff;

            // Mind Control spell
            if (m_uiMindControlTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_CONTROL);
                m_uiMindControlTimer = urand(14000,17000);
            }
            else
                m_uiMindControlTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dathrohan_balnazzar(Creature* pCreature)
{
    return new boss_dathrohan_balnazzarAI(pCreature);
}

void AddSC_boss_dathrohan_balnazzar()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_dathrohan_balnazzar";
    pNewscript->GetAI = &GetAI_boss_dathrohan_balnazzar;
    pNewscript->RegisterSelf();
}
