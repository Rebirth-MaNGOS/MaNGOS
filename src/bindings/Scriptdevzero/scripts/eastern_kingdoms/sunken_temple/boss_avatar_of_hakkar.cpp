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
SDName: Boss Avatar Of Hakkar
SD%Complete: 90
SDComment: Avatar, Nightmare Suppressor, Eternal Flames
SDCategory: Sunken Temple
EndScriptData */

#include "precompiled.h"
#include "sunken_temple.h"

enum Spells
{
    // Avatar of Hakkar
    SPELL_CAUSE_INSANITY                = 12888,
    SPELL_CURSE_OF_TONGUES              = 12889,
    SPELL_LASH                          = 6607,

    // Suppressor
    SPELL_SUPPRESSION                   = 12623,

    EVENT_SPELL_AWAKEN_THE_SOULFLAYER   = 8502,
	YELL_SUPPRESSOR_1					= -1720200,
	YELL_SUPPRESSOR_2					= -1720201,
	YELL_AVATAR_SPAWN					= -1720198,
	YELL_AVATAR_AGGRO					= -1720199 
};

struct MANGOS_DLL_DECL boss_avatar_of_hakkarAI : public ScriptedAI
{
    boss_avatar_of_hakkarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_sunken_temple*)pCreature->GetInstanceData();
		DoScriptText(YELL_AVATAR_SPAWN, m_creature, NULL);
        Reset();
    }

    instance_sunken_temple* m_pInstance;

    uint32 m_uiCauseInsanityTimer;
    uint32 m_uiCurseOfTonguesTimer;
    uint32 m_uiLashTimer;

    void Reset()
    {
        m_uiCauseInsanityTimer = urand(3000,4000);
        m_uiCurseOfTonguesTimer = urand(4000,6000);
        m_uiLashTimer = urand(6000,8000);
    }

    void JustReachedHome()
    {
        m_creature->RemoveFromWorld();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_AVATAR_OF_HAKKAR, FAIL);
    }

    void Aggro(Unit* /*pWho*/)
    {
		DoScriptText(YELL_AVATAR_AGGRO, m_creature, NULL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AVATAR_OF_HAKKAR, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cause Insanity
        if (m_uiCauseInsanityTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_CAUSE_INSANITY);
            m_uiCauseInsanityTimer = urand(10000,15000);
        }
        else
            m_uiCauseInsanityTimer -= uiDiff;

        // Curse Of Tongues
        if (m_uiCurseOfTonguesTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_CURSE_OF_TONGUES);
            m_uiCurseOfTonguesTimer = urand(8000,9000);
        }
        else
            m_uiCurseOfTonguesTimer -= uiDiff;

        // Lash
        if (m_uiLashTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_LASH);
            m_uiLashTimer = urand(5000,7000);
        }
        else
            m_uiLashTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_avatar_of_hakkar(Creature* pCreature)
{
    return new boss_avatar_of_hakkarAI(pCreature);
}

struct MANGOS_DLL_DECL mob_nightmare_suppressorAI : public ScriptedAI
{
    mob_nightmare_suppressorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_sunken_temple*)pCreature->GetInstanceData();
		randomYell = urand(0,1);
		if(randomYell == 0)
			DoScriptText(YELL_SUPPRESSOR_1, m_creature, NULL);
		else
			DoScriptText(YELL_SUPPRESSOR_2, m_creature, NULL);
        Reset();
    }

    instance_sunken_temple* m_pInstance;

    bool m_bIsCasting;
    uint32 m_uiSuppressionTimer;
	int randomYell;

    void Reset()
    {
        m_bIsCasting = false;
        m_uiSuppressionTimer = 0;
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_AVATAR_OF_HAKKAR) == IN_PROGRESS)
            m_pInstance->HandleAvatarEventWave();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_pInstance && m_pInstance->GetData(TYPE_AVATAR_OF_HAKKAR) == IN_PROGRESS)
            {
                if (!m_bIsCasting && m_creature->IsStopped())
                {
					if (Creature* pTarget = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADE_OF_HAKKAR))
                        DoCastSpellIfCan(pTarget, SPELL_SUPPRESSION);
                    m_uiSuppressionTimer = 7000;
                    m_bIsCasting = true;
                }

                if (m_bIsCasting && m_uiSuppressionTimer < uiDiff)
                {
                    m_pInstance->SetData(TYPE_AVATAR_OF_HAKKAR, FAIL);
                }
                else
                    m_uiSuppressionTimer -= uiDiff;
            }
            return;
        }
        else
        {
            m_bIsCasting = false;
            m_creature->InterruptNonMeleeSpells(false, 12623);
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_nightmare_suppressor(Creature* pCreature)
{
    return new mob_nightmare_suppressorAI(pCreature);
}

bool GOUse_go_eternal_flame(Player* pPlayer, GameObject* pGo)
{
    instance_sunken_temple* m_pInstance = (instance_sunken_temple*)pGo->GetInstanceData();

    if (m_pInstance && m_pInstance->GetData(TYPE_AVATAR_OF_HAKKAR) == IN_PROGRESS &&
        pPlayer->HasItemCount(ITEM_HAKKARI_BLOOD, 1))
	{
        m_pInstance->HandleAvatarEventWave();
		m_pInstance->InteractWithGo(pGo->GetGUID(),false);
	}
    return false;
}

bool ProcessEventId_event_spell_awaken_the_soulflayer(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_PLAYER && uiEventId == EVENT_SPELL_AWAKEN_THE_SOULFLAYER)
    {
        instance_sunken_temple* m_pInstance = (instance_sunken_temple*)((Player*)pSource)->GetInstanceData();

        if (m_pInstance && m_pInstance->GetData(TYPE_AVATAR_OF_HAKKAR) == NOT_STARTED && !m_pInstance->GetWaveCount())
        {
            m_pInstance->HandleAvatarEventWave();
            ((Player*)pSource)->SummonCreature(NPC_SHADE_OF_HAKKAR, -467.56f, 273.42f, -90.45f, 6.23f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        }
    }
    return true;
}

void AddSC_boss_avatar_of_hakkar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_avatar_of_hakkar";
    pNewScript->GetAI = &GetAI_boss_avatar_of_hakkar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_nightmare_suppressor";
    pNewScript->GetAI = &GetAI_mob_nightmare_suppressor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_eternal_flame";
    pNewScript->pGOUse = &GOUse_go_eternal_flame;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_awaken_the_soulflayer";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_awaken_the_soulflayer;
    pNewScript->RegisterSelf();
}
