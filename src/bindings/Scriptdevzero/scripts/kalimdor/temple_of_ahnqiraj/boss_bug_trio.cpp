/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: boss_kri, boss_yauj, boss_vem : The Bug Trio
SD%Complete: 100
SDComment:
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "temple_of_ahnqiraj.h"

#define SPELL_CLEAVE        26350
#define SPELL_TOXIC_VOLLEY  25812
#define SPELL_POISON_CLOUD  25786                           //Only Spell with right dmg.
#define SPELL_ENRAGE        28798                           //Changed cause 25790 is casted on gamers too. Same prob with old explosion of twin emperors.

#define SPELL_CHARGE        26561
#define SPELL_KNOCKBACK     26027

#define SPELL_HEAL      25807
#define SPELL_FEAR      19408

#define SPELL_STUN       25900

struct MANGOS_DLL_DECL boss_kriAI : public ScriptedAI
{
    boss_kriAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->RemoveAurasAtMechanicImmunity(MECHANIC_STUN, 25900);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 Cleave_Timer;
    uint32 ToxicVolley_Timer;
    uint32 Check_Timer;
    
    bool m_bStun;
    uint32 m_uiRemoveStunTimer;

    bool VemDead;
    bool Death;

    void Reset()
    {
        Cleave_Timer = urand(4000, 8000);
        ToxicVolley_Timer = urand(6000, 12000);
        Check_Timer = 2000;

        VemDead = false;
        Death = false;
        
        m_bStun = false;
    }

    void JustDied(Unit* /*killer*/)
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(DATA_BUG_TRIO_DEATH) < 2)
                                                            // Unlootable if death
                m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            m_pInstance->SetData(DATA_BUG_TRIO_DEATH, 1);
        }
        DoCast(m_creature->getVictim(), SPELL_POISON_CLOUD, true);
        
        CallEatDeadBoss();
    }

    void ResetToHome()
    {
        Creature* pVem = m_pInstance->GetSingleCreatureFromStorage(NPC_VEM);
        Creature* pYauj = m_pInstance->GetSingleCreatureFromStorage(NPC_YAUJ);

        if(pVem && pVem->isDead())
            pVem->Respawn();

        if(pYauj && pYauj->isDead())
            pYauj->Respawn();
        
        ScriptedAI::ResetToHome();
    }
    
    void CallEatDeadBoss()
    {
        Creature* pVem = m_pInstance->GetSingleCreatureFromStorage(NPC_VEM);
        Creature* pYauj = m_pInstance->GetSingleCreatureFromStorage(NPC_YAUJ);
        
        float x, y, z;
        
        if(pVem && !pVem->isDead())
        {
            m_creature->GetClosePoint(x, y, z, 10.f, 5.f);
            pVem->SetSpeedRate(MOVE_RUN, 6.f, false);
            pVem->GetMotionMaster()->MovePoint(1,x,y,z);
        }
        
         if(pYauj && !pYauj->isDead())
         {
             m_creature->GetClosePoint(x, y, z, 3.f, 2.f);
             pYauj->SetSpeedRate(MOVE_RUN, 6.f, false);
            pYauj->GetMotionMaster()->MovePoint(1,x,y,z);
         }
    }
    
    void MovementInform(uint32 uiMotionType, uint32 uiPointId)
    {
        Creature* pVem = m_pInstance->GetSingleCreatureFromStorage(NPC_VEM);
        Creature* pYauj = m_pInstance->GetSingleCreatureFromStorage(NPC_YAUJ);
        if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 3)    
        {            
            m_creature->SetFacingToObject(pVem);
            StopDueToStun();
        }
            if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 2)    
        {            
            m_creature->SetFacingToObject(pYauj);
            StopDueToStun();
        }
    }
    
    void StopDueToStun()
    {
        m_bStun = true;
        m_creature->CastSpell(m_creature, SPELL_STUN, true);

        m_uiRemoveStunTimer = 2000;
        m_creature->SetSpeedRate(MOVE_RUN, 2.9f, false);
    }
    
    void UpdateAI(const uint32 diff)
    {        
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bStun)
        {
            if (m_uiRemoveStunTimer <= diff)          
            {
                m_creature->RemoveAurasDueToSpell(25900);               
                m_bStun = false;
                
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
         
                SetCombatMovement(true);
            }
            else
                m_uiRemoveStunTimer -= diff; 
        }
        
        //Cleave_Timer
        if (Cleave_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_CLEAVE, true);
            Cleave_Timer = urand(5000, 12000);
        }else Cleave_Timer -= diff;

        //ToxicVolley_Timer
        if (ToxicVolley_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_TOXIC_VOLLEY, true);
            ToxicVolley_Timer = urand(10000, 15000);
        }else ToxicVolley_Timer -= diff;

        if (!VemDead)
        {
            //Checking if Vem is dead. If yes we will enrage.
            if (Check_Timer < diff)
            {
                if (m_pInstance && m_pInstance->GetData(TYPE_VEM) == DONE)
                {
                    DoCast(m_creature, SPELL_ENRAGE, true);
                    VemDead = true;
                }
                Check_Timer = 2000;
            }else Check_Timer -=diff;
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_vemAI : public ScriptedAI
{
    boss_vemAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->RemoveAurasAtMechanicImmunity(MECHANIC_STUN, 25900);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 Charge_Timer;
    uint32 KnockBack_Timer;
    uint32 Enrage_Timer;

    bool Enraged;
    
    bool m_bStun;
    uint32 m_uiRemoveStunTimer;

    void Reset()
    {
        Charge_Timer = urand(15000, 27000);
        KnockBack_Timer = urand(8000, 20000);
        Enrage_Timer = 120000;

        Enraged = false;
        
        m_bStun = false;
    }

    void ResetToHome()
    {
        Creature* pKri = m_pInstance->GetSingleCreatureFromStorage(NPC_KRI);
        Creature* pYauj = m_pInstance->GetSingleCreatureFromStorage(NPC_YAUJ);

        if(pKri && pKri->isDead())
            pKri->Respawn();

        if(pYauj && pYauj->isDead())
            pYauj->Respawn();

        ScriptedAI::ResetToHome();
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_VEM, DONE);

            // Unlootable if death
            if (m_pInstance->GetData(DATA_BUG_TRIO_DEATH) < 2)
                m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            m_pInstance->SetData(DATA_BUG_TRIO_DEATH, 1);
        }
        CallEatDeadBoss();
    }
       
    void CallEatDeadBoss()
    {
        Creature* pKri = m_pInstance->GetSingleCreatureFromStorage(NPC_KRI);
        Creature* pYauj = m_pInstance->GetSingleCreatureFromStorage(NPC_YAUJ);
        
        float x, y, z;
                         
        if(pKri && !pKri->isDead())
        {
            m_creature->GetClosePoint(x, y, z, 10.f, 5.f);
            pKri->SetSpeedRate(MOVE_RUN, 6.f, false);
            pKri->GetMotionMaster()->MovePoint(3,x,y,z);
        }
        
         if(pYauj && !pYauj->isDead())
         {
             m_creature->GetClosePoint(x, y, z, 3.f, 2.f);
             pYauj->SetSpeedRate(MOVE_RUN, 6.f, false);
            pYauj->GetMotionMaster()->MovePoint(3,x,y,z);
         }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId)
    {
        Creature* pKri = m_pInstance->GetSingleCreatureFromStorage(NPC_KRI);
        Creature* pYauj = m_pInstance->GetSingleCreatureFromStorage(NPC_YAUJ);
        if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 1)    
        {            
            m_creature->SetFacingToObject(pKri);
           StopDueToStun();
        }
         if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 2)    
        {            
            m_creature->SetFacingToObject(pYauj);
           StopDueToStun();
        }
    }
    
    void StopDueToStun()
    {
        m_bStun = true;
        m_creature->CastSpell(m_creature, SPELL_STUN, true);

        m_uiRemoveStunTimer = 3000;
        m_creature->SetSpeedRate(MOVE_RUN, 5.f, false);
    }    
    
    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
                      
        if (m_bStun)
        {
            if (m_uiRemoveStunTimer <= diff)          
            {
                m_creature->RemoveAurasDueToSpell(25900);               
                m_bStun = false;
                
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
         
                SetCombatMovement(true);
            }
            else
                m_uiRemoveStunTimer -= diff; 
        }
    
        //Charge_Timer
        if (Charge_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(target, SPELL_CHARGE, true);

            Charge_Timer = urand(8000, 16000);
        }else Charge_Timer -= diff;

        //KnockBack_Timer
        if (KnockBack_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_KNOCKBACK, true);
            if (m_creature->getThreatManager().getThreat(m_creature->getVictim()))
                m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(),-80);
            KnockBack_Timer = urand(15000, 25000);
        }else KnockBack_Timer -= diff;

        //Enrage_Timer
        if (!Enraged && Enrage_Timer < diff)
        {
            DoCast(m_creature, SPELL_ENRAGE, true);
            Enraged = true;
        }else Enrage_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_yaujAI : public ScriptedAI
{
    boss_yaujAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->RemoveAurasAtMechanicImmunity(MECHANIC_STUN, 25900);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 Heal_Timer;
    uint32 Fear_Timer;
    uint32 Check_Timer;

    bool m_bStun;
    uint32 m_uiRemoveStunTimer;
    
    bool VemDead;

    void Reset()
    {
        Heal_Timer = urand(25000, 40000);
        Fear_Timer = urand(12000, 24000);
        Check_Timer = 2000;

        VemDead = false;
        m_bStun = false;
    }

    void ResetToHome()
    {
        Creature* pKri = m_pInstance->GetSingleCreatureFromStorage(NPC_KRI);
        Creature* pVem = m_pInstance->GetSingleCreatureFromStorage(NPC_VEM);

        if(pKri && pKri->isDead())       
            pKri->Respawn();

        if(pVem && pVem->isDead())
            pVem->Respawn();

        ScriptedAI::ResetToHome();
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (m_pInstance)
        {
            if (m_pInstance->GetData(DATA_BUG_TRIO_DEATH) < 2)
                                                            // Unlootable if death
                m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            m_pInstance->SetData(DATA_BUG_TRIO_DEATH, 1);
        }

        for(int i = 0; i < 10; ++i)
        {
            Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            Creature* Summoned = m_creature->SummonCreature(15621,m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(),0,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,90000);
            if (Summoned && target)
                Summoned->AI()->AttackStart(target);
        }
        CallEatDeadBoss();
    }
    
    void CallEatDeadBoss()
    {
        Creature* pKri = m_pInstance->GetSingleCreatureFromStorage(NPC_KRI);
        Creature* pVem = m_pInstance->GetSingleCreatureFromStorage(NPC_VEM);
        
        float x, y, z;
                         
        if(pKri && !pKri->isDead())
        {
            m_creature->GetClosePoint(x, y, z, 10.f, 5.0f);
            pKri->SetSpeedRate(MOVE_RUN, 5.f, false);
            pKri->GetMotionMaster()->MovePoint(2,x,y,z);
        }
        
         if(pVem && !pVem->isDead())
         {
             m_creature->GetClosePoint(x, y, z, 3.f, 2.f);
             pVem->SetSpeedRate(MOVE_RUN, 5.f, false);
            pVem->GetMotionMaster()->MovePoint(2,x,y,z);
         }
    }
    
    void MovementInform(uint32 uiMotionType, uint32 uiPointId)
    {
        Creature* pKri = m_pInstance->GetSingleCreatureFromStorage(NPC_KRI);
        Creature* pVem = m_pInstance->GetSingleCreatureFromStorage(NPC_VEM);
        
        if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 1)    
        {            
            m_creature->SetFacingToObject(pKri);
           StopDueToStun();
        }
         if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 3)    
        {            
            m_creature->SetFacingToObject(pVem);
           StopDueToStun();
        }
    }
    
    void StopDueToStun()
    {
        m_bStun = true;
        m_creature->CastSpell(m_creature, SPELL_STUN, true);

        m_uiRemoveStunTimer = 2000;
        m_creature->SetSpeedRate(MOVE_RUN, 2.9f, false);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bStun)
        {
            if (m_uiRemoveStunTimer <= diff)          
            {
                m_creature->RemoveAurasDueToSpell(25900);               
                m_bStun = false;                
                
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
         
                SetCombatMovement(true);
            }
            else
                m_uiRemoveStunTimer -= diff; 
        }
        
        //Fear_Timer
        if (Fear_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_FEAR, true);
            DoResetThreat();
            Fear_Timer = 20000;
        }else Fear_Timer -= diff;

        //Casting Heal to other twins or herself.
        if (Heal_Timer < diff)
        {
            if (m_pInstance)
            {
                Creature* pKri = m_pInstance->GetSingleCreatureFromStorage(NPC_KRI);
                Creature* pVem = m_pInstance->GetSingleCreatureFromStorage(NPC_VEM);
                    
                // First rand for self or other boss, then rand which one of them, if none then heal self
                int rnd = urand(0,1);
                int rnd2 = urand(0,1);
                bool m_bFoundTarget = false;
                
                if(rnd == 0)
                {                         
                    if(rnd2 == 0)
                    {
                        if (pKri && pKri->isAlive() && !m_bFoundTarget)
                        {
                            DoCastSpellIfCan(pKri, SPELL_HEAL);
                            m_bFoundTarget = true;
                        }
                        else if (pVem && pVem->isAlive() && !m_bFoundTarget)
                        {
                            DoCastSpellIfCan(pVem, SPELL_HEAL);
                            m_bFoundTarget = true;
                        }                            
                    }
                    if(rnd2 == 1)   // same as above just the other way around
                    {
                        if (pVem && pVem->isAlive() && !m_bFoundTarget)
                        {
                            DoCastSpellIfCan(pVem, SPELL_HEAL);
                            m_bFoundTarget = true;
                        }
                        else if (pKri && pKri->isAlive() && !m_bFoundTarget)
                        {
                            DoCastSpellIfCan(pKri, SPELL_HEAL);
                            m_bFoundTarget = true;
                        }                            
                    }
                    if(!m_bFoundTarget)
                    {
                        DoCastSpellIfCan(m_creature, SPELL_HEAL);
                        m_bFoundTarget = true;
                    }
                }                                 
                else
                    DoCastSpellIfCan(m_creature, SPELL_HEAL);
            }
            Heal_Timer = urand(15000, 30000);
        }else Heal_Timer -= diff;

        //Checking if Vem is dead. If yes we will enrage.
        if (Check_Timer < diff)
        {
            if (!VemDead)
            {
                if (m_pInstance)
                {
                    if (m_pInstance->GetData(TYPE_VEM) == DONE)
                    {
                        DoCast(m_creature, SPELL_ENRAGE, true);
                        VemDead = true;
                    }
                }
            }
            Check_Timer = 2000;
        }else Check_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_yauj(Creature* pCreature)
{
    return new boss_yaujAI(pCreature);
}

CreatureAI* GetAI_boss_vem(Creature* pCreature)
{
    return new boss_vemAI(pCreature);
}

CreatureAI* GetAI_boss_kri(Creature* pCreature)
{
    return new boss_kriAI(pCreature);
}

void AddSC_bug_trio()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_kri";
    newscript->GetAI = &GetAI_boss_kri;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_vem";
    newscript->GetAI = &GetAI_boss_vem;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_yauj";
    newscript->GetAI = &GetAI_boss_yauj;
    newscript->RegisterSelf();
}
