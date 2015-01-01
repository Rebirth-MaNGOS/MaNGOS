#include "precompiled.h"

enum
{
    SPELL_CURSE_OF_THE_ELEMENTAL_LORD	= 26977,
    SPELL_FIERY_BURST			= 13900,
    SPELL_MIGHTY_BLOW			= 14099,
    SPELL_FIRE_STORM			= 13899
};

struct MANGOS_DLL_DECL boss_lord_incendiusAI : public ScriptedAI
{
    boss_lord_incendiusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_FIRE, true);
        Reset();
    }

    uint32 m_uiCurseTimer;
    uint32 m_uiFieryBurstTimer;
    uint32 m_uiMightyBlowTimer;
    uint32 m_uiFireStormTimer;

    void Reset()
    {
        m_uiCurseTimer = urand(1000, 5000);
        m_uiFieryBurstTimer = urand(6000, 11000);
        m_uiMightyBlowTimer = urand(9000, 17000);
        m_uiFireStormTimer = urand(12000, 16000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // If Incendius moves more than 100 yd from his spawnpoint we make him reset to avoid abuse.
        float x, y, z;

        m_creature->GetRespawnCoord(x, y, z);

        if (m_creature->GetDistance(x, y, z) >= 100.f)
        {
            m_creature->CombatStop(true);
            m_creature->getThreatManager().clearReferences();
            m_creature->SetEvadeMode(true);

        }


        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCurseTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_THE_ELEMENTAL_LORD);

            m_uiCurseTimer = urand(20000, 28000);
        }
        else
            m_uiCurseTimer -= uiDiff;

        if (m_uiFieryBurstTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIERY_BURST);

            m_uiFieryBurstTimer = urand(9000, 16000);
        }
        else
            m_uiFieryBurstTimer -= uiDiff;

        if (m_uiMightyBlowTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIGHTY_BLOW);

            m_uiMightyBlowTimer = urand(18000, 24000);
        }
        else
            m_uiMightyBlowTimer -= uiDiff;

        if (m_uiFireStormTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIRE_STORM);

            m_uiFireStormTimer = urand(11000, 14000);
        }
        else
            m_uiFireStormTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lord_incendius(Creature* pCreature)
{
    return new boss_lord_incendiusAI(pCreature);
}

void AddSC_boss_lord_incendius()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "boss_lord_incendius";
    pNewScript->GetAI = &GetAI_boss_lord_incendius;
    pNewScript->RegisterSelf();
}
