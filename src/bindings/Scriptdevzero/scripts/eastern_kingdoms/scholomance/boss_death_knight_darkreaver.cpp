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
SDName: Boss_Death_knight_darkreaver
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_BLOOD_LEECH           = 22644,
    SPELL_CLEAVE                = 15284,
    SPELL_DOMINATE_MIND         = 7645,
    SPELL_SHADOW_BOLT           = 17393,

    SPELL_HOLY_BLAST            = 23269,
    SPELL_JUDGMENT              = 20271,
    // Aspect of banality
    SPELL_BEFUDDLEMENT          = 8140,
    SPELL_CURSE_OF_THORNS       = 16247,
    SPELL_DEMORALIZING_SHOUT    = 16244,
    // Aspect of Corruption
    SPELL_CORRUPTION            = 23244,
    SPELL_SHADOW_WORD_PAIN      = 23268,
    // Aspect of Malice
    SPELL_CURSE_OF_BLOOD        = 16098,
    SPELL_REND                  = 13738,
    // Aspect of Shadow
    SPELL_SHADOW_BOLT_ASPECT    = 15232,
    SPELL_SHADOW_BOLT_VOLLEY    = 17228,
    SPELL_SHADOW_SHIELD         = 22417,
    SPELL_SHADOW_SHOCK          = 17289,
    
    EVENT_SPELL_PLACE_SCRYER    = 8436,

	SPELL_JUDGEMENT_OF_WISDOM			= 20355,			// banality
	SPELL_JUDGEMENT_OF_JUSTICE			= 20184,			// malicious
	SPELL_JUDGEMENT_OF_RIGHTEOUSNESS	= 20286,			// corrupted
	SPELL_JUDGEMENT_OF_LIGHT			= 20346,			// shadowed

	YELL_SPAWN							= -1720042,
};

struct MANGOS_DLL_DECL boss_death_knight_darkreaverAI : public ScriptedAI
{
    boss_death_knight_darkreaverAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
		DoScriptText(YELL_SPAWN, m_creature);
		m_creature->SetInCombatWithZone();
        Reset();
    }

    instance_scholomance* m_pInstance;

    uint32 m_uiBloodLeechTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiDominateMindTimer;
    uint32 m_uiShadowBoltTimer;

    void Reset()
    {
        m_uiBloodLeechTimer = urand(10000,12000);
        m_uiCleaveTimer = urand(3000,4000);
        m_uiDominateMindTimer = urand(15000,17000);
        m_uiShadowBoltTimer = urand(7000,9000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GREAT_OSSUARY, FAIL);

        m_creature->RemoveFromWorld();
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GREAT_OSSUARY, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance && pKiller->GetTypeId() == TYPEID_PLAYER)
        {
            if (((Player*)pKiller)->GetTeam() == ALLIANCE)
                DoSpawnCreature(NPC_DARKREAVERS_FALLEN_CHARGER, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 300000);

            m_pInstance->SetData(TYPE_GREAT_OSSUARY, DONE);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Blood Leech
        if (m_uiBloodLeechTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLOOD_LEECH);
            m_uiBloodLeechTimer = urand(20000,25000);
        }
        else
            m_uiBloodLeechTimer -= uiDiff;

        // Cleave
        if (m_uiCleaveTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(3000,6000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Dominate Mind
        if (m_uiDominateMindTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DOMINATE_MIND);
            m_uiDominateMindTimer = urand(30000,40000);
        }
        else
            m_uiDominateMindTimer -= uiDiff;

        // Shadow Bolt
        if (m_uiShadowBoltTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT);
            m_uiShadowBoltTimer = urand(6000,8000);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_death_knight_darkreaver(Creature* pCreature)
{
    return new boss_death_knight_darkreaverAI(pCreature);
}

struct MANGOS_DLL_DECL mob_aspectAI : public ScriptedAI
{
    mob_aspectAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

	bool m_bNoMove;
    uint32 m_uiSpell1Entry;
    uint32 m_uiSpell2Entry;
    uint32 m_uiSpell3Entry;
    uint32 m_uiSpell4Entry;
	uint32 m_uiSpell5Entry;
    uint32 m_uiSpell1Timer;
    uint32 m_uiSpell2Timer;
    uint32 m_uiSpell3Timer;
    uint32 m_uiSpell4Timer;
	uint32 m_uiSpell5Timer;

    void Reset()
    {
        m_bNoMove = false;
        m_uiSpell1Timer = urand(4000,6000);
        m_uiSpell2Timer = urand(8000,12000);
        m_uiSpell3Timer = urand(15000,20000);
        m_uiSpell4Timer = urand(30000,40000);
		m_uiSpell5Timer = urand(1500,3000);

        switch(m_creature->GetEntry())						// not doing some spells because they have no mana?
        {
			case NPC_BANAL_SPIRIT:
				m_uiSpell1Entry = 0;
                m_uiSpell2Entry = SPELL_BEFUDDLEMENT;		// not doing this?
                m_uiSpell3Entry = SPELL_DEMORALIZING_SHOUT;
                m_uiSpell4Entry = 0;
				m_uiSpell5Entry = 0;
				break;
            case NPC_ASPECT_OF_BANALITY:
                m_uiSpell1Entry = SPELL_BEFUDDLEMENT;		// not doing this?
                m_uiSpell2Entry = SPELL_CURSE_OF_THORNS;
                m_uiSpell3Entry = SPELL_DEMORALIZING_SHOUT;
                m_uiSpell4Entry = 0;
				m_uiSpell5Entry = 0;
                break;
			case NPC_MALICIOUS_SPIRIT:
				m_uiSpell1Entry = 0;
                m_uiSpell2Entry = SPELL_REND;
                m_uiSpell3Entry = 0;
                m_uiSpell4Entry = 0;
				m_uiSpell5Entry = 0;
				break;
            case NPC_ASPECT_OF_MALICE:
                m_uiSpell1Entry = SPELL_REND;
                m_uiSpell2Entry = 0;
                m_uiSpell3Entry = 0;
                m_uiSpell4Entry = SPELL_CURSE_OF_BLOOD;
				m_uiSpell5Entry = 0;
                break;
			case NPC_CORRUPTED_SPIRIT:
				m_uiSpell1Entry = 0;
                m_uiSpell2Entry = 0;
                m_uiSpell3Entry = SPELL_CORRUPTION;		// not doing this? 
                m_uiSpell4Entry = 0;
				m_uiSpell5Entry = 0;
				break;
            case NPC_ASPECT_OF_CORRUPTION:
                m_uiSpell1Entry = 0;
                m_uiSpell2Entry = SPELL_SHADOW_WORD_PAIN;
                m_uiSpell3Entry = SPELL_CORRUPTION;		// not doing this?
                m_uiSpell4Entry = 0;
				m_uiSpell5Entry = 0;
                break;
			case NPC_SHADOWED_SPIRIT:
				m_uiSpell1Entry = 0;
                m_uiSpell2Entry = 0;
                m_uiSpell3Entry = 0;
                m_uiSpell4Entry = SPELL_SHADOW_SHIELD;
				m_uiSpell5Entry = SPELL_SHADOW_BOLT_ASPECT;
				break;
            case NPC_ASPECT_OF_SHADOW:
                m_uiSpell1Entry = 0;
                m_uiSpell2Entry = 0;
                m_uiSpell3Entry = SPELL_SHADOW_SHOCK;		// not doing this?
                m_uiSpell4Entry = SPELL_SHADOW_SHIELD;
				m_uiSpell5Entry = SPELL_SHADOW_BOLT_ASPECT;
                break;
        }
    }

    void JustReachedHome()
    {
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GREAT_OSSUARY, IN_PROGRESS);

		switch(m_creature->GetEntry())
        {
			case NPC_CORRUPTED_SPIRIT:				// pull one and the whole room should come NOT WORKING
				m_creature->CallForHelp(40.0f);
		}
    }

	void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)				// make sure they won't attack each other after holy blast
    {
		if (pDoneBy->GetTypeId() != TYPEID_PLAYER)
			m_creature->getThreatManager().addThreat(pDoneBy, -10000);
	}

    void JustDied(Unit* /*pKiller*/)
    {
		m_creature->ForcedDespawn();
    }

    void Execute(uint32 uiSpellEntry)
    {
        Unit* pTarget = NULL;
        switch(uiSpellEntry)
        {
            case SPELL_SHADOW_SHIELD:
                pTarget = m_creature;
                break;
            case SPELL_DEMORALIZING_SHOUT:
            case SPELL_CURSE_OF_BLOOD:
            case SPELL_REND:      
            case SPELL_SHADOW_BOLT_VOLLEY:
                pTarget = m_creature->getVictim();
                break;
			case SPELL_SHADOW_BOLT_ASPECT:
            case SPELL_BEFUDDLEMENT:
            case SPELL_CURSE_OF_THORNS:
            case SPELL_CORRUPTION:
            case SPELL_SHADOW_WORD_PAIN:
            case SPELL_SHADOW_SHOCK:
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                break;
        }
        if (pTarget && uiSpellEntry != 0)
            DoCastSpellIfCan(pTarget, uiSpellEntry);
    }

    void SpellHit(Unit* pUnit, const SpellEntry* pSpell)
    {
		switch(m_creature->GetEntry())
		{
			case NPC_BANAL_SPIRIT:
				if (pSpell->Id == SPELL_JUDGEMENT_OF_WISDOM)
				{
					m_creature->GenericTextEmote("Banal Spirit recoils in pain as the Judgement of Wisdom washes over it!", NULL, false);
					DoCast(m_creature, SPELL_HOLY_BLAST, true);
				}
				break;
			case NPC_ASPECT_OF_BANALITY:
				if (pSpell->Id == SPELL_JUDGEMENT_OF_WISDOM)
				{
					m_creature->GenericTextEmote("Aspect of Banality recoils in pain as the Judgement of Wisdom washes over it!", NULL, false);
					DoCast(m_creature, SPELL_HOLY_BLAST, true);
				}
                break;
			case NPC_MALICIOUS_SPIRIT:
				if (pSpell->Id == SPELL_JUDGEMENT_OF_JUSTICE)
				{
					m_creature->GenericTextEmote("Malicious Spirit recoils in pain as the Judgement of Justice washes over it!", NULL, false);
					DoCast(m_creature, SPELL_HOLY_BLAST, true);
				}
				break;
            case NPC_ASPECT_OF_MALICE:
				if (pSpell->Id == SPELL_JUDGEMENT_OF_JUSTICE)
				{
					m_creature->GenericTextEmote("Aspect of Malice recoils in pain as the Judgement of Justice washes over it!", NULL, false);
					DoCast(m_creature, SPELL_HOLY_BLAST, true);
				}
                break;
			case NPC_CORRUPTED_SPIRIT:
				if (pSpell->Id == SPELL_JUDGEMENT_OF_RIGHTEOUSNESS)
				{
					m_creature->GenericTextEmote("Corrupted Spirit recoils in pain as the Judgement of Righteousness washes over it!", NULL, false);
					DoCast(m_creature, SPELL_HOLY_BLAST, true);
				}
				break;
            case NPC_ASPECT_OF_CORRUPTION:
				if (pSpell->Id == SPELL_JUDGEMENT_OF_RIGHTEOUSNESS)
				{
					m_creature->GenericTextEmote("Aspect of Corruption recoils in pain as the Judgement of Righteousness washes over it!", NULL, false);
					DoCast(m_creature, SPELL_HOLY_BLAST, true);
				}
                break;
			case NPC_SHADOWED_SPIRIT:
				if (pSpell->Id == SPELL_JUDGEMENT_OF_LIGHT)
				{
					m_creature->GenericTextEmote("Shadowed Spirit recoils in pain as the Judgement of Light washes over it!", NULL, false);
					DoCast(m_creature, SPELL_HOLY_BLAST, true);
				}
                break;
            case NPC_ASPECT_OF_SHADOW:
				if (pSpell->Id == SPELL_JUDGEMENT_OF_LIGHT)
				{
					m_creature->GenericTextEmote("Aspect of Shadow recoils in pain as the Judgement of Light washes over it!", NULL, false);
					DoCast(m_creature, SPELL_HOLY_BLAST, true);
				}
                break;
	    }
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpell1Timer < uiDiff)
        {
            Execute(m_uiSpell1Entry);
            m_uiSpell1Timer = urand(3000,6000);
        }
        else
            m_uiSpell1Timer -= uiDiff;

        if (m_uiSpell2Timer < uiDiff)
        {
            Execute(m_uiSpell2Entry);
            m_uiSpell2Timer = urand(10000,15000);
        }
        else
            m_uiSpell2Timer -= uiDiff;

        if (m_uiSpell3Timer < uiDiff)
        {
            Execute(m_uiSpell3Entry);
            m_uiSpell3Timer = urand(15000,20000);
        }
        else
            m_uiSpell3Timer -= uiDiff;

        if (m_uiSpell4Timer < uiDiff)
        {
            Execute(m_uiSpell4Entry);
            m_uiSpell4Timer = urand(30000,40000);
        }
        else
            m_uiSpell4Timer -= uiDiff;

		if (m_uiSpell5Timer < uiDiff)		// only used for shadow bolt
        {
            Execute(m_uiSpell5Entry);
            m_uiSpell5Timer = urand(2000,4000);
        }
        else
            m_uiSpell5Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_aspect(Creature* pCreature)
{
    return new mob_aspectAI(pCreature);
}

bool ProcessEventId_event_spell_place_scryer(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_PLAYER && uiEventId == EVENT_SPELL_PLACE_SCRYER)
    {
        instance_scholomance* m_pInstance = (instance_scholomance*)((Player*)pSource)->GetInstanceData();

        if (m_pInstance && m_pInstance->GetData(TYPE_GREAT_OSSUARY) == NOT_STARTED)
        {
            m_pInstance->SetData(TYPE_GREAT_OSSUARY, IN_PROGRESS);
			m_pInstance->OssuaryStartEvent();
        }
    }
    return true;
}

void AddSC_boss_death_knight_darkreaver()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_death_knight_darkreaver";
    pNewscript->GetAI = &GetAI_boss_death_knight_darkreaver;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_aspect";
    pNewscript->GetAI = &GetAI_mob_aspect;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "event_spell_place_scryer";
    pNewscript->pProcessEventId = &ProcessEventId_event_spell_place_scryer;
    pNewscript->RegisterSelf();
}
