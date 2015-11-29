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
SDName: Boss_Renataki
SD%Complete: 100
SDComment: Vanish causes combat stop - core issue
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"
//#include "spellMgr.h"

enum eRenataki
{
    SPELL_AMBUSH            = 24337,
    SPELL_FRENZY            = 8269,     // probably bad spell id
    SPELL_GOUGE             = 24698,
    SPELL_THOUSAND_BLADES   = 24649,    // like Throw Dagger
    //SPELL_TRASH             = 3391,
    //SPELL_VANISH            = 24699,

    MODELID_NO_MODEL        = 11686,
};

#define EQUIP_ID_MAIN_HAND      0                           //was item display id 31818, but this id does not exist

struct MANGOS_DLL_DECL boss_renatakiAI : public ScriptedAI
{
    boss_renatakiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiDefaultDisplayId = m_creature->GetDisplayId();
		m_bSpawnLightning = false;
        Reset();
    }

    bool m_bAmbushed;
    bool m_bInvisible;
    bool m_bEnraged;
	bool m_bSpawnLightning;

    ObjectGuid m_uiAmbushTargetGUID;

    uint32 m_uiDefaultDisplayId;

    uint32 m_uiInvisibleTimer;
    uint32 m_uiAmbushTimer;
    uint32 m_uiVisibleTimer;
    uint32 m_uiAggroTimer;
    uint32 m_uiThousandBladesTimer;
    uint32 m_uiGougeTimer;

    void Reset()
    {
        m_bAmbushed = false;
        m_bInvisible = false;
        m_bEnraged = false;

        m_uiAmbushTargetGUID.Clear();

        m_uiInvisibleTimer = urand(8000, 18000);
        m_uiAmbushTimer = 3000;
        m_uiVisibleTimer = 4000;
        m_uiAggroTimer = urand(15000, 25000);
        m_uiThousandBladesTimer = urand(4000, 8000);
        m_uiGougeTimer = urand(45000, 60000);
    }

    void RenatakiVisibility(bool m_bVisible)
    {
        m_creature->InterruptSpell(CURRENT_GENERIC_SPELL);

        if (m_bVisible)
        {
            /*m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 0);
            m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, 218171138);
            m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + 1, 3);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetDisplayId(MODELID_NO_MODEL);*/
            m_creature->SetVisibility(VISIBILITY_ON);
        }
        else
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
            /*m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 31818);
            m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, 218171138);
            m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + 1, 3);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetDisplayId(m_uiDefaultDisplayId);     // 15268*/
        }

        m_bInvisible = m_bVisible ? false : true;
    }

    /*void SpellHit(Unit* pUnit, const SpellEntry* pSpell)
    {
        if (IsAreaAuraEffect(pSpell->Effect[EFFECT_INDEX_0]))
        {
            //m_creature->MonsterSay("%t ma zasiahol AOE efektom!", LANG_UNIVERSAL, pUnit);
            //error_log("%s zasah AOE efektom!", pUnit->GetName());
            if (!m_bInvisible)
                RenatakiVisibility(true);
        }
    }*/

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_bInvisible)
            RenatakiVisibility(true);
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32& /*uiDamage*/)
    {
        if (m_bInvisible)
            RenatakiVisibility(true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_bSpawnLightning)
		{
			m_creature->CastSpell(m_creature, SPELL_RED_LIGHTNING, true);
			m_bSpawnLightning = true;
		}
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Frenzy
        if (!m_bEnraged && HealthBelowPct(50))
        {
            m_bEnraged = true;
            DoCastSpellIfCan(m_creature, SPELL_FRENZY);
        }

        // Invisible
        if (m_uiInvisibleTimer <= uiDiff)
        {
            if (Unit* pAmbushTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                m_uiAmbushTargetGUID = pAmbushTarget->GetObjectGuid();
                m_creature->GetMotionMaster()->MoveFollow(pAmbushTarget, ATTACK_DISTANCE, 0.0f);
            }

            // Cancel spell casts of attackers
            /*std::vector<ObjectGuid> vGuids;
            m_creature->FillGuidsListFromThreatList(vGuids);
            if (!vGuids.empty())
            {
                for (std::vector<ObjectGuid>::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
                {
                    if (Unit* pTarget = m_creature->GetMap()->GetUnit(*itr))
                    {
                        if (pTarget->getVictim()->GetObjectGuid() == m_creature->GetObjectGuid())
                            if (pTarget->IsNonMeleeSpellCasted(false, true))
                                pTarget->InterruptNonMeleeSpells(false);
                    }
                }
            }*/

            RenatakiVisibility(false);

            m_uiInvisibleTimer = urand(15000, 30000);
        }
        else
            m_uiInvisibleTimer -= uiDiff;

        if (m_bInvisible)
        {
            if (!m_bAmbushed)
            {
                // Ambush
                if (m_uiAmbushTimer <= uiDiff)
                {
                    Unit* pTarget = NULL;

                    if (m_uiAmbushTargetGUID)
                        pTarget = m_creature->GetMap()->GetUnit(m_uiAmbushTargetGUID);

                    if (!pTarget)
                        pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);

                    if (pTarget)
                    {
                        m_creature->NearTeleportTo(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0.0f);
                        DoCastSpellIfCan(pTarget, SPELL_AMBUSH);
                    }

                    m_bAmbushed = true;
                    m_uiAmbushTimer = 3000;
                }
                else
                    m_uiAmbushTimer -= uiDiff;
            }
            else
            {
                // Visible
                if (m_uiVisibleTimer <= uiDiff)
                {
                    RenatakiVisibility(true);
                    m_uiVisibleTimer = 4000;
                }
                else
                    m_uiVisibleTimer -= uiDiff;
            }
        }
        else
        {
            // Gouge
            if (m_uiGougeTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_GOUGE);
                m_uiGougeTimer = urand(45000, 60000);
            }
            else
                m_uiGougeTimer -= uiDiff;

            // Resetting some aggro so he attacks other gamers
            if (m_uiAggroTimer <= uiDiff)
            {
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
                AttackStart(pTarget ? pTarget : m_creature->getVictim());

                if (m_creature->getThreatManager().getThreat(m_creature->getVictim()))
                    m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), -50);

                m_uiAggroTimer = urand(7000, 20000);
            }
            else
                m_uiAggroTimer -= uiDiff;

            // Thousand Blades
            if (m_uiThousandBladesTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_THOUSAND_BLADES);
                m_uiThousandBladesTimer = urand(7000, 12000);
            }
            else
                m_uiThousandBladesTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }
};
CreatureAI* GetAI_boss_renataki(Creature* pCreature)
{
    return new boss_renatakiAI(pCreature);
}

void AddSC_boss_renataki()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_renataki";
    pNewScript->GetAI = &GetAI_boss_renataki;
    pNewScript->RegisterSelf();
}
