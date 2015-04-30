/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Boss_Kurinnaxx
SD%Complete: 100
SDComment:
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "ruins_of_ahnqiraj.h"

enum eKurrinnax
{
    SPELL_ENRAGE              = 26527,
    SPELL_MORTAL_WOUND        = 25646,
    SPELL_SAND_TRAP           = 25648,
    SPELL_SAND_TRAP_EFFECT    = 25656,
    SPELL_SUMMON_PLAYER       = 26446,
    SPELL_TRASH               = 3391,
    SPELL_WIDE_SLASH          = 25814,
};

struct MANGOS_DLL_DECL boss_kurinnaxxAI : public ScriptedAI
{
    boss_kurinnaxxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData());
        Reset();
    }

    instance_ruins_of_ahnqiraj* m_pInstance;

    bool m_bIsSandTrap;

    uint32 m_uiMortalWoundTimer;
    uint32 m_uiSandTrapTimer;
    uint32 m_uiSummonPlayerTimer;
    uint32 m_uiTrashTimer;
    uint32 m_uiWideSlashTimer;

    void Reset()
    {
        m_bIsSandTrap = false;

        m_uiMortalWoundTimer = urand(6000,8000);
        m_uiSandTrapTimer = urand(9000,11000);
        m_uiSummonPlayerTimer = urand(10000,12000);
        m_uiTrashTimer = urand(6000,8000);
        m_uiWideSlashTimer = urand(7000,9000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KURINNAXX, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KURINNAXX, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KURINNAXX, DONE);
    }

	void SpellHit(Unit* pCaster, SpellEntry const* pSpell) // emote if he enrages
    {
        if (pSpell->Id == SPELL_ENRAGE)
			m_creature->GenericTextEmote("Kurinnaxx goes into a frenzy!", NULL, false);
	}


    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If we are < 30% cast enrage
        if (HealthBelowPct(30))
            DoCastSpellIfCan(m_creature, SPELL_ENRAGE, CAST_AURA_NOT_PRESENT);        

        // Mortal Wound
        if (m_uiMortalWoundTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_WOUND);
            m_uiMortalWoundTimer = urand(8000,12000);
        }
        else
            m_uiMortalWoundTimer -= uiDiff;

        // Sand Trap
        if (m_uiSandTrapTimer <= uiDiff)
        {
            if(!m_bIsSandTrap)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
					pTarget->CastSpell(pTarget, SPELL_SAND_TRAP, true, 0, 0, m_creature->GetObjectGuid());
                    m_bIsSandTrap = true;
                }
                m_uiSandTrapTimer = 5000;//3000;
            }
            else
            {
                if (GameObject* pTrap = GetClosestGameObjectWithEntry(m_creature, GO_SAND_TRAP, DEFAULT_VISIBILITY_INSTANCE))
                {
                    float fX, fY, fZ;
                    pTrap->GetPosition(fX, fY, fZ);

                    if (Creature* pTrigger = m_creature->SummonCreature(NPC_AHNQIRAJ_TRIGGER, fX, fY, fZ, 0, TEMPSUMMON_TIMED_DESPAWN, 2000))
                        pTrigger->CastSpell(pTrigger, SPELL_SAND_TRAP_EFFECT, false);

                    m_creature->SendObjectDeSpawnAnim(pTrap->GetObjectGuid());
                    pTrap->Delete();
                }
                m_uiSandTrapTimer = 5000;
                m_bIsSandTrap = false;
            }
        }
        else
            m_uiSandTrapTimer -= uiDiff;

        // Summon Player
        if (m_uiSummonPlayerTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                DoCastSpellIfCan(pTarget, SPELL_SUMMON_PLAYER);
            m_uiSummonPlayerTimer = urand(8000,10000);
        }
        else
            m_uiSummonPlayerTimer -= uiDiff;

        // Trash
        if (m_uiTrashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_TRASH);
            m_uiTrashTimer = urand(4000,6000);
        }
        else
            m_uiTrashTimer -= uiDiff;

        // Wide Slash
        if (m_uiWideSlashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WIDE_SLASH);
            m_uiWideSlashTimer = urand(5000,10000);
        }
        else
            m_uiWideSlashTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kurinnaxx(Creature* pCreature)
{
    return new boss_kurinnaxxAI(pCreature);
}

void AddSC_boss_kurinnaxx()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_kurinnaxx";
    pNewscript->GetAI = &GetAI_boss_kurinnaxx;
    pNewscript->RegisterSelf();
}
