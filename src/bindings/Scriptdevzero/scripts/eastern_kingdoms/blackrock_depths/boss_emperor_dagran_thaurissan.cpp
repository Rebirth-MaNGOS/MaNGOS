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
SDName: Boss_Emperor_Dagran_Thaurissan
SD%Complete: 100
SDComment: With script for Moria
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"
#include "blackrock_depths.h"

enum eEmperor
{
    YELL_AGGRO_1                = -1230001,
    YELL_AGGRO_2                = -1230064,
    YELL_AGGRO_3                = -1230065,
    YELL_SLAY                   = -1230002,

    SPELL_HAND_OF_THAURISSAN    = 17492,
    SPELL_AVATAR_OF_FLAME       = 15636,
};

struct MANGOS_DLL_DECL boss_emperor_dagran_thaurissanAI : public ScriptedAI
{
    boss_emperor_dagran_thaurissanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_depths* m_pInstance;

    uint32 m_uiHandOfThaurissanTimer;
    uint32 m_uiAvatarOfFlameTimer;

    void Reset()
    {
        m_uiHandOfThaurissanTimer = 4000;
        m_uiAvatarOfFlameTimer = 25000;
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
			m_pInstance->SetData(TYPE_IMPERIAL_SEAT, IN_PROGRESS);
        
        uint32 uiTextId;
        switch (urand(0, 2))
        {
            case 0: uiTextId = YELL_AGGRO_1; break;
            case 1: uiTextId = YELL_AGGRO_2; break;
            default: uiTextId = YELL_AGGRO_3; break;
        }
        DoScriptText(uiTextId, m_creature);
    }

    void JustDied(Unit* /*pVictim*/)
    {
        if (!m_pInstance)
            return;

		if (Creature* pPrincess = m_pInstance->GetSingleCreatureFromStorage(NPC_PRINCESS))
        {
            pPrincess->setFaction(FACTION_FRIENDLY_FOR_ALL);
            if (pPrincess->IsNonMeleeSpellCasted(false))
                pPrincess->InterruptNonMeleeSpells(false);
            if (pPrincess->isAlive() && pPrincess->isInCombat())
                pPrincess->AI()->ResetToHome();
        }

		m_pInstance->SetData(TYPE_IMPERIAL_SEAT, DONE);
    }

	void JustReachedHome()
	{
		if (m_pInstance)
			m_pInstance->SetData(TYPE_IMPERIAL_SEAT, FAIL);
	}

    void KilledUnit(Unit *)
    {
        DoScriptText(YELL_SLAY, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Hand of Thaurissan
        if (m_uiHandOfThaurissanTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_HAND_OF_THAURISSAN);

            m_uiHandOfThaurissanTimer = urand(5000, 10000);
        }
        else
            m_uiHandOfThaurissanTimer -= uiDiff;

        // Avatar of Flame
        if (m_uiAvatarOfFlameTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_AVATAR_OF_FLAME);
            m_uiAvatarOfFlameTimer = 18000;
        }
        else
            m_uiAvatarOfFlameTimer -= uiDiff;
		m_creature->CallForHelp(120.f);
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_emperor_dagran_thaurissan(Creature* pCreature)
{
    return new boss_emperor_dagran_thaurissanAI(pCreature);
}

/*######
## boss_moira_bronzebeard
######*/

enum ePrincess
{
    SPELL_HEAL                      = 15586,
    SPELL_RENEW                     = 10929,
    SPELL_MINDBLAST                 = 15587,
    SPELL_SHADOWWORDPAIN            = 15654,
    SPELL_SHADOW_BOLT               = 15537,
    SPELL_PRINCESS_SUMMONS_PORTAL   = 13912
};

struct MANGOS_DLL_DECL boss_moira_bronzebeardAI : public ScriptedAI
{
    boss_moira_bronzebeardAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_depths* m_pInstance;

    uint32 m_uiHealTimer;
	uint32 m_uiRenewTimer;
    uint32 m_uiMindBlastTimer;
    uint32 m_uiShadowWordPainTimer;
	uint32 m_uiShadowBoltTimer;

    void Reset()
    {
        m_uiHealTimer = 12000;                                 //These times are probably wrong
		m_uiRenewTimer = 10000;
        m_uiMindBlastTimer = 16000;
        m_uiShadowWordPainTimer = 2000;
		m_uiShadowBoltTimer = 8000;
    }

    void Aggro(Unit* /*pWho*/) {}

    void JustReachedHome()
    {
        if (m_pInstance)
		{
			Creature* pEmperor = m_pInstance->GetSingleCreatureFromStorage(NPC_EMPEROR);
			if (pEmperor && !pEmperor->isAlive())
				DoCastSpellIfCan(m_creature, SPELL_PRINCESS_SUMMONS_PORTAL, CAST_INTERRUPT_PREVIOUS);
		}
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mind Blast
        if (m_uiMindBlastTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MINDBLAST);
            m_uiMindBlastTimer = 14000;
        }
        else
            m_uiMindBlastTimer -= uiDiff;

		// Renew
        if (m_uiRenewTimer <= uiDiff)
        {
            if (Unit* pAlliedSoldier = DoSelectLowestHpFriendly(40.0f, 500))
				DoCastSpellIfCan(pAlliedSoldier, SPELL_RENEW);

            m_uiRenewTimer = 30000;
        }
        else
            m_uiRenewTimer -= uiDiff;

        // Shadow Word: Pain
        if (m_uiShadowWordPainTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOWWORDPAIN);
            m_uiShadowWordPainTimer = 18000;
        }
        else
            m_uiShadowWordPainTimer -= uiDiff;

		//ShadowBoltTimer
        if (m_uiShadowBoltTimer <= uiDiff)
        {
			DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT);
            m_uiShadowBoltTimer = 8000;
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        // Heal
        if (m_uiHealTimer <= uiDiff)
        {
            if (Unit* pAlliedSoldier = DoSelectLowestHpFriendly(40.0f, 2000))
				DoCastSpellIfCan(pAlliedSoldier, SPELL_HEAL);

            m_uiHealTimer = 10000;
        }
        else
            m_uiHealTimer -= uiDiff;

        //No meele?
    }
};

CreatureAI* GetAI_boss_moira_bronzebeard(Creature* pCreature)
{
    return new boss_moira_bronzebeardAI(pCreature);
}

void AddSC_boss_draganthaurissan()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_emperor_dagran_thaurissan";
    pNewScript->GetAI = &GetAI_boss_emperor_dagran_thaurissan;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_moira_bronzebeard";
    pNewScript->GetAI = &GetAI_boss_moira_bronzebeard;
    pNewScript->RegisterSelf();
}
