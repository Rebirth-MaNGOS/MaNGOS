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
SDName: Boss_Sapphiron
SD%Complete: 0
SDComment: Ice block not protecting from breath, fly + cast is bugged visual, blizzard is wrong, 
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

/*enum
{
    EMOTE_BREATH       = -1533082,
    EMOTE_ENRAGE       = -1533083,

    SPELL_CLEAVE  = 19983,
    SPELL_ICEBOLT      = 28522,
    SPELL_FROST_BREATH_DUMMY    = 30101,
    SPELL_FROST_BREATH          = 28524,            // triggers 29318
    SPELL_FROST_BREATH_2 = 29318,
    SPELL_FROST_AURA   = 28531,
    SPELL_LIFE_DRAIN   = 28542,
    SPELL_BLIZZARD     = 28547,
    SPELL_BESERK       = 26662
};

struct MANGOS_DLL_DECL boss_sapphironAI : public ScriptedAI
{
    boss_sapphironAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_FROST, true);
        Reset();
    }

    instance_naxxramas* m_pInstance;

    uint32 m_uiCleaveTimer;
    uint32 Icebolt_Count;
    uint32 Icebolt_Timer;
    uint32 FrostBreath_Timer;
    uint32 FrostAura_Timer;
    uint32 LifeDrain_Timer;
    uint32 Blizzard_Timer;
    uint32 Fly_Timer;
    uint32 Beserk_Timer;
    uint32 phase;
    bool landoff;
    uint32 land_Timer;

    void Reset()
    {
        m_uiCleaveTimer = 5000;
        FrostAura_Timer = 2000;
        FrostBreath_Timer = 2500;
        LifeDrain_Timer = 24000;
        Blizzard_Timer = 20000;
        Fly_Timer = 45000;
        Icebolt_Timer = 4000;
        land_Timer = 2000;
        Beserk_Timer = 15 * MINUTE * IN_MILLISECONDS;
        phase = 1;
        Icebolt_Count = 0;
        landoff = false;

        //m_creature->ApplySpellMod(SPELL_FROST_AURA, SPELLMOD_DURATION, -1);
    }
    
    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_ICEBOLT && pTarget->GetTypeId() == TYPEID_PLAYER)
            pTarget->CastSpell(pTarget, 28535, true);

        if (pSpell->Id == SPELL_FROST_BREATH_2 && pTarget->GetTypeId() == TYPEID_PLAYER && pTarget->HasAura(28535))
            pTarget->RemoveAurasDueToSpell(28535);
    }

    void Aggro(Unit* /*pWho*//*)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAPPHIRON, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*//*)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAPPHIRON, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (phase == 1)
        {
            if (FrostAura_Timer < uiDiff)
            {
                m_creature->CastSpell(m_creature->getVictim(),SPELL_FROST_AURA, true);
                FrostAura_Timer = 5000;
            }
            else FrostAura_Timer -= uiDiff;

            if (m_uiCleaveTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE))
                    m_uiCleaveTimer = urand(5000, 10000);
            }
            else
                m_uiCleaveTimer -= uiDiff;
            
            if (LifeDrain_Timer < uiDiff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                    DoCastSpellIfCan(target,SPELL_LIFE_DRAIN);

                LifeDrain_Timer = 24000;
            }
            else LifeDrain_Timer -= uiDiff;

            if (Blizzard_Timer < uiDiff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                    DoCastSpellIfCan(target,SPELL_BLIZZARD);

                Blizzard_Timer = 20000;
            }
            else Blizzard_Timer -= uiDiff;

            if (m_creature->GetHealthPercent() > 10.0f)
            {
                if (Fly_Timer < uiDiff)
                {
                    phase = 2;
                    m_creature->InterruptNonMeleeSpells(false);
                    m_creature->HandleEmote(EMOTE_ONESHOT_LIFTOFF);
                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->GetMotionMaster()->MoveIdle();
                    DoCastSpellIfCan(m_creature,11010);
                    m_creature->SetHover(true);
                    DoCastSpellIfCan(m_creature,18430);
                    Icebolt_Timer = 4000;
                    Icebolt_Count = 0;
                    landoff = false;
                }
                else Fly_Timer -= uiDiff;
            }
        }

        if (phase == 2)
        {            
            if (Icebolt_Timer < uiDiff && Icebolt_Count < 5)            // if less than 5 people alive, do x bolts instead of 5?
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                    DoCastSpellIfCan(target,SPELL_ICEBOLT);

                ++Icebolt_Count;
                Icebolt_Timer = 4000;
            }
            else Icebolt_Timer -= uiDiff;

            if (Icebolt_Count == 5 && !landoff)
            {
                if (FrostBreath_Timer < uiDiff)
                {
                    DoScriptText(EMOTE_BREATH, m_creature);
                    m_creature->CastSpell(m_creature, SPELL_FROST_BREATH, true);
                    DoCastSpellIfCan(m_creature->getVictim(),SPELL_FROST_BREATH_DUMMY, CAST_TRIGGERED);
                    land_Timer = 2000;
                    landoff = true;
                    FrostBreath_Timer = 6000;
                }
                else FrostBreath_Timer -= uiDiff;
            }

            if (landoff)
            {
                if (land_Timer < uiDiff)
                {
                    phase = 1;
                    m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
                    m_creature->SetHover(false);
                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    Fly_Timer = 67000;
                }
                else land_Timer -= uiDiff;
            }
        }
    
        if (Beserk_Timer < uiDiff)
        {
            DoScriptText(EMOTE_ENRAGE, m_creature);
            DoCastSpellIfCan(m_creature,SPELL_BESERK);
            Beserk_Timer = 300000;
        }
        else Beserk_Timer -= uiDiff;

        if (phase!=2)
            DoMeleeAttackIfReady();
    }
};*/

  enum
{
    /*EMOTE_BREATH                = -1533082,
    EMOTE_GENERIC_ENRAGED       = -1000003,
    EMOTE_FLY                   = -1533022,
    EMOTE_GROUND                = -1533083,*/

    SPELL_CLEAVE                = 19983,
    SPELL_TAIL_SWEEP            = 15847,
    SPELL_ICEBOLT               = 28526,        // animation
    SPELL_ICEBOLT_2               = 28522,      // damage + bolt
    SPELL_FROST_BREATH_DUMMY    = 30101,
    SPELL_FROST_BREATH          = 28524,            // triggers 29318
    SPELL_FROST_AURA            = 28531,
    SPELL_LIFE_DRAIN            = 28542,
    SPELL_CHILL                 = 28547,
    SPELL_SUMMON_BLIZZARD       = 28560,
    SPELL_BESERK                = 26662,
    SPELL_DRAGON_HOVER = 18430,

    NPC_YOU_KNOW_WHO            = 16474         // blizzard
};

static const float aLiftOffPosition[3] = {3522.386f, -5236.784f, 137.709f};

enum Phases
{
    PHASE_GROUND        = 1,
    PHASE_LIFT_OFF      = 2,
    PHASE_AIR_BOLTS     = 3,
    PHASE_AIR_BREATH    = 4,
    PHASE_LANDING       = 5,
};

struct MANGOS_DLL_DECL boss_sapphironAI : public ScriptedAI
{
    boss_sapphironAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_naxxramas*)pCreature->GetInstanceData();
        Reset();
    }

    instance_naxxramas* m_pInstance;

    uint32 m_uiCleaveTimer;
    uint32 m_uiTailSweepTimer;
    uint32 m_uiIceboltTimer;
    uint32 m_uiFrostBreathTimer;
    uint32 m_uiLifeDrainTimer;
    uint32 m_uiBlizzardTimer;
    uint32 m_uiFlyTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiLandTimer;
    uint32 FrostAura_Timer;

    uint32 m_uiIceboltCount;
    Phases m_Phase;

    void Reset()
    {
        FrostAura_Timer = 2000;
        m_uiCleaveTimer = 5000;
        m_uiTailSweepTimer = 12000;
        m_uiFrostBreathTimer = 7000;
        m_uiLifeDrainTimer = 11000;
        m_uiBlizzardTimer = 15000;                          // Wrong spell, "Once the encounter starts,based on your version of Naxx, this will be used x2 for normal and x6 on HC" 26607 more accurate, but should be moving
        m_uiFlyTimer = 45000;
        m_uiIceboltTimer = 5000;
        m_uiLandTimer = 0;
        m_uiBerserkTimer = 15 * MINUTE * IN_MILLISECONDS;
        m_Phase = PHASE_GROUND;
        m_uiIceboltCount = 0;

        SetCombatMovement(true);
        m_creature->SetHover(false);
        // m_creature->ApplySpellMod(SPELL_FROST_AURA, SPELLMOD_DURATION, -1);
        if(m_creature->HasAura(SPELL_DRAGON_HOVER))
            m_creature->RemoveAurasDueToSpell(SPELL_DRAGON_HOVER);
        if(m_creature->GetSplineFlags() == SPLINEFLAG_FLYING)
            m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAPPHIRON, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAPPHIRON, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAPPHIRON, FAIL);
    }
    
    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_ICEBOLT_2 && pTarget->GetTypeId() == TYPEID_PLAYER)
            pTarget->CastSpell(pTarget, 28535, true);

        if (pSpell->Id == SPELL_FROST_BREATH && pTarget->GetTypeId() == TYPEID_PLAYER && (pTarget->HasAura(28535) || pTarget->HasAura(SPELL_ICEBOLT_2)))
        {
            pTarget->RemoveAurasDueToSpell(28535);          // to do: remove the iceblocks 
            pTarget->RemoveAurasDueToSpell(SPELL_ICEBOLT_2);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_YOU_KNOW_WHO)
            if (Unit* pEnemy = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pEnemy);
    }

    void MovementInform(uint32 uiType, uint32 /*uiPointId*/) override
    {
        if (uiType == POINT_MOTION_TYPE && m_Phase == PHASE_LIFT_OFF)
        {
            m_creature->GenericTextEmote("Sapphiron lifts off into the air!", NULL, false);
            m_creature->HandleEmote(EMOTE_ONESHOT_LIFTOFF);            
            m_creature->SetHover(true);
            m_creature->CastSpell(m_creature, SPELL_DRAGON_HOVER, true);
            m_creature->SetSplineFlags(SPLINEFLAG_FLYING);
            m_Phase = PHASE_AIR_BOLTS;

            m_uiFrostBreathTimer = 5000;
            m_uiIceboltTimer = 5000;
            m_uiIceboltCount = 0;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_Phase)
        {
            case PHASE_GROUND:
                if (FrostAura_Timer < uiDiff)
                {
                    m_creature->CastSpell(m_creature->getVictim(),SPELL_FROST_AURA, true);
                    FrostAura_Timer = 5000;
                }
                else FrostAura_Timer -= uiDiff;
            
                if (m_uiCleaveTimer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
                    m_uiCleaveTimer = urand(5000, 10000);
                }
                else
                    m_uiCleaveTimer -= uiDiff;

                if (m_uiTailSweepTimer < uiDiff)        // should he have tail sweep?
                {
                    DoCastSpellIfCan(m_creature, SPELL_TAIL_SWEEP);
                    m_uiTailSweepTimer = urand(7000, 10000);
                }
                else
                    m_uiTailSweepTimer -= uiDiff;

                if (m_uiLifeDrainTimer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_LIFE_DRAIN);
                    m_uiLifeDrainTimer = 24000;
                }
                else
                    m_uiLifeDrainTimer -= uiDiff;

                if (m_uiBlizzardTimer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_BLIZZARD);
                    m_uiBlizzardTimer = 20000;
                }
                else
                    m_uiBlizzardTimer -= uiDiff;

                if (m_creature->GetHealthPercent() > 10.0f)
                {
                    if (m_uiFlyTimer < uiDiff)
                    {
                        m_Phase = PHASE_LIFT_OFF;
                        m_creature->InterruptNonMeleeSpells(false);
                        SetCombatMovement(false);
                        m_creature->GetMotionMaster()->Clear(false);
                        m_creature->GetMotionMaster()->MovePoint(1, aLiftOffPosition[0], aLiftOffPosition[1], aLiftOffPosition[2]);
                        
                        // TODO This should clear the target, too
                        return;
                    }
                    else
                        m_uiFlyTimer -= uiDiff;
                }

                // Only Phase in which we have melee attack!
                DoMeleeAttackIfReady();
                break;
            case PHASE_LIFT_OFF:
                break;
            case PHASE_AIR_BOLTS:
                if (m_uiIceboltCount == 5)
                {
                    if (m_uiFrostBreathTimer < uiDiff)
                    {
                        m_creature->CastSpell(m_creature, SPELL_FROST_BREATH, true);
                        m_creature->CastSpell(m_creature, SPELL_FROST_BREATH_DUMMY, true);
                        m_creature->GenericTextEmote("Sapphiron takes in a deep breath...", NULL, false);
                        m_Phase = PHASE_AIR_BREATH;
                        m_uiFrostBreathTimer = 5000;
                        m_uiLandTimer = 11000;
                    }
                    else
                        m_uiFrostBreathTimer -= uiDiff;
                }
                else
                {
                    if (m_uiIceboltTimer < uiDiff && m_uiIceboltCount < 5)            // if less than 5 people alive, do x bolts instead of 5?
                    {
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                        {
                            m_creature->CastSpell(target,SPELL_ICEBOLT_2, true);        // damage
                            //m_creature->CastSpell(m_creature,SPELL_ICEBOLT, true);      // animation                            
                        }

                        ++m_uiIceboltCount;
                        m_uiIceboltTimer = 4000;
                    }
                    else m_uiIceboltTimer -= uiDiff;
                }

                break;
            case PHASE_AIR_BREATH:
                if (m_uiLandTimer)
                {
                    if (m_uiLandTimer <= uiDiff)
                    {
                        // Begin Landing                        
                        m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
                        m_creature->SetHover(false);
                        
                        if(m_creature->HasAura(SPELL_DRAGON_HOVER))
                            m_creature->RemoveAurasDueToSpell(SPELL_DRAGON_HOVER);
                        
                        m_Phase = PHASE_LANDING;
                        m_uiLandTimer = 2000;
                    }
                    else
                        m_uiLandTimer -= uiDiff;
                }

                break;
            case PHASE_LANDING:
                if (m_uiLandTimer < uiDiff)
                {
                    m_Phase = PHASE_GROUND;
                    if(m_creature->GetSplineFlags() == SPLINEFLAG_FLYING)
                        m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
                    m_creature->GenericTextEmote("Sapphiron resumes his attacks!", NULL, false);
                    SetCombatMovement(true);
                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                
                    m_uiFlyTimer = 67000;
                    m_uiLandTimer = 0;
                }
                else
                    m_uiLandTimer -= uiDiff;
                break;
        }

        // Enrage can happen in any phase
        if (m_uiBerserkTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature, SPELL_BESERK, true);
            m_creature->GenericTextEmote("Sapphiron becomes enraged!", NULL, false); 
            m_uiBerserkTimer = 300000;
        }
        else         
            m_uiBerserkTimer -= uiDiff;
    }
};
  
CreatureAI* GetAI_boss_sapphiron(Creature* pCreature)
{
    return new boss_sapphironAI(pCreature);
}

void AddSC_boss_sapphiron()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_sapphiron";
    pNewscript->GetAI = &GetAI_boss_sapphiron;
    pNewscript->RegisterSelf();
}
