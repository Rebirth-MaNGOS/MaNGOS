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
SDName: Boss_Moam
SD%Complete: 100
SDComment:
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "ruins_of_ahnqiraj.h"

enum eMoam
{
    //EMOTE_AGGRO              = -1509000,
    //EMOTE_MANA_FULL          = -1509001,
    //EMOTE_ENERGIZING         = -1509028,

    SPELL_TRAMPLE            = 15550,
    SPELL_DRAIN_MANA         = 25671,
	SPELL_DRAIN_MANA_VISUAL  = 26639,
    SPELL_ARCANE_ERUPTION    = 25672,
    SPELL_SUMMON_MANAFIEND_1 = 25681,
    SPELL_SUMMON_MANAFIEND_2 = 25682,
    SPELL_SUMMON_MANAFIEND_3 = 25683,
    SPELL_ENERGIZE           = 25685,

	SPELL_IVUS_TELEPORT_VISUAL	= 16807, //21649,		// visual for mana fiend spawn, new spell simple teleport has better animation

    PHASE_ATTACKING          = 0,
    PHASE_ENERGIZING         = 1
};

struct MANGOS_DLL_DECL boss_moamAI : public ScriptedAI
{
    boss_moamAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ruins_of_ahnqiraj* m_pInstance;

    uint8 m_uiPhase;
	uint8 m_uiDrain_count;
    
    uint32 m_uiTrampleTimer;
    uint32 m_uiManaDrainTimer;
    uint32 m_uiCheckoutManaTimer;
    uint32 m_uiSummonManaFiendsTimer;
	uint32 m_uiRemoveEnergizeTimer;

    void Reset()
    {
		m_uiDrain_count = 0;
        m_uiTrampleTimer = 9000;
        m_uiManaDrainTimer = 3000;
        m_uiSummonManaFiendsTimer = 90000;
        m_uiCheckoutManaTimer = 15000;
		m_uiRemoveEnergizeTimer = 0;
        m_uiPhase = PHASE_ATTACKING;
        m_creature->SetPower(POWER_MANA, 0);
        m_creature->SetMaxPower(POWER_MANA, 0);
    }

    void Aggro(Unit* /*pWho*/)
    {
		m_creature->GenericTextEmote("Moam senses your fear.", NULL, false);
        //DoScriptText(EMOTE_AGGRO, m_creature);			// until DB emotes work this is commented out
        m_creature->SetMaxPower(POWER_MANA, m_creature->GetCreatureInfo()->maxmana);
    }
    
    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MOAM, DONE);
    }

	void JustSummoned(Creature* pSummoned)
    {
		if (pSummoned->GetEntry() == NPC_MANA_FIEND)
			pSummoned->CastSpell(pSummoned,SPELL_IVUS_TELEPORT_VISUAL,true);	
		pSummoned->SetRespawnEnabled(false);
	}

	void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_DRAIN_MANA)
			pTarget->CastSpell(m_creature,SPELL_DRAIN_MANA_VISUAL, true);				// animation for mana drain
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiCheckoutManaTimer <= uiDiff)
        {
			m_uiCheckoutManaTimer = 2000;
			if (m_creature->GetPower(POWER_MANA) == m_creature->GetMaxPower(POWER_MANA))
			{
				if(m_creature->HasAura(SPELL_ENERGIZE))
					m_creature->RemoveAurasDueToSpell(SPELL_ENERGIZE);

				DoCastSpellIfCan(m_creature, SPELL_ARCANE_ERUPTION);
				m_creature->GenericTextEmote("Moam bristles with energy!", NULL, false);
				//DoScriptText(EMOTE_MANA_FULL, m_creature);
				m_uiPhase = PHASE_ATTACKING;
                return;
			}
		} 
		else
			m_uiCheckoutManaTimer -= uiDiff;

        switch(m_uiPhase)
        {
            case PHASE_ATTACKING:

                if (m_uiSummonManaFiendsTimer <= uiDiff)		// do the stone transform here
                {
					float fX, fY, fZ;
					m_creature->GetPosition(fX, fY, fZ);
					for(uint8 i = 0; i < 3; ++i)
						if (Creature* pFiend = m_creature->SummonCreature(NPC_MANA_FIEND, fX+irand(-5,5), fY+irand(-5,5), fZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000))
							if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))							
								pFiend->AI()->AttackStart(pTarget);

					DoCastSpellIfCan(m_creature, SPELL_ENERGIZE);
					m_creature->GenericTextEmote("Moam drains your mana and turns to stone.", NULL, false);
                    m_uiSummonManaFiendsTimer = 90000;
					m_uiRemoveEnergizeTimer = 90000;
					m_uiPhase = PHASE_ENERGIZING;
                    return;
                }
                else
                    m_uiSummonManaFiendsTimer -= uiDiff;

                if (m_uiManaDrainTimer <= uiDiff)
                {
					const ThreatList& threatList = m_creature->getThreatManager().getThreatList();

					if(!threatList.empty())
					{
						for (HostileReference *currentReference : threatList)
						{
							Unit *target = currentReference->getTarget();
							if (target && target->GetTypeId() == TYPEID_PLAYER && target->getPowerType() == POWER_MANA && target->GetDistance(m_creature) < 40.0f)
							{	
								uint32 rndm_counter = urand(2,6);
								m_creature->CastSpell(target, SPELL_DRAIN_MANA, true);
								++m_uiDrain_count;
								if(rndm_counter == m_uiDrain_count)
									break;
							}							
						}
					}
					
                    m_uiManaDrainTimer = urand(2000, 6000);
                } 
                else
                    m_uiManaDrainTimer -= uiDiff;

                if (m_uiTrampleTimer <= uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_TRAMPLE);
                    m_uiTrampleTimer = 15000;
                } 
                else
                    m_uiTrampleTimer -= uiDiff;

                DoMeleeAttackIfReady();
                break;
            case PHASE_ENERGIZING:
                // after 90 sec he should go back to attacking if he still didn't reach 100% mana
				if (m_uiRemoveEnergizeTimer <= uiDiff)
                {
                    if(m_creature->HasAura(SPELL_ENERGIZE))
						m_creature->RemoveAurasDueToSpell(SPELL_ENERGIZE);
					m_uiSummonManaFiendsTimer = 90000;
                    m_uiRemoveEnergizeTimer = 0;
					m_uiPhase = PHASE_ATTACKING;
                    return;
                } 
                else
                    m_uiRemoveEnergizeTimer -= uiDiff;
                break;
        }
    }
};

CreatureAI* GetAI_boss_moam(Creature* pCreature)
{
    return new boss_moamAI(pCreature);
}

void AddSC_boss_moam()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_moam";
    pNewScript->GetAI = &GetAI_boss_moam;
    pNewScript->RegisterSelf();
}
