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
SDName: Boss_Twinemperors
SD%Complete: 95
SDComment:
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "temple_of_ahnqiraj.h"
#include "WorldPacket.h"

#include "Item.h"
#include "Spell.h"

#define SPELL_HEAL_BROTHER          7393
#define SPELL_TWIN_TELEPORT         800                     // CTRA watches for this spell to start its teleport timer
#define SPELL_TWIN_TELEPORT_VISUAL  26638                   // visual
#define SPELL_ROOT_SELF             23973

#define SPELL_EXPLODEBUG            804
#define SPELL_MUTATE_BUG            802

#define SOUND_VN_DEATH              8660                    //8660 - Death - Feel
#define SOUND_VN_AGGRO              8661                    //8661 - Aggro - Let none
#define SOUND_VN_KILL               8662                    //8661 - Kill - your fate

#define SOUND_VL_AGGRO              8657                    //8657 - Aggro - To Late
#define SOUND_VL_KILL               8658                    //8658 - Kill - You will not
#define SOUND_VL_DEATH              8659                    //8659 - Death

#define PULL_RANGE                  50
#define ABUSE_BUG_RANGE             20
#define SPELL_BERSERK               26662
#define TELEPORTTIME                30000

#define SPELL_UPPERCUT              26007
#define SPELL_UNBALANCING_STRIKE    26613

#define VEKLOR_DIST                 20                      // VL will not come to melee when attacking

#define SPELL_SHADOWBOLT            26006
#define SPELL_BLIZZARD              26607
#define SPELL_ARCANEBURST           568

struct MANGOS_DLL_DECL boss_twinemperorsAI : public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    uint32 Heal_Timer;
    uint32 Teleport_Timer;
    bool AfterTeleport;
    uint32 AfterTeleportTimer;
    bool DontYellWhenDead;
    uint32 Abuse_Bug_Timer, BugsTimer;
    bool tspellcasted;
    uint32 EnrageTimer;

    virtual bool IAmVeklor() = 0;
    virtual void Reset() = 0;
    virtual void CastSpellOnBug(Creature *target) = 0;

    boss_twinemperorsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    void TwinReset()
    {
        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
        m_creature->SetSheath(SHEATH_STATE_UNARMED);
        Heal_Timer = 0;                                     // first heal immediately when they get close together
        Teleport_Timer = TELEPORTTIME;
        AfterTeleport = false;
        tspellcasted = false;
        AfterTeleportTimer = 0;
        Abuse_Bug_Timer = urand(10000, 17000);
        BugsTimer = 2000;
        m_creature->clearUnitState(UNIT_STAT_STUNNED);
        DontYellWhenDead = false;
        EnrageTimer = 15*60000;
    }

    Creature* GetOtherBoss()
    {
        if (m_pInstance)
        {
            return m_pInstance->GetSingleCreatureFromStorage(IAmVeklor() ? NPC_VEKNILASH : NPC_VEKLOR);
        }
        else
        {
            return NULL;
        }
    }

    void DamageTaken(Unit* /*done_by*/, uint32 &damage)
    {
        Creature *pOtherBoss = GetOtherBoss();
        if (pOtherBoss)
        {
            float dPercent = ((float)damage) / ((float)m_creature->GetMaxHealth());
            int odmg = (int)(dPercent * ((float)pOtherBoss->GetMaxHealth()));
            int ohealth = pOtherBoss->GetHealth()-odmg;
            pOtherBoss->SetHealth(ohealth > 0 ? ohealth : 0);
            if (ohealth <= 0)
            {
                pOtherBoss->SetDeathState(JUST_DIED);
                pOtherBoss->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            }
        }
    }

    void JustDied(Unit* Killer)
    {
        if (Creature* pOtherBoss = GetOtherBoss())
        {
            pOtherBoss->SetHealth(0);
            pOtherBoss->SetDeathState(JUST_DIED);
            pOtherBoss->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            pOtherBoss->SetLootRecipient(Killer);

            if (boss_twinemperorsAI* pOtherAI = dynamic_cast<boss_twinemperorsAI*>(pOtherBoss->AI()))
                pOtherAI->DontYellWhenDead = true;
        }

        if (!DontYellWhenDead)                              // I hope AI is not threaded
            DoPlaySoundToSet(m_creature, IAmVeklor() ? SOUND_VL_DEATH : SOUND_VN_DEATH);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_TWINS, DONE);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoPlaySoundToSet(m_creature, IAmVeklor() ? SOUND_VL_KILL : SOUND_VN_KILL);
    }

    void Aggro(Unit* pWho)
    {
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->SetSheath(SHEATH_STATE_MELEE);
        m_creature->SetInCombatWithZone();

        Creature *pOtherBoss = GetOtherBoss();
        if (pOtherBoss)
        {
            // TODO: we should activate the other boss location so he can start attackning even if nobody
            // is near I dont know how to do that
            if (!pOtherBoss->isInCombat())
            {
                DoPlaySoundToSet(m_creature, IAmVeklor() ? SOUND_VL_AGGRO : SOUND_VN_AGGRO);
                pOtherBoss->AI()->AttackStart(pWho);
            }

            pOtherBoss->SetInCombatWithZone();
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_TWINS, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_TWINS, FAIL);
    }

    void SpellHit(Unit *caster, const SpellEntry *entry)
    {
        if (caster == m_creature)
            return;

        Creature *pOtherBoss = GetOtherBoss();
        if (entry->Id != SPELL_HEAL_BROTHER || !pOtherBoss)
            return;

        // add health so we keep same percentage for both brothers
        uint32 mytotal = m_creature->GetMaxHealth(), histotal = pOtherBoss->GetMaxHealth();
        float mult = ((float)mytotal) / ((float)histotal);
        if (mult < 1)
            mult = 1.0f/mult;
        #define HEAL_BROTHER_AMOUNT 30000.0f
        uint32 largerAmount = (uint32)((HEAL_BROTHER_AMOUNT * mult) - HEAL_BROTHER_AMOUNT);

        if (mytotal > histotal)
        {
            uint32 h = m_creature->GetHealth()+largerAmount;
            m_creature->SetHealth(std::min(mytotal, h));
        }
        else
        {
            uint32 h = pOtherBoss->GetHealth()+largerAmount;
            pOtherBoss->SetHealth(std::min(histotal, h));
        }
    }

    void TryHealBrother(uint32 diff)
    {
        if (IAmVeklor())                                    // this spell heals caster and the other brother so let VN cast it
            return;

        if (Heal_Timer < diff)
        {
            Unit *pOtherBoss = GetOtherBoss();
            if (pOtherBoss && pOtherBoss->IsWithinDist(m_creature, 60.0f))
            {
                DoCastSpellIfCan(pOtherBoss, SPELL_HEAL_BROTHER);
                Heal_Timer = 1000;
            }
        } else Heal_Timer -= diff;
    }

    Unit *GetAnyoneCloseEnough(float dist, bool totallyRandom)
    {
        int cnt = 0;
        std::list<HostileReference*> candidates;

        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        for (ThreatList::const_iterator i = tList.begin();i != tList.end(); ++i)
        {
            Unit* pUnit = m_creature->GetMap()->GetUnit((*i)->getUnitGuid());

            if (pUnit && m_creature->GetCombatDistance(pUnit) < dist)
            {
                if (!totallyRandom)
                    return pUnit;
                candidates.push_back((*i));
                ++cnt;
            }
        }
        if (!cnt)
            return NULL;
        for (int randomi = rand() % cnt; randomi > 0; randomi --)
            candidates.pop_front();

        Unit *ret = m_creature->GetMap()->GetUnit(candidates.front()->getUnitGuid());
        candidates.clear();
        return ret;
    }

    Unit *PickNearestPlayer()
    {
        Unit *nearp = NULL;
        float neardist = 0.0f;
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        for (ThreatList::const_iterator i = tList.begin();i != tList.end(); ++i)
        {
            Unit* pUnit = m_creature->GetMap()->GetUnit((*i)->getUnitGuid());

            if (!pUnit)
                continue;

            float pudist = pUnit->GetDistance((const Creature *)m_creature);
            if (!nearp || (neardist > pudist))
            {
                nearp = pUnit;
                neardist = pudist;
            }
        }
        return nearp;
    }

    void TeleportToMyBrother()
    {
        if (!m_pInstance)
            return;

        Teleport_Timer = TELEPORTTIME;

        if (IAmVeklor())
            return;                                         // mechanics handled by veknilash so they teleport exactly at the same time and to correct coordinates

        Creature *pOtherBoss = GetOtherBoss();
        if (pOtherBoss)
        {
            //m_creature->MonsterYell("Teleporting ...", LANG_UNIVERSAL);
            float other_x = pOtherBoss->GetPositionX();
            float other_y = pOtherBoss->GetPositionY();
            float other_z = pOtherBoss->GetPositionZ();
            float other_o = pOtherBoss->GetOrientation();

            pOtherBoss->RelocateCreature(m_creature->GetPositionX(),
                m_creature->GetPositionY(),    m_creature->GetPositionZ(), m_creature->GetOrientation());
            pOtherBoss->GetMotionMaster()->MoveIdle();
            m_creature->RelocateCreature(other_x, other_y, other_z, other_o);
            m_creature->GetMotionMaster()->MoveIdle();

            SetAfterTeleport();

            if (boss_twinemperorsAI* pOtherAI = dynamic_cast<boss_twinemperorsAI*>(pOtherBoss->AI()))
                pOtherAI->SetAfterTeleport();
        }
    }

    void SetAfterTeleport()
    {
        m_creature->InterruptNonMeleeSpells(false);
        DoStopAttack();
        DoResetThreat();
        DoCastSpellIfCan(m_creature, SPELL_TWIN_TELEPORT_VISUAL);
        m_creature->addUnitState(UNIT_STAT_STUNNED);
        AfterTeleport = true;
        AfterTeleportTimer = urand(1500, 2000);
        tspellcasted = false;
    }

    bool TryActivateAfterTTelep(uint32 diff)
    {
        if (AfterTeleport)
        {
            if (!tspellcasted)
            {
                m_creature->clearUnitState(UNIT_STAT_STUNNED);
                DoCastSpellIfCan(m_creature, SPELL_TWIN_TELEPORT);
                m_creature->addUnitState(UNIT_STAT_STUNNED);
            }

            tspellcasted = true;

            if (AfterTeleportTimer < diff)
            {
                AfterTeleport = false;
                m_creature->clearUnitState(UNIT_STAT_STUNNED);
                Unit *nearu = PickNearestPlayer();
                //DoYell(nearu->GetName(), LANG_UNIVERSAL, 0);
                AttackStart(nearu);
                m_creature->getThreatManager().addThreat(nearu, 10000);
                m_creature->GetMotionMaster()->MoveChase(nearu);
                return true;
            }
            else
            {
                AfterTeleportTimer -= diff;
                // update important timers which would otherwise get skipped
                if (EnrageTimer > diff)
                    EnrageTimer -= diff;
                else
                    EnrageTimer = 0;
                if (Teleport_Timer > diff)
                    Teleport_Timer -= diff;
                else
                    Teleport_Timer = 0;
                return false;
            }
        }
        else
        {
            return true;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who || m_creature->getVictim())
            return;

        if (who->isTargetableForAttack() && who->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(who))
        {
            float attackRadius = m_creature->GetAttackDistance(who);
            if (attackRadius < PULL_RANGE)
                attackRadius = PULL_RANGE;
            if (m_creature->IsWithinDistInMap(who, attackRadius) && m_creature->GetDistanceZ(who) <= /*CREATURE_Z_ATTACK_RANGE*/7 /*there are stairs*/)
            {
                if (who->HasStealthAura())
                    who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                AttackStart(who);
            }
        }
    }

    Creature* RespawnNearbyBugsAndGetOne(uint32 bugEntry)
    {
        std::list<Creature*> lUnitList;
        GetCreatureListWithEntryInGrid(lUnitList, m_creature, bugEntry, 150.0f);

        if (lUnitList.empty())
            return NULL;


        float dist = 9999999999999.f;
        Creature* pClosest = nullptr;
        for(std::list<Creature*>::iterator iter = lUnitList.begin(); iter != lUnitList.end(); ++iter)
        {
            Creature *c = (Creature *)(*iter);

            if (c->HasAura(SPELL_EXPLODEBUG) || c->HasAura(SPELL_MUTATE_BUG))
                continue;

            if (c->isDead())
            {
                c->Respawn();
                c->setFaction(7);
                c->RemoveAllAuras();
            }
            if (c->GetDistance(m_creature) < dist)
            {
                pClosest = c;
                dist = c->GetDistance(m_creature);
            }
        }
        return pClosest;
    }

    void HandleBugs(uint32 diff)
    {
        if (BugsTimer < diff || Abuse_Bug_Timer < diff)
        {
            Creature *c = RespawnNearbyBugsAndGetOne(IAmVeklor() ? 15316 : 15317);
            if (Abuse_Bug_Timer < diff)
            {
                if (c)
                {
                    CastSpellOnBug(c);

                    if (m_creature->getVictim())
                    {
                        c->Attack(m_creature->getVictim(), false);
                        c->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    }

                    Abuse_Bug_Timer = urand(10000, 17000);
                }
                else
                {
                    Abuse_Bug_Timer = 1000;
                }
            }
            else
            {
                Abuse_Bug_Timer -= diff;
            }
            BugsTimer = 2000;
        }
        else
        {
            BugsTimer -= diff;
            Abuse_Bug_Timer -= diff;
        }
    }

    void CheckEnrage(uint32 diff)
    {
        if (EnrageTimer < diff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(true))
            {
                DoCastSpellIfCan(m_creature, SPELL_BERSERK);
                EnrageTimer = 60*60000;
            } else EnrageTimer = 0;
        } else EnrageTimer-=diff;
    }
};

struct MANGOS_DLL_DECL boss_veknilashAI : public boss_twinemperorsAI
{
    bool IAmVeklor() {return false;}
    boss_veknilashAI(Creature* pCreature) : boss_twinemperorsAI(pCreature)
    {
        Reset();
    }

    uint32 UpperCut_Timer;
    uint32 UnbalancingStrike_Timer;
    uint32 Scarabs_Timer;
    int Rand;
    int RandX;
    int RandY;

    Creature* Summoned;

    void Reset()
    {
        TwinReset();
        UpperCut_Timer = urand(14000, 29000);
        UnbalancingStrike_Timer = urand(8000, 18000);
        Scarabs_Timer = urand(7000, 14000);

        //Added. Can be removed if its included in DB.
        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
    }

    void CastSpellOnBug(Creature *target)
    {
        target->setFaction(14);

        DoCastSpellIfCan(target, SPELL_MUTATE_BUG, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!TryActivateAfterTTelep(diff))
            return;

        //UnbalancingStrike_Timer
        if (UnbalancingStrike_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_UNBALANCING_STRIKE);
            UnbalancingStrike_Timer = urand(8000, 20000);
        }else UnbalancingStrike_Timer -= diff;

        if (UpperCut_Timer < diff)
        {
            Unit* randomMelee = GetAnyoneCloseEnough(2*ATTACK_DISTANCE, true);
            if (randomMelee)
                DoCastSpellIfCan(randomMelee,SPELL_UPPERCUT);
            UpperCut_Timer = urand(15000, 30000);
        }else UpperCut_Timer -= diff;

        HandleBugs(diff);

        //Heal brother when 60yrds close
        TryHealBrother(diff);

        //Teleporting to brother
        if (Teleport_Timer < diff)
        {
            TeleportToMyBrother();
        }else Teleport_Timer -= diff;

        CheckEnrage(diff);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_veklorAI : public boss_twinemperorsAI
{
    bool IAmVeklor() {return true;}
    boss_veklorAI(Creature* pCreature) : boss_twinemperorsAI(pCreature)
    {
        Reset();
    }

    uint32 ShadowBolt_Timer;
    uint32 ShadowBolt_Cooldown;
    uint32 ShadowBolt_Counter;
    uint32 Blizzard_Timer;
    uint32 ArcaneBurst_Timer;
    uint32 Scorpions_Timer;
    int Rand;
    int RandX;
    int RandY;

    Creature* Summoned;

    void Reset()
    {
        TwinReset();
        ShadowBolt_Timer = 0;
        ShadowBolt_Cooldown = 0;
        ShadowBolt_Counter = urand(1, 3);
        Blizzard_Timer = urand(15000, 20000);
        ArcaneBurst_Timer = 1000;
        Scorpions_Timer = urand(7000, 14000);

        //Added. Can be removed if its included in DB.
        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);
    }

    void CastSpellOnBug(Creature *target)
    {
        target->setFaction(14);
        target->SetPassiveToSpells(true);
        DoCastSpellIfCan(target, SPELL_EXPLODEBUG, CAST_TRIGGERED);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // reset arcane burst after teleport - we need to do this because
        // when VL jumps to VN's location there will be a warrior who will get only 2s to run away
        // which is almost impossible
        if (AfterTeleport)
            ArcaneBurst_Timer = 5000;
        if (!TryActivateAfterTTelep(diff))
            return;

        if (ShadowBolt_Cooldown <= diff)
        {
            if (ShadowBolt_Counter > 0)
            {
                //ShadowBolt_Timer
                if (ShadowBolt_Timer < diff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(),SPELL_SHADOWBOLT) == CAST_OK)
                    {
                        ShadowBolt_Timer = 2000;
                        --ShadowBolt_Counter;
                    }
                }
                else 
                    ShadowBolt_Timer -= diff;

            }
            else
            {
                ShadowBolt_Counter = urand(1, 3);
                ShadowBolt_Cooldown = 5000;
            }
        }
        else
            ShadowBolt_Cooldown -= diff;

        //Blizzard_Timer
        if (Blizzard_Timer < diff)
        {
            Unit* target = NULL;
            target = GetAnyoneCloseEnough(45, true);
            if (target)
                DoCastSpellIfCan(target,SPELL_BLIZZARD);
            Blizzard_Timer = urand(15000, 30000);
        }
        else 
            Blizzard_Timer -= diff;

        if (ArcaneBurst_Timer < diff)
        {
            Unit *mvic;
            if ((mvic=GetAnyoneCloseEnough(2*ATTACK_DISTANCE, false))!=NULL)
            {
                DoCastSpellIfCan(mvic,SPELL_ARCANEBURST);
                ArcaneBurst_Timer = 5000;
            }
        }
        else 
            ArcaneBurst_Timer -= diff;

        HandleBugs(diff);

        //Heal brother when 60yrds close
        TryHealBrother(diff);

        //Teleporting to brother
        if (Teleport_Timer < diff)
        {
            TeleportToMyBrother();
        }
        else 
            Teleport_Timer -= diff;

        CheckEnrage(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_veknilash(Creature* pCreature)
{
    return new boss_veknilashAI(pCreature);
}

CreatureAI* GetAI_boss_veklor(Creature* pCreature)
{
    return new boss_veklorAI(pCreature);
}

struct MANGOS_DLL_DECL npc_the_masters_eye : public ScriptedAI
{
    npc_the_masters_eye(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiEventTimer;
    uint32 m_uiEventPhase;
    ObjectGuid m_oVeklor;
    ObjectGuid m_oVeknilash;
    bool m_bEventStarted;

    void Reset()
    {
        m_creature->SetHover(true);
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND/* | UNIT_BYTE1_FLAG_UNK_2*/);
        m_creature->SetSplineFlags(SPLINEFLAG_FLYING);
        m_uiEventTimer = 0;
        m_uiEventPhase = 0;
        m_bEventStarted = false;
    }

    void StartRpIntro()
    {
        if(!m_bEventStarted)
        {
            m_creature->GenericTextEmote("The massive floating eyeball in the center of the chamber turns its gaze upon you. You stand before a god.",
                nullptr, true);

            m_uiEventTimer = 2000;
        
            Creature *pVeklor = m_creature->GetClosestCreatureWithEntry(m_creature, 15276, 100.0f);
            Creature *pVeknilash = m_creature->GetClosestCreatureWithEntry(m_creature, 15275, 100.0f);

            if(pVeklor)
                m_oVeklor = pVeklor->GetObjectGuid();

            if(pVeknilash)
                m_oVeknilash = pVeknilash->GetObjectGuid();

            m_bEventStarted = true;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(m_uiEventTimer)
        {
            if(m_uiEventTimer <= diff)
            {
                Creature *pVeklor = m_creature->GetMap()->GetCreature(m_oVeklor);
                Creature *pVeknilash = m_creature->GetMap()->GetCreature(m_oVeknilash);

                switch(m_uiEventPhase)
                {
                case 0:
                    m_creature->SetFacingTo(1.77f);
                    m_uiEventTimer = 8000;
                    break;
                case 1:
                    m_creature->SetVisibility(VISIBILITY_OFF);
                    m_creature->UpdateVisibilityAndView();
                    if(pVeklor && pVeknilash)
                    {
                        pVeklor->SetStandState(UNIT_STAND_STATE_STAND);
                        pVeknilash->SetStandState(UNIT_STAND_STATE_STAND);
                    }
                    m_uiEventTimer = 4000;
                    break;
                case 2:
                    if(pVeklor)
                    {
                        pVeklor->MonsterYell("Only flesh and bone. Mortals are such easy prey...", LANG_UNIVERSAL);
                        pVeklor->HandleEmote(EMOTE_ONESHOT_TALK);
                    }
                    m_uiEventTimer = 5000;
                    break;
                case 3:
                    if(pVeknilash)
                    {
                        pVeknilash->MonsterYell("Where are your manners, brother. Let us properly welcome our guests.", LANG_UNIVERSAL);
                        pVeknilash->HandleEmote(EMOTE_ONESHOT_TALK);
                    }
                    m_uiEventTimer = 5000;
                    break;
                case 4:
                    if(pVeklor)
                    {
                        pVeklor->MonsterYell("There will be pain...", LANG_UNIVERSAL);
                        pVeklor->HandleEmote(EMOTE_ONESHOT_TALK);
                    }
                    m_uiEventTimer = 3000;
                    break;
                case 5:
                    if(pVeknilash) //skrik
                    {
                        pVeknilash->MonsterYell("Oh so much pain...", LANG_UNIVERSAL);
                        pVeknilash->HandleEmote(EMOTE_ONESHOT_SHOUT);
                    }
                    m_uiEventTimer = 2000;
                    break;
                case 6:
                    if(pVeklor && pVeknilash)
                    {
                        pVeklor->HandleEmote(EMOTE_ONESHOT_ROAR);
                        pVeknilash->HandleEmote(EMOTE_ONESHOT_ROAR);
                    }
                    m_uiEventTimer = 1500;
                    break;
                case 7:
                    if(pVeklor && pVeknilash)
                    {
                        pVeklor->SetFacingTo(2.21f);
                        pVeknilash->SetFacingTo(1.20f);
                    }
                    m_uiEventTimer = 1000;
                    break;
                case 8:
                    if(pVeklor)
                    {
                        pVeklor->MonsterYell("Come, little ones.", LANG_UNIVERSAL);
                        pVeklor->HandleEmote(EMOTE_ONESHOT_TALK);
                    }
                    m_uiEventTimer = 2000;
                    break;
                case 9:
                    if(pVeknilash)
                    {
                        pVeknilash->MonsterYell("The feast of souls begin now...", LANG_UNIVERSAL);
                        pVeknilash->HandleEmote(EMOTE_ONESHOT_TALK);
                    }
                    m_uiEventTimer = 2000;
                    break;
                case 10:
                    if(pVeklor && pVeknilash)
                    {
                        pVeklor->HandleEmote(EMOTE_ONESHOT_POINT);
                        pVeknilash->HandleEmote(EMOTE_ONESHOT_POINT);
                    }
                    break;
                    m_uiEventTimer = 0;
                default:
                    m_uiEventTimer = 0;
                    m_uiEventPhase = 0;
                    break;
                }
                ++m_uiEventPhase;
            }
            else
                m_uiEventTimer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_the_masters_eye(Creature* pCreature)
{
    return new npc_the_masters_eye(pCreature);
}

bool AreaTrigger_at_twin_emperor_room(Player* pPlayer, AreaTriggerEntry const* /*pAt*/)
{
    if(pPlayer)
    {
        Creature *pMasterEye = GetClosestCreatureWithEntry(pPlayer, 15963, 100.0f);

        if(pMasterEye)
        {
            npc_the_masters_eye *pMasterEyeAI = dynamic_cast<npc_the_masters_eye*>(pMasterEye->AI());

            if(pMasterEyeAI)
            {
                pMasterEyeAI->StartRpIntro();
            }
        }
    }
	return false;
}

void AddSC_boss_twinemperors()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_veknilash";
    newscript->GetAI = &GetAI_boss_veknilash;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_veklor";
    newscript->GetAI = &GetAI_boss_veklor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_the_masters_eye";
    newscript->GetAI = &GetAI_npc_the_masters_eye;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "at_twin_emperor_room";
    newscript->pAreaTrigger = &AreaTrigger_at_twin_emperor_room;
    newscript->RegisterSelf();
}
