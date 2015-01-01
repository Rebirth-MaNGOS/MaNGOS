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
SDName: Boss_Magmadar
SD%Complete: 90
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum eMagmadar
{
    EMOTE_GENERIC_FRENZY_KILL   = -1000001,

    SPELL_FRENZY                = 19451,
    SPELL_MAGMASPIT             = 19449,                    // This is actually a buff he gives himself
    SPELL_PANIC                 = 19408,                    // Tank doesnt lose aggro when affected by fear, need to fix that within the script
    SPELL_LAVA_BOMB             = 19411,
    SPELL_LAVA_BREATH           = 19272
};

struct MANGOS_DLL_DECL boss_magmadarAI : public ScriptedAI
{
    boss_magmadarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
        Reset();
    }

    instance_molten_core* m_pInstance;

    bool mTankFeared;

	uint32 m_uiFrenzyTimer;
    uint32 m_uiPanicTimer;
    uint32 m_uiLavabombTimer;
    uint32 m_uiLavaBreathTimer;

	ObjectGuid mTank;

    void Reset()
    {
        m_creature->SetBoundingValue(0, 7);
        m_creature->SetBoundingValue(1, 4);
        m_uiFrenzyTimer = 30000;
        m_uiPanicTimer = 20000;
        m_uiLavabombTimer = 12000;
        m_uiLavaBreathTimer = 8000;
		mTank = ObjectGuid();
		mTankFeared = false;

        DoCastSpellIfCan(m_creature, SPELL_MAGMASPIT, CAST_TRIGGERED);

		MakeTauntImmune();
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAGMADAR, FAIL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAGMADAR, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Frenzy
        if (m_uiFrenzyTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
            {
				//Another shim emote- this way works, but doesn't do text replacement, normal emotes don't work in MaNGOS Zero... some day
				//it would be nice to fix this.
				m_creature->GenericTextEmote("Magmadar goes into a killing frenzy!",NULL,true);
                //DoScriptText(EMOTE_GENERIC_FRENZY_KILL, m_creature);
                m_uiFrenzyTimer = 15000;
            }
        }
        else
            m_uiFrenzyTimer -= uiDiff;

        // Panic
        if (m_uiPanicTimer <= uiDiff)
        {
			if (Unit* pTank = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO,0))
				mTank = pTank->GetObjectGuid();

            DoCastSpellIfCan(m_creature->getVictim(), SPELL_PANIC);
            m_uiPanicTimer = 30000;
        }
        else
			m_uiPanicTimer -= uiDiff;

		// Tank aggro loss at fear
		if (!mTankFeared && mTank)
		{
			Unit* pTank = m_creature->GetMap()->GetUnit(mTank);
			if (pTank && pTank->HasAura(SPELL_PANIC))
			{
				mTankFeared = true;
				m_creature->getThreatManager().modifyThreatPercent(pTank,-99);
			}
		}

		// Tank aggro regain after fear
		if (mTankFeared && mTank)
		{
			Unit* pTank = m_creature->GetMap()->GetUnit(mTank);
			if (pTank && !pTank->HasAura(SPELL_PANIC))
			{
				mTankFeared = false;
				m_creature->getThreatManager().modifyThreatPercent(pTank,11000);
			}
		}

        // Lavabomb
        if (m_uiLavabombTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_LAVA_BOMB);
            m_uiLavabombTimer = 8000;
        }
        else
            m_uiLavabombTimer -= uiDiff;

        // Lava Breath
        if (m_uiLavaBreathTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_LAVA_BREATH);
            m_uiLavaBreathTimer = 14000;
        }
        else
            m_uiLavaBreathTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_magmadar(Creature* pCreature)
{
    return new boss_magmadarAI(pCreature);
}

void AddSC_boss_magmadar()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_magmadar";
    pNewscript->GetAI = &GetAI_boss_magmadar;
    pNewscript->RegisterSelf();
}
