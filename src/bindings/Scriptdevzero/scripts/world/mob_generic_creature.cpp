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
SDName: Generic_Creature
SD%Complete: 80
SDComment: Should be replaced with core based AI
SDCategory: Creatures
EndScriptData */

#include "precompiled.h"

#include <string>

#define GENERIC_CREATURE_COOLDOWN   5000

struct MANGOS_DLL_DECL generic_creatureAI : public ScriptedAI
{
    generic_creatureAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 GlobalCooldown;      //This variable acts like the global cooldown that players have (1.5 seconds)
    uint32 BuffTimer;           //This variable keeps track of buffs
    bool IsSelfRooted;

    void Reset()
    {
        GlobalCooldown = 0;
        BuffTimer = 0;          //Rebuff as soon as we can
        IsSelfRooted = false;
    }

    void Aggro(Unit *who)
    {
            if (!m_creature->IsWithinDistInMap(who, ATTACK_DISTANCE))
            {
                IsSelfRooted = true;
            }

			//Try to buff ourselves with some combat buff
			SpellEntry const *info = SelectSpell(m_creature, -1, -1, SELECT_TARGET_SELF, 0, 0, 0, 0, SELECT_EFFECT_AURA);
			if (info)
			{
                //Cast the buff spell
                DoCastSpell(m_creature, info);
			}
    }

    void UpdateAI(const uint32 diff)
    {
        //Always decrease our global cooldown first
        if (GlobalCooldown > diff)
            GlobalCooldown -= diff;
        else GlobalCooldown = 0;

        //Buff timer (only buff when we are alive and not in combat
        if (!m_creature->isInCombat() && m_creature->isAlive())
	{
            if (BuffTimer < diff)
            {
                //Find a spell that targets friendly and applies an aura (these are generally buffs)
                SpellEntry const *info = SelectSpell(m_creature, -1, -1, SELECT_TARGET_ANY_ALLY, 0, 0, 0, 0, SELECT_EFFECT_AURA);

                if (info && !GlobalCooldown)
                {
                    //Cast the buff spell
                    DoCastSpell(m_creature, info);

                    //Set our global cooldown
                    GlobalCooldown = GENERIC_CREATURE_COOLDOWN;

                    //Set our timer to 10 minutes before rebuff
                    BuffTimer = 600000;
                }//Try agian in 30 seconds
                else BuffTimer = 30000;
            }else BuffTimer -= diff;
	}

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Return if we already cast a spell
        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        //If we are within range melee the target
        if (m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
        {
            //Make sure our attack is ready
            if (m_creature->isAttackReady())
            {
                bool Healing = false;
                SpellEntry const *info = NULL;

                //Select a healing spell if less than 30% hp
                if (m_creature->GetHealthPercent() < 30.0f)
                    info = SelectSpell(m_creature, -1, -1, SELECT_TARGET_ANY_FRIEND, 0, 0, 0, 0, SELECT_EFFECT_HEALING);

                //No healing spell available, select a hostile spell
                if (info) Healing = true;
                else info = SelectSpell(m_creature->getVictim(), -1, -1, SELECT_TARGET_ANY_ENEMY, 0, 0, 0, 0, SELECT_EFFECT_DONTCARE);

                //50% chance if elite or higher, 20% chance if not, to replace our white hit with a spell
                if (info && (rand() % (m_creature->GetCreatureInfo()->rank > 1 ? 2 : 5) == 0) && !GlobalCooldown)
                {
                    //Cast the spell
                    if (Healing)DoCastSpellIfCan(m_creature, info->Id);
                    else DoCastSpellIfCan(m_creature->getVictim(), info->Id);

                    //Set our global cooldown
                    GlobalCooldown = GENERIC_CREATURE_COOLDOWN;
                }
                else m_creature->AttackerStateUpdate(m_creature->getVictim());

                m_creature->resetAttackTimer();
            }
        }
        else
        {
            bool Healing = false;
            SpellEntry const *info = NULL;

            //Select a healing spell if less than 30% hp ONLY 33% of the time
            if (m_creature->GetHealthPercent() < 30.0f && !urand(0, 2))
                info = SelectSpell(m_creature, -1, -1, SELECT_TARGET_ANY_FRIEND, 0, 0, 0, 0, SELECT_EFFECT_HEALING);

            //No healing spell available, See if we can cast a ranged spell (Range must be greater than ATTACK_DISTANCE)
            if (info) Healing = true;
            else info = SelectSpell(m_creature->getVictim(), -1, -1, SELECT_TARGET_ANY_ENEMY, 0, 0, ATTACK_DISTANCE, 0, SELECT_EFFECT_DONTCARE);

            //Found a spell, check if we arn't on cooldown
            if (info && !GlobalCooldown)
            {
                //If we are currently moving stop us and set the movement generator
                if (!IsSelfRooted)
                {
                    IsSelfRooted = true;
                }

                //Cast spell
                if (Healing) DoCastSpellIfCan(m_creature,info->Id);
                else DoCastSpellIfCan(m_creature->getVictim(),info->Id);

                //Set our global cooldown
                GlobalCooldown = GENERIC_CREATURE_COOLDOWN;


            }//If no spells available and we arn't moving run to target
            else if (IsSelfRooted)
            {
                //Cancel our current spell and then allow movement agian
                m_creature->InterruptNonMeleeSpells(false);
                IsSelfRooted = false;
            }
        }
    }
};

CreatureAI* GetAI_generic_creature(Creature* pCreature)
{
    return new generic_creatureAI(pCreature);
}

/*######
## mob_linked
######*/

struct MANGOS_DLL_DECL mob_linkedAI : public ScriptedAI
{
    mob_linkedAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Adds.clear();
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveIdle();
        m_bGetPos = true;
        Reset();
    }
    
    bool m_bGetPos;
    std::vector<ObjectGuid> Adds;
    
    void Reset() 
    {                       
    }
    
    void GetAddsPos()
    {
        for(uint8 i = 0; i < Adds.size(); ++i)
        {
            Creature* currentAdd = m_creature->GetMap()->GetCreature(Adds[i]);
            
            if (currentAdd)
            {
                if (currentAdd && currentAdd->IsInWorld() && currentAdd->isAlive())
                {
                    CreatureCreatePos pos(currentAdd->GetMap(), currentAdd->GetPositionX(), currentAdd->GetPositionY(), currentAdd->GetPositionZ(), currentAdd->GetOrientation());
                    currentAdd->SetSummonPoint(pos);              
                    m_bGetPos = false;
                }
            }
        }
    }
    
    void GetAdds()
    {
        std::vector<Creature*> members;
        m_creature->GetFormationMembers(members);
        
        for (Creature* member : members)
            Adds.push_back(member->GetObjectGuid());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_bGetPos)
        {
            GetAdds();
            GetAddsPos();
        }
        else
            return;
    }
};

CreatureAI* GetAI_mob_linked(Creature* pCreature)
{
    return new mob_linkedAI(pCreature);
}

struct MANGOS_DLL_DECL mob_schnakeAI : public ScriptedAI
{
	mob_schnakeAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
	}

	uint32 m_uiSchnakTimer;

	void Reset()
	{
		m_uiSchnakTimer = urand(300000, 600000);
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (m_uiSchnakTimer <= uiDiff)
		{
			std::string schnak("§"), insert("§");

			for (uint32 i = urand(1, 10); i > 0; i--)
				schnak.append(insert);
			schnak.append("!");

			m_creature->MonsterSay(schnak.c_str(), LANG_UNIVERSAL, nullptr);

			m_uiSchnakTimer = urand(300000, 600000);

		}
		else
			m_uiSchnakTimer -= uiDiff;
	}
};

CreatureAI* GetAI_mob_schnake(Creature* pCreature)
{
	return new mob_schnakeAI(pCreature);
}

enum resonating_crystal
{
    MOB_ERODED_ANUBISATH = 15810,
    MOB_MINOR_ANUBISATH = 15807,
    MOB_ANUBISATH = 15751,
    MOB_GREATER_ANUBISATH = 15754,
    MOB_SUPREME_ANUBISATH = 15758,
    MOB_COLOSSAL_ANUBISATH = 15743,

    CREATURE_OF_NIGHTMARE = 25806,
};

static uint32 m_AnubisathTypes[6] = {
    MOB_ERODED_ANUBISATH,
    MOB_MINOR_ANUBISATH,
    MOB_ANUBISATH,
    MOB_GREATER_ANUBISATH,
    MOB_SUPREME_ANUBISATH,
    MOB_COLOSSAL_ANUBISATH
};

struct MANGOS_DLL_DECL trigger_resonating_crystal : public ScriptedAI
{
	trigger_resonating_crystal(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
	}

    uint32 m_McTimer;

	void Reset()
	{
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->UpdateVisibilityAndView();
        m_McTimer = 30000;
	}

	void UpdateAI(const uint32 uiDiff)
	{
        if(m_McTimer)
        {
            if(m_McTimer <= uiDiff)
            {

                std::list<Creature*> l_Anubisath;
                
                for(int i = 0; i < 5; ++i)
                {
                    GetCreatureListWithEntryInGrid(l_Anubisath, m_creature, m_AnubisathTypes[i], 50.0f);

                    if(!l_Anubisath.empty())
                        break;
                }

                if(!l_Anubisath.empty())
                {
			        for (std::list<Creature*>::iterator iter = l_Anubisath.begin(); iter != l_Anubisath.end(); ++iter)
			        {
				        if (*iter)
				        {
					        Creature *pAnub = (*iter);

                            if(pAnub && !pAnub->isDead() && pAnub->isInCombat() && pAnub->getVictim())
                            {
                                Unit *pTarget = pAnub->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

                                if(pTarget)
                                {
                                    pAnub->AI()->DoCastSpellIfCan(pTarget, CREATURE_OF_NIGHTMARE, CastFlags::CAST_TRIGGERED);
                                    m_McTimer = 30000;
                                    
                                    return;
                                }
                            }
                        }
                    }
                }

                m_McTimer = 30000;
            }
            else
                m_McTimer -= uiDiff;
        }
	}
};

CreatureAI* GetAI_trigger_resonating_crystal(Creature* pCreature)
{
	return new trigger_resonating_crystal(pCreature);
}

void AddSC_generic_creature()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "generic_creature";
    pNewscript->GetAI = &GetAI_generic_creature;
    pNewscript->RegisterSelf(false);

    pNewscript = new Script;
    pNewscript->Name = "mob_linked";
    pNewscript->GetAI = &GetAI_mob_linked;
    pNewscript->RegisterSelf();
    
	pNewscript = new Script;
	pNewscript->Name = "mob_schnake";
	pNewscript->GetAI = &GetAI_mob_schnake;
	pNewscript->RegisterSelf();

	pNewscript = new Script;
	pNewscript->Name = "trigger_resonating_crystal";
	pNewscript->GetAI = &GetAI_trigger_resonating_crystal;
	pNewscript->RegisterSelf();
}
