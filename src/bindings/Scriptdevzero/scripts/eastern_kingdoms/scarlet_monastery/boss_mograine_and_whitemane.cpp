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
SDName: Boss_Mograine_And_Whitemane
SD%Complete: 90
SDComment:
SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"
#include "scarlet_monastery.h"

enum
{
    // Mograine says
    SAY_MO_AGGRO                 = -1189005,
    SAY_MO_KILL                  = -1189006,
    SAY_MO_RESSURECTED           = -1189007,

    // Whitemane says
    SAY_WH_INTRO                 = -1189008,
    SAY_WH_KILL                  = -1189009,
    SAY_WH_RESSURECT             = -1189010,

	// Ashbringer event
	// Renault says
	SAY_ASH_RENAULT1             = -1189022,    // welcome (male)
	SAY_ASH_RENAULT2             = -1189023,    // welcome (female)
	SAY_ASH_RENAULT3             = -1189024,    // „Father..But..How?“
	SAY_ASH_RENAULT4             = -1189025,    // „Forgive Me, Father! Please..“

	// Spirit of Mograine says
	SAY_ASH_HIGHLORD1            = -1189026,    // „Renault...“
	SAY_ASH_HIGHLORD2            = -1189027,    // „..I AM ASHBRINGER“
	SAY_ASH_HIGHLORD3            = -1189028,    // „You are forgiven..“

    // Mograine Spells
    SPELL_CRUSADERSTRIKE         = 14518,
    SPELL_HAMMEROFJUSTICE        = 5589,
    SPELL_LAYONHANDS             = 9257,
    SPELL_RETRIBUTIONAURA        = 8990,
	SPELL_DIVINE_SHIELD			= 642,		

    // Whitemanes Spells
    SPELL_DEEP_SLEEP              = 9256,
    SPELL_SCARLETRESURRECTION    = 9232,
    SPELL_DOMINATEMIND           = 14515,
    SPELL_HOLYSMITE              = 9481,
    SPELL_HEAL                   = 12039,
    SPELL_POWERWORDSHIELD        = 22187,
};

struct MANGOS_DLL_DECL boss_scarlet_commander_mograineAI : public ScriptedAI
{
    boss_scarlet_commander_mograineAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scarlet_monastery*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scarlet_monastery* m_pInstance;

    uint32 m_uiCrusaderStrike_Timer;
    uint32 m_uiHammerOfJustice_Timer;

	bool m_bHasBubbled;
    bool m_bHasDied;
    bool m_bHeal;
    bool m_bFakeDeath;

    void Reset()
    {
        if (m_pInstance)
        {
		    Creature* pWhitemane = m_pInstance->GetSingleCreatureFromStorage(NPC_WHITEMANE);
            if (pWhitemane && !pWhitemane->isAlive())
                pWhitemane->Respawn();
        }

        m_uiCrusaderStrike_Timer = 10000;
        m_uiHammerOfJustice_Timer = 10000;

        // Incase wipe during phase that mograine fake death
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

		m_bHasBubbled = false;
        m_bHasDied = false;
        m_bHeal = false;
        m_bFakeDeath = false;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_MO_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_RETRIBUTIONAURA);
        m_creature->CallForHelp(80.f);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(SAY_MO_KILL, m_creature);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE | UNIT_DYNFLAG_TAPPED);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
		if (uiDamage < m_creature->GetHealth() || m_bHasDied)
            return;

        if (m_bFakeDeath)
        {
			m_creature->SetHealth(uiDamage * 3);
            uiDamage = 0;
            return;
        }

        if (!m_pInstance)
            return;

        // On first death, fake death and open door, as well as initiate whitemane if exist
        if (Creature* pWhitemane = m_pInstance->GetSingleCreatureFromStorage(NPC_WHITEMANE))
        {
            m_pInstance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, IN_PROGRESS);

            m_creature->GetMotionMaster()->MovementExpired();
            m_creature->GetMotionMaster()->MoveIdle();

            m_creature->ClearComboPointHolders();
            m_creature->RemoveAllAuras();
            m_creature->ClearAllReactives();

            if (m_creature->IsNonMeleeSpellCasted(false))
                m_creature->InterruptNonMeleeSpells(false);

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
            m_creature->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);

            pWhitemane->GetMotionMaster()->MovePoint(1, 1163.113370f, 1398.856812f, 32.527786f);
            pWhitemane->AI()->AttackStart(pDoneBy);

            m_bHasDied = true;
            m_bFakeDeath = true;

			m_creature->SetHealth(uiDamage * 3);
            uiDamage = 0;
        }
    }

    void SpellHit(Unit* /*pWho*/, const SpellEntry* pSpell)
    {
        // When hit with ressurection say text
        if (pSpell->Id == SPELL_SCARLETRESURRECTION)
        {
            DoScriptText(SAY_MO_RESSURECTED, m_creature);
            m_bFakeDeath = false;

            if (m_pInstance)
                m_pInstance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, SPECIAL);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (!m_bHasDied && !m_bHeal && !m_bHasBubbled && HealthBelowPct(25))			// Only bubble once and do it before first death
		{
			DoCastSpellIfCan(m_creature, SPELL_DIVINE_SHIELD);
			m_bHasBubbled = true;
		}	

        if (m_bHasDied && !m_bHeal && m_pInstance && m_pInstance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == SPECIAL)
        {
            // On ressurection, stop fake death and heal whitemane and resume fight
            if (Creature* pWhitemane = m_pInstance->GetSingleCreatureFromStorage(NPC_WHITEMANE))
            {
                m_creature->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                DoCastSpellIfCan(pWhitemane, SPELL_LAYONHANDS);

                m_uiCrusaderStrike_Timer = 10000;
                m_uiHammerOfJustice_Timer = 10000;

                if (m_creature->getVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

                m_bHeal = true;
            }
        }

        // This if-check to make sure mograine does not attack while fake death
        if (m_bFakeDeath)
            return;

        // Crusader Strike
        if (m_uiCrusaderStrike_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CRUSADERSTRIKE);
            m_uiCrusaderStrike_Timer = 10000;
        }
        else 
            m_uiCrusaderStrike_Timer -= uiDiff;

        // Hammer of Justice
        if (m_uiHammerOfJustice_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAMMEROFJUSTICE);
            m_uiHammerOfJustice_Timer = 60000;
        }
        else 
            m_uiHammerOfJustice_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_high_inquisitor_whitemaneAI : public ScriptedAI
{
    boss_high_inquisitor_whitemaneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scarlet_monastery*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scarlet_monastery* m_pInstance;

    uint32 m_uiHeal_Timer;
    uint32 m_uiPowerWordShield_Timer;
    uint32 m_uiHolySmite_Timer;
    uint32 m_uiWait_Timer;

    bool m_bCanResurrectCheck;
    bool m_bCanResurrect;

    void Reset()
    {
		if (m_pInstance)
        {
		    Creature* pMograine = m_pInstance->GetSingleCreatureFromStorage(NPC_MOGRAINE);
            if (pMograine && !pMograine->isAlive())
                pMograine->Respawn();
        }

        m_uiWait_Timer = 7000;
        m_uiHeal_Timer = 10000;
        m_uiPowerWordShield_Timer = 15000;
        m_uiHolySmite_Timer = 6000;

        m_bCanResurrectCheck = false;
        m_bCanResurrect = false;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            if (!(m_pInstance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == NOT_STARTED) || !(m_pInstance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == FAIL))
                m_pInstance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, FAIL);
        }
    }

    void MoveInLineOfSight(Unit*)
    {
        // This needs to be empty because Whitemane should NOT aggro while fighting Mograine. Mograine will give us a target.
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        if (!m_bCanResurrectCheck || m_bCanResurrect)
        {
            // prevent killing blow before rezzing commander
            //m_creature->SetHealth(uiDamage+1);
            uiDamage = 0;
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (m_pInstance && (m_pInstance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == NOT_STARTED || m_pInstance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == FAIL))
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_WH_INTRO, m_creature);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(SAY_WH_KILL, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bCanResurrect)
        {
            // When casting resuruction make sure to delay so on rez when reinstate battle deepsleep runs out
            if (m_pInstance && m_uiWait_Timer <= uiDiff)
            {
                if (Creature* pMograine = m_pInstance->GetSingleCreatureFromStorage(NPC_MOGRAINE))
                {
                    DoCastSpellIfCan(pMograine, SPELL_SCARLETRESURRECTION);
                    DoScriptText(SAY_WH_RESSURECT, m_creature);
                    m_bCanResurrect = false;
                }
            }
            else 
                m_uiWait_Timer -= uiDiff;
        }

        // Cast Deep sleep when health is less than 50%
        if (!m_bCanResurrectCheck && HealthBelowPct(50))
        {
            if (m_creature->IsNonMeleeSpellCasted(false))
                m_creature->InterruptNonMeleeSpells(false);

            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEEP_SLEEP);
            m_bCanResurrectCheck = true;
            m_bCanResurrect = true;
            return;
        }

        // while in "resurrect-mode", don't do anything
        if (m_bCanResurrect)
            return;

        // If we are <75% hp cast healing spells at self or Mograine
        if (m_uiHeal_Timer <= uiDiff)
        {
            Creature* pTarget = NULL;

            if (HealthBelowPct(75))
                pTarget = m_creature;

            if (m_pInstance)
            {
                if (Creature* pMograine = m_pInstance->GetSingleCreatureFromStorage(NPC_MOGRAINE))
                {
                    if (pMograine->isAlive() && pMograine->GetHealth() <= pMograine->GetMaxHealth()*0.75f)
                        pTarget = pMograine;
                }
            }

            if (pTarget)
                DoCastSpellIfCan(pTarget, SPELL_HEAL);

            m_uiHeal_Timer = 13000;
        }
        else 
            m_uiHeal_Timer -= uiDiff;

        // Power word: Shield
        if (m_uiPowerWordShield_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_POWERWORDSHIELD);
            m_uiPowerWordShield_Timer = 15000;
        }
        else
            m_uiPowerWordShield_Timer -= uiDiff;

        // Holy Smite
        if (m_uiHolySmite_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_HOLYSMITE);
            m_uiHolySmite_Timer = 6000;
        }
        else
            m_uiHolySmite_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_scarlet_commander_mograine(Creature* pCreature)
{
    return new boss_scarlet_commander_mograineAI(pCreature);
}

CreatureAI* GetAI_boss_high_inquisitor_whitemane(Creature* pCreature)
{
    return new boss_high_inquisitor_whitemaneAI(pCreature);
}

void AddSC_boss_mograine_and_whitemane()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_scarlet_commander_mograine";
    pNewScript->GetAI = &GetAI_boss_scarlet_commander_mograine;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_high_inquisitor_whitemane";
    pNewScript->GetAI = &GetAI_boss_high_inquisitor_whitemane;
    pNewScript->RegisterSelf();
}
