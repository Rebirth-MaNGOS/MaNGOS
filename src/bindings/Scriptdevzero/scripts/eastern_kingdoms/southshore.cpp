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
SDName: Southshore
SD%Complete: 
SDComment: Quest support: 0. Support for Attack on Southshore event, for now use them as active objects but would need to be loaded even when no player is close to spawn them.
SDCategory: Southshore
EndScriptData */

/* ContentData
EndContentData */

#include "precompiled.h"

/*######
## npc_southshore_crier
######*/

enum
{
	YELL_SPAWN				= -1720024,
	SAY_1					= -1720025,
	SAY_2					= -1720026,
	SAY_DIE					= -1720027,
	MAGISTRATE_SAY			= -1720028,

	NPC_MAGISTRATE			= 2276,
};

struct MANGOS_DLL_DECL npc_southshore_crierAI : public ScriptedAI
{
    npc_southshore_crierAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_creature->SetStandState(UNIT_STAND_STATE_STAND);
		DoScriptText(YELL_SPAWN, m_creature);
		m_bOutro = false;
        Reset();
    }
		
	uint32 m_uiSpeechTimer;
    uint8 m_uiSpeechStep;
	bool m_bOutro;

    void Reset()
    {
        if (!m_creature->isActiveObject() && m_creature->isAlive())
            m_creature->SetActiveObjectState(true);

		m_bOutro = false;
		m_uiSpeechTimer = 0;
        m_uiSpeechStep = 1;
    }

	void MovementInform(uint32 /*uiMotionType*/, uint32 uiPointId)
    {
        switch(uiPointId)
        {
			case 5:
				DoScriptText(SAY_1, m_creature);
				m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
				break;
			case 20:
				DoScriptText(SAY_1, m_creature);
				m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
				break;
			case 25:
				DoScriptText(SAY_1, m_creature);
				m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
				break;
            case 28:
				DoScriptText(SAY_2, m_creature);
				break;
			case 29:
				m_creature->GenericTextEmote("Southshore Crier grabs the back of his neck and his eyes go wide.", NULL, false);
				m_creature->HandleEmote(EMOTE_ONESHOT_WOUND);
				break;
			case 30:
				DoScriptText(SAY_DIE, m_creature);
				break;
			case 31:				// not working
			/*	if (Creature* pMagistrate = m_creature->GetClosestCreatureWithEntry(m_creature, NPC_MAGISTRATE, 50.0f))
					if (pMagistrate)
						DoScriptText(MAGISTRATE_SAY, pMagistrate);*/

				m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);		// kill self
				break;
		}
	}

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_southshore_crier(Creature* pCreature)
{
    return new npc_southshore_crierAI(pCreature);
}

/*######
## mob_shadowy_assassin
######*/

enum 
{
	SPELL_STEALTH			= 1786,
	SPELL_POISON_PROCC		= 3616,
};

struct MANGOS_DLL_DECL mob_shadowy_assassinAI : public ScriptedAI
{
    mob_shadowy_assassinAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		DoCastSpellIfCan(m_creature, SPELL_POISON_PROCC, CAST_AURA_NOT_PRESENT);
        Reset();
    }

    void Reset()
    {
		DoCastSpellIfCan(m_creature, SPELL_STEALTH, CAST_AURA_NOT_PRESENT);
        
        if (!m_creature->isActiveObject() && m_creature->isAlive())
            m_creature->SetActiveObjectState(true);
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_shadowy_assassin(Creature* pCreature)
{
    return new mob_shadowy_assassinAI(pCreature);
}

/*######
## npc_southshore_guard
######*/

struct MANGOS_DLL_DECL npc_southshore_guardAI : public ScriptedAI
{
    npc_southshore_guardAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        if (!m_creature->isActiveObject() && m_creature->isAlive())
            m_creature->SetActiveObjectState(true);
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_southshore_guard(Creature* pCreature)
{
    return new npc_southshore_guardAI(pCreature);
}

void AddSC_southshore()
{
    Script* pNewscript;

	pNewscript = new Script;
	pNewscript->Name = "npc_southshore_crier";
	pNewscript->GetAI = &GetAI_npc_southshore_crier;
	pNewscript->RegisterSelf();

	pNewscript = new Script;
	pNewscript->Name = "mob_shadowy_assassin";
	pNewscript->GetAI = &GetAI_mob_shadowy_assassin;
	pNewscript->RegisterSelf();

	pNewscript = new Script;
	pNewscript->Name = "npc_southshore_guard";
	pNewscript->GetAI = &GetAI_npc_southshore_guard;
	pNewscript->RegisterSelf();
}
