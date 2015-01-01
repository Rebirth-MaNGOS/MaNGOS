#include "precompiled.h"
#include "sc_creature.h"



/*#########################
## engineering_target_dummy
##########################*/

#define TARGET_DUMMY_SPAWN_EFFECT 4507

struct MANGOS_DLL_DECL engineering_target_dummyAI : public Scripted_NoMovementAI
{
    engineering_target_dummyAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) {
        Reset();
    }

    uint32 DurationTimer, TauntTimer;

    void Reset()
    {
        SetCombatMovement(false);
        DurationTimer = 15000;
        TauntTimer = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (DurationTimer <= uiDiff)
        {
            // Kill the dummy when the duration ends.
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
        }
        else
        {
            DurationTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;


        if (TauntTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, TARGET_DUMMY_SPAWN_EFFECT) == CAST_OK)
                TauntTimer = 3000;
        }
        else
        {
            TauntTimer -= uiDiff;
        }
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_creature->GetOwner())
        {
            m_creature->SetLootRecipient(m_creature->GetOwner());

            // Allow loot only if has loot_id in creature_template
            m_creature->PrepareBodyLootState(true);
        }
    }
};

CreatureAI* GetAI_engineering_target_dummy(Creature* pCreature)
{
    return new engineering_target_dummyAI(pCreature);
}

/*############################
## engineering_explosive_sheep
############################*/

#define EXPLOSIVE_SHEEP_ITEM 4384
#define EXPLOSIVE_SHEEP_SUMMON_SPELL 4074
#define SHEEP_EXPLOSION_SPELL 4050

struct MANGOS_DLL_DECL engineering_explosive_sheepAI : public ScriptedAI
{
    engineering_explosive_sheepAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 DurationTimer;
    bool is_chasing;

    Unit* chasing_victim;

    void Reset()
    {
        // Make the sheep follow the player.
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveFollow(m_creature->GetOwner(), 0.5f, 3.14f / 6.0f);

        is_chasing = false;

        DurationTimer = 180000;
    }

    void JustDied(Unit* /*pKiller*/)
    {
        time_t curTime = time(NULL);

        Player* owner = dynamic_cast<Player*>(m_creature->GetOwner());

        if (owner == NULL)
            return;

        owner->RemoveGuardian(dynamic_cast<Pet*>(m_creature));

        // Make sure the cooldown of the sheep is activated.
        owner->AddSpellCooldown(EXPLOSIVE_SHEEP_SUMMON_SPELL, EXPLOSIVE_SHEEP_ITEM, curTime + 60);
        owner->SendCooldownUpdate();

    }

    void UpdateAI(const uint32 uiDiff)
    {

        if (DurationTimer <= uiDiff)
        {
            // Destroy the sheep when duration ends.
            m_creature->SetDeathState(JUST_DIED);
            m_creature->SetHealth(0);
        }
        else
        {
            DurationTimer -= uiDiff;
        }

        if (!m_creature->getVictim())
        {
            std::list<Creature*> hostile_list = DoFindHostile(30);

            if (!hostile_list.empty())
            {
                m_creature->Attack(hostile_list.front(), false);
                AttackStart(hostile_list.front());
            }
            else
                return;
        }


		if (m_creature->getVictim())
		{
			// Explode if we're close.
			float distance = m_creature->getVictim()->GetDistance(m_creature);
			if (distance < 2.0f)
				DoCastSpellIfCan(m_creature, SHEEP_EXPLOSION_SPELL);
		}
    }

    void AttackStart(Unit* victim)
    {
        Player* pOwner = dynamic_cast<Player*>(m_creature->GetCharmerOrOwner());

        if(!pOwner || !m_creature)  // Make sure the sheep has a valid victim.
            return;

        if (!is_chasing)
        {
            // Make the sheep chase its victim and initialize all combat.
            m_creature->AddThreat(victim);
            pOwner->AddThreat(victim);

            m_creature->Attack(victim, true);

            pOwner->SetInCombatWith(victim);
            m_creature->SetInCombatWith(victim);

            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MovePoint(0, victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ());

            is_chasing = true;
            chasing_victim = victim;
        }

        // If we were spawned close to the attacker we should explode.
        float distance = victim->GetDistance(m_creature);
        if (distance < 2.0f)
            DoCastSpellIfCan(m_creature, SHEEP_EXPLOSION_SPELL);
    }

    void MovementInform(uint32 /*uiMotionType*/, uint32 /*pointID*/)
    {
        // We're at the victim, update coordinates.
        m_creature->Relocate(chasing_victim->GetPositionX(), chasing_victim->GetPositionY(), chasing_victim->GetPositionZ());

        if (!chasing_victim->isAlive())
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveFollow(m_creature->GetOwner(), 0.5f, 3.14f / 6.0f);
        }

        is_chasing = false;
    }

    // Explode the sheep if it takes damage.
    void DamageTaken(Unit* /*pDoneBy*/, uint32& /*uiDamage*/)
    {
        DoCastSpellIfCan(m_creature, SHEEP_EXPLOSION_SPELL);
    }

};

CreatureAI* GetAI_engineering_explosive_sheep(Creature* pCreature)
{
    return new engineering_explosive_sheepAI(pCreature);
}


/*########################
## engineering_alarm-o-bot
########################*/

#define DETECTION_PULSE_SPELL 23002

struct MANGOS_DLL_DECL engineering_alarmobotAI : public ScriptedAI
{
    engineering_alarmobotAI(Creature* pCreature) : ScriptedAI(pCreature) {
        Reset();
    };

    uint32 DurationTimer, DetectionPulseTimer;

    void Reset()
    {
        // Make the bot follow the player and ignore attackers.
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveFollow(m_creature->GetOwner(), 0.5f, 3.14f / 6.0f);
        SetCombatMovement(false);

        DurationTimer = 600000;
        DetectionPulseTimer = 5000;
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if (DurationTimer <= uiDiff)
        {
            // Destroy the Alarm-o-Bot when the duration ends.
            KillBot();

            return;
        }
        else
        {
            DurationTimer -= uiDiff;
        }

        if (DetectionPulseTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, DETECTION_PULSE_SPELL);

            DetectionPulseTimer = 5000;
        }
        else
        {
            DetectionPulseTimer -= uiDiff;
        }

        Unit* owner = m_creature->GetOwner();

        if (owner)
        {
            if (owner->IsTaxiFlying() || !m_creature->IsInRange(owner, 0, 5000))
            {
                // Destroy the Alarm-o-Bot if the player flys somewhere or gets to far away.
                KillBot();
                return;
            }

            // Update the runspeed so it can keep up with the player.
            m_creature->SetSpeedRate(MOVE_WALK, m_creature->GetOwner()->GetSpeedRate(MOVE_RUN) * 3, true);

        }
        else
        {
            // Destroy the bot if it loses its owner.
            KillBot();
            return;
        }

    }

    void KillBot()
    {
        m_creature->SetDeathState(JUST_DIED);
        m_creature->SetHealth(0);
    }

    void JustDied(Unit* /*pKiller*/)
    {

        if (m_creature->GetOwner())
        {
            m_creature->SetLootRecipient(m_creature->GetOwner());

            // Allow loot only if has loot_id in creature_template
            m_creature->PrepareBodyLootState(true);
        }
    }
};

CreatureAI* GetAI_engineering_alarmobot(Creature* pCreature)
{
    return new engineering_alarmobotAI(pCreature);
}

/*######
## AddSc
######*/

void AddSC_engineering()
{
    Script* pNewScript;

    pNewScript = new Script();
    pNewScript->Name = "engineering_target_dummy";
    pNewScript->GetAI = &GetAI_engineering_target_dummy;
    pNewScript->RegisterSelf();

    pNewScript = new Script();
    pNewScript->Name = "engineering_explosive_sheep";
    pNewScript->GetAI = &GetAI_engineering_explosive_sheep;
    pNewScript->RegisterSelf();

    pNewScript = new Script();
    pNewScript->Name = "engineering_alarmobot";
    pNewScript->GetAI = &GetAI_engineering_alarmobot;
    pNewScript->RegisterSelf();

}
