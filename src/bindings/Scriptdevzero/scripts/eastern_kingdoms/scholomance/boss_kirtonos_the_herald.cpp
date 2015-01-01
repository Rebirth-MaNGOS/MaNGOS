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
SDName: Boss Kirtonos The Herald
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_CURSE_OF_TONGUES      = 12889,
    SPELL_DISARM                = 8379,
    SPELL_DOMINATE_MIND         = 14515,
    SPELL_KIRTONOS_TRANSFORM    = 16467,
    SPELL_PIERCE_ARMOR          = 6016,
    SPELL_SHADOW_BOLT_VOLLEY    = 17228,
    SPELL_SWOOP                 = 18144,
    SPELL_WING_FLAP             = 12882
};

struct MANGOS_DLL_DECL boss_kirtonos_the_heraldAI : public ScriptedAI
{
    boss_kirtonos_the_heraldAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

    uint32 m_uiCurseOfTonguesTimer;
    uint32 m_uiDisarmTimer;
    uint32 m_uiDominateMindTimer;
    uint32 m_uiPierceArmorTimer;
    uint32 m_uiShadowBoltVolleyTimer;
    uint32 m_uiSwoopTimer;
    uint32 m_uiWingFlapTimer;

    void Reset()
    {
        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->setFaction(FACTION_FRIENDLY);

        m_uiCurseOfTonguesTimer = urand(7000,9000);
        m_uiDisarmTimer = urand(10000,12000);
        m_uiDominateMindTimer = 30000;
        m_uiPierceArmorTimer = 20000;
        m_uiShadowBoltVolleyTimer = 25000;
        m_uiSwoopTimer = urand(4000,6000);
        m_uiWingFlapTimer = urand(13000,15000);
    }

    void JustReachedHome()
    {
        m_creature->RemoveFromWorld();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KIRTONOS_THE_HERALD, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KIRTONOS_THE_HERALD, DONE);
    }

    void MovementInform(uint32 /*uiMotionType*/, uint32 uiPointId)
    {
        if (uiPointId == 0)
        {
            DoCastSpellIfCan(m_creature, SPELL_KIRTONOS_TRANSFORM);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
            m_creature->setFaction(FACTION_HOSTILE);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Curse of Tongues
        if (m_uiCurseOfTonguesTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_CURSE_OF_TONGUES);
            m_uiCurseOfTonguesTimer = urand(8000,10000);
        }
        else
            m_uiCurseOfTonguesTimer -= uiDiff;

        // Disarm
        if (m_uiDisarmTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DISARM);
            m_uiDisarmTimer = urand(6000,8000);
        }
        else
            m_uiDisarmTimer -= uiDiff;

        // Dominate Mind
        if (m_uiDominateMindTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DOMINATE_MIND);
            m_uiDominateMindTimer = 40000;
        }
        else
            m_uiDominateMindTimer -= uiDiff;

        // Pierce Armor
        if (m_uiPierceArmorTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_PIERCE_ARMOR);
            m_uiPierceArmorTimer = 46000;
        }
        else
            m_uiPierceArmorTimer -= uiDiff;

        // Shadow Bolt Volley
        if (m_uiShadowBoltVolleyTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT_VOLLEY);
            m_uiShadowBoltVolleyTimer = urand(8000,12000);
        }
        else
            m_uiShadowBoltVolleyTimer -= uiDiff;

        // Swoop
        if (m_uiSwoopTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SWOOP);
            m_uiSwoopTimer = urand(5000,9000);
        }
        else
            m_uiSwoopTimer -= uiDiff;

        // Wing Flap
        if (m_uiWingFlapTimer < uiDiff)
        {
            if (Unit* pPrevTarget = m_creature->getVictim())
            {
                DoCastSpellIfCan(pPrevTarget, SPELL_WING_FLAP);
                if (m_creature->getThreatManager().getThreat(pPrevTarget))
                    m_creature->getThreatManager().modifyThreatPercent(pPrevTarget, -40);
            }
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                m_creature->Attack(pTarget, true);
            m_uiWingFlapTimer = urand(8000,12000);
        }
        else
            m_uiWingFlapTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kirtonos_the_herald(Creature* pCreature)
{
    return new boss_kirtonos_the_heraldAI(pCreature);
}

bool GOUse_go_brazier_of_the_herald(Player* pPlayer, GameObject* pGo)
{
    instance_scholomance* m_pInstance = (instance_scholomance*)pGo->GetInstanceData();

    if (m_pInstance && m_pInstance->GetData(TYPE_KIRTONOS_THE_HERALD) == NOT_STARTED && pPlayer->HasItemCount(ITEM_BLOOD_OF_INNOCENTS, 1))
    {
        pGo->SetGoState(GO_STATE_ACTIVE);
        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND + GO_FLAG_NO_INTERACT);
        if (Creature* Kirtonos = pGo->SummonCreature(NPC_KIRTONOS_THE_HERALD, 360.71f, 91.301f, 110.63f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 60000))
        {
            float pX, pY, pZ;
            pGo->GetPosition(pX, pY, pZ);
            Kirtonos->GetMotionMaster()->MovePoint(0, pX-6, pY, pZ,false);
            m_pInstance->SetData(TYPE_KIRTONOS_THE_HERALD, IN_PROGRESS);
        }
    }
    return true;
}

void AddSC_boss_kirtonos_the_herald()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_kirtonos_the_herald";
    pNewscript->GetAI = &GetAI_boss_kirtonos_the_herald;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_brazier_of_the_herald";
    pNewscript->pGOUse = &GOUse_go_brazier_of_the_herald;
    pNewscript->RegisterSelf();
}
