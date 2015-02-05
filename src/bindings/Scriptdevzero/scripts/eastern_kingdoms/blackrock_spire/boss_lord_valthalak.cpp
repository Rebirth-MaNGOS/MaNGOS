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
SDName: Boss Lord Valthalak
SD%Complete: 95%
SDComment: Missing transition to phase 2, needs Yell + flag cannot target for 10sec while "he changes weapon".
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

enum eLordValthalak
{
    CASTER                          = 0,
    ENRAGE                          = 1,
    AOE                             = 2,

	EQUIP_ID_STAFF2					= 34891,

    SPELL_SHADOW_BOLT_VOLLEY        = 27383,
    SPELL_SHADOW_STAFF              = 27338,
    SPELL_SHADOW_WRATH              = 27286,
    SPELL_SUMMON_SPECTRAL_ASSASSIN  = 27249,
};

struct MANGOS_DLL_DECL boss_lord_valthalakAI : public ScriptedAI
{
    boss_lord_valthalakAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_spire*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_spire* m_pInstance;

    uint32 m_uiShadowBoltVolleyTimer;
    uint32 m_uiShadowStaffTimer;
    uint32 m_uiShadowWrathTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiPhase;

    void Reset()
    {
        m_uiShadowBoltVolleyTimer = 1000;
        m_uiShadowStaffTimer = urand(6000,10000);
        m_uiShadowWrathTimer = urand(8000,12000);
        m_uiSummonTimer = urand(8000,10000);
        m_uiPhase = CASTER;
		SetEquipmentSlots(true);
    }

    void JustReachedHome()
    {
        m_creature->RemoveFromWorld();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_LORD_VALTHALAK, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LORD_VALTHALAK, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LORD_VALTHALAK, DONE);
    }

    void JustSummoned(Creature* pSummoned)
    {
        Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
        if (!m_creature->getVictim() && !pTarget)
            return;

        pSummoned->AI()->AttackStart(pTarget ? pTarget : m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return; ;

        if (HealthBelowPct(40))
		{
			SetEquipmentSlots(false, EQUIP_ID_STAFF2, EQUIP_UNEQUIP, EQUIP_UNEQUIP);
			const CreatureInfo* cinfo = m_creature->GetCreatureInfo();
            m_creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 30)));
            m_creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 30)));
            m_creature->UpdateDamagePhysical(BASE_ATTACK);
            m_uiPhase = ENRAGE;
		}

        if (HealthBelowPct(15))
            m_uiPhase = AOE;

        // Summon Spectral Assassin
        if (m_uiPhase == CASTER)
	{
            if (m_uiSummonTimer < uiDiff)
            {
                for(uint8 i = 0; i < 2; ++i)
                    DoSpawnCreature(NPC_SPECTRAL_ASSASSIN, irand(-5,5), irand(-5,5), 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);

                m_uiSummonTimer = urand(8000,10000);
            }
            else
                m_uiSummonTimer -= uiDiff;
	}

        // Shadow Staff
        if (m_uiPhase == ENRAGE)
	{
            if (m_uiShadowStaffTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_STAFF);
                m_uiShadowStaffTimer = urand(6000,10000);
            }
            else
                m_uiShadowStaffTimer -= uiDiff;
	}

        // Shadow Wrath
        if (m_uiPhase != AOE)
	{
            if (m_uiShadowWrathTimer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_WRATH);
                m_uiShadowWrathTimer = urand(8000,12000);
            }
            else
                m_uiShadowWrathTimer -= uiDiff;
	}

        // Shadow Bolt Volley
        if (m_uiPhase == AOE)
	{
            if (m_uiShadowBoltVolleyTimer < uiDiff)
            {
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_SHADOW_BOLT_VOLLEY);
				
                m_uiShadowBoltVolleyTimer = 1500;
            }
            else
                m_uiShadowBoltVolleyTimer -= uiDiff;
	}

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lord_valthalak(Creature* pCreature)
{
    return new boss_lord_valthalakAI (pCreature);
}

void AddSC_boss_lord_valthalak()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_lord_valthalak";
    pNewscript->GetAI = &GetAI_boss_lord_valthalak;
    pNewscript->RegisterSelf();
}
