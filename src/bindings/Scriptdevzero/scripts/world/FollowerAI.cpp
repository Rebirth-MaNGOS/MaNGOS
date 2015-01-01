/* Copyright (C) 2009 - 2010 ScriptDevZero <http://github.com/scriptdevzero/scriptdevzero> 
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

#include "precompiled.h"
#include "NightmareDragons.h"

//#define SPELL_POISON_CLOUD 23861
//
//struct MANGOS_DLL_DECL mob_dreamcloudAI : public ScriptedAI
//{
//
// mob_dreamcloudAI(Creature* pCreature) : ScriptedAI(pCreature) 
// {
//	 m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
//     m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
//     m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
//	 m_creature->setFaction(14);
//	 Reset();
// }
//
// Unit* Target;
// uint32 TauntTimer;
// uint32 SwitchTimer;
// uint32 SleepTimer;
// uint32 VisualEffectTimer;
//
// void Reset()
// {
//  Target = 0;
//  TauntTimer = 500;
//  SwitchTimer = urand(4000,5000);
//  SleepTimer = 3000;
//  VisualEffectTimer = 40000;
//  m_creature->CastSpell(m_creature,DREAM_FOG_VISUAL,true);
// }
//
// void SwitchTarget()
// {
//   Unit* u = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
//   if (u && u->isAlive()) {
//    Target = u;
//    SwitchTimer = urand(4000,5000);
//   }
// }
//
// void UpdateAI(const uint32 diff)
// {
//  if (!Target || !Target->isAlive())
//   SwitchTarget();
//
//  if (TauntTimer < diff)
//  {
//   TauntTimer = 500;
//   if (Target && Target->isAlive())
//   {
//    m_creature->TauntApply(Target);
//	m_creature->getThreatManager().addThreat(Target, 500);
//	m_creature->getThreatManager().modifyThreatPercent(Target, 100);
//   }
//  } else TauntTimer -= diff;
//
//  if (SleepTimer < diff)
//  {
//   SleepTimer = 1500;
//   DoCastSpellIfCan(m_creature, SPELL_SLEEP_CLOUD);
//   //If reached target: Switch to another one
//   if (Target && m_creature->GetDistance(Target) < 5.0f)
//    SwitchTarget();
//  } else SleepTimer -= diff;
//
//  if (VisualEffectTimer < diff)
//  {
//   VisualEffectTimer = 40000;
//   m_creature->CastSpell(m_creature,DREAM_FOG_VISUAL,true);
//  } else VisualEffectTimer -= diff;
//   
//  if (SwitchTimer < diff) 
//  {
//   SwitchTarget();
//  } else SwitchTimer -= diff;
//
// }
//
//};
//
////AI for the trigger that does damage to players close to putrid mushrooms
//
//struct MANGOS_DLL_DECL mob_putrid_mushroomAI : public ScriptedAI
//{
//
// mob_putrid_mushroomAI(Creature* pCreature) : ScriptedAI(pCreature) 
// {
//     m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
//     m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
//     m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
//     m_creature->setFaction(14);
//     m_creature->SetVisibility(VISIBILITY_OFF);
//	 Reset();
// }
//
// uint32 PoisonCloudTimer;
//
// void Reset()
// {
//  PoisonCloudTimer = 500;
// }
//
// void Aggro(Unit* u)
// {
//  SetCombatMovement(false); 
//  if (m_creature->GetMotionMaster())
//  {
//   m_creature->GetMotionMaster()->Clear(false);
//   m_creature->GetMotionMaster()->MoveIdle();
//  }
// }
//
// void UpdateAI(const uint32 diff)
// {
//  if (PoisonCloudTimer < diff)
//  {
//   m_creature->CastSpell(m_creature,SPELL_POISON_CLOUD,true);
//   PoisonCloudTimer = 9500;
//  }
//  else PoisonCloudTimer -= diff;
// }
//
//};
//
//CreatureAI* GetAI_mob_dreamcloud(Creature* pCreature)
//{
//    return new mob_dreamcloudAI(pCreature);
//}
//
//CreatureAI* GetAI_mob_putrid_mushroom(Creature* pCreature)
//{
//    return new mob_putrid_mushroomAI(pCreature);
//}
//
//void AddSC_dragon_adds()
//{
//    Script *newscript;
//    newscript = new Script;
//    newscript->Name = "mob_dreamcloud";
//    newscript->GetAI = &GetAI_mob_dreamcloud;
//    newscript->RegisterSelf();
//
//    newscript = new Script;
//    newscript->Name = "mob_putrid_mushroom";
//    newscript->GetAI = &GetAI_mob_putrid_mushroom;
//    newscript->RegisterSelf();
//}