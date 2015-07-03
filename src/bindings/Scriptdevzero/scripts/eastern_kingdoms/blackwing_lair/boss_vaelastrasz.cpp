/*
 * Copyright (C) 2006-2011 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2010-2011 ScriptDev0 <http://github.com/mangos-zero/scriptdev0>
 *
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
SDName: Boss_Vaelastrasz
SD%Complete: 90
SDComment: Needs a playtest.
SDCategory: Blackwing Lair
EndScriptData */

#include "precompiled.h"
#include "blackwing_lair.h"

enum
{
    SAY_LINE_1                  = -1469026,
    SAY_LINE_2                  = -1469027,
    SAY_LINE_3                  = -1469028,
    SAY_HALFLIFE                = -1469029,
    SAY_KILLTARGET              = -1469030,
    SAY_NEFARIUS_CORRUPT_1      = -1469006,                 // When he corrupts Vaelastrasz
    SAY_NEFARIUS_CORRUPT_2      = -1469031,
    SAY_TECHNICIAN_RUN          = -1469034,

    SPELL_ESSENCE_OF_THE_RED    = 23513,
    SPELL_FLAME_BREATH          = 23461,
    SPELL_FIRE_NOVA             = 23462,
    SPELL_TAIL_SWEEP            = 15847,
    SPELL_BURNING_ADRENALINE    = 23620,
    SPELL_CLEAVE                = 19983,					// The cleave listed at wowhead as the spell Vaelastrasz is using. Can affect up to 10 extra targets.

    SPELL_NEFARIUS_CORRUPTION   = 23642,

    GOSSIP_ITEM_VAEL_1          = -3469003,
    GOSSIP_ITEM_VAEL_2          = -3469004,
    // Vael Gossip texts might be 7156 and 7256; At the moment are missing from DB
    // For the moment add the default values
    GOSSIP_TEXT_VAEL_1          = 7156,
    GOSSIP_TEXT_VAEL_2          = 7256,

    FACTION_HOSTILE             = 14,

    AREATRIGGER_VAEL_INTRO      = 3626,

    MOB_BLACKWING_TECHNICIAN	= 13996,

    FACTION_ENEMY				= 54
};

struct MANGOS_DLL_DECL boss_vaelastraszAI : public ScriptedAI
{
    boss_vaelastraszAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackwing_lair*)pCreature->GetInstanceData();

        m_bIsCorrupted = false;
        m_bIsBeingCorrupted = false;
        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_FIRE, true);

        Reset();

        // Set stand state to dead before the intro event
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
    }

    instance_blackwing_lair* m_pInstance;

    ObjectGuid m_nefariusGuid;
    uint32 m_uiIntroTimer;
    uint8 m_uiIntroPhase;

    ObjectGuid m_playerGuid;
    uint32 m_uiSpeechTimer;
    uint8 m_uiSpeechNum;

    uint32 m_uiCleaveTimer;
    uint32 m_uiFlameBreathTimer;
    uint32 m_uiFireNovaTimer;
    uint32 m_uiBurningAdrenalineCasterTimer;
    uint32 m_uiBurningAdrenalineTankTimer;
    uint32 m_uiTailSweepTimer;
    bool m_bHasYelled;
    bool m_bIsCorrupted, m_bIsBeingCorrupted;

    void Reset()
    {
        m_playerGuid.Clear();

        m_uiIntroTimer                   = 0;
        m_uiIntroPhase                   = 0;
        m_uiSpeechTimer                  = 0;
        m_uiSpeechNum                    = 0;
        m_uiCleaveTimer                  = 8000;            // These times are probably wrong
        m_uiFlameBreathTimer             = 11000;
        m_uiBurningAdrenalineCasterTimer = 15000;
        m_uiBurningAdrenalineTankTimer   = 45000;
        m_uiFireNovaTimer                = 5000;
        m_uiTailSweepTimer               = 20000;
        m_bHasYelled = false;

        // Remove flags so you cannot talk to vael.
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

        // Creature should have only 1/3 of hp
        if (m_bIsCorrupted)
            m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.3)); // Nefarian has to corrupt him.
        else
            m_creature->SetHealth(m_creature->GetMaxHealth());
    }

    void BeginIntro()
    {
        // Start Intro delayed
        m_uiIntroTimer = 1000;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAELASTRASZ, SPECIAL);
    }

    void BeginSpeech(Player* pTarget)
    {
        // Remove the flags so you can no longer talk to Vael.
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

        // Stand up and begin speach
        m_playerGuid = pTarget->GetObjectGuid();

        // 10 seconds
        DoScriptText(SAY_LINE_1, m_creature);

        // Make boss stand
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);

        m_uiSpeechTimer = 10000;
        m_uiSpeechNum = 0;
    }

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 4))
            return;

        DoScriptText(SAY_KILLTARGET, m_creature, pVictim);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAELASTRASZ, IN_PROGRESS);

        // Buff players on aggro
        DoCastSpellIfCan(m_creature, SPELL_ESSENCE_OF_THE_RED);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAELASTRASZ, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAELASTRASZ, FAIL);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_LORD_NEFARIAN)
        {
            // Set not selectable, so players won't interact with it
            pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_nefariusGuid = pSummoned->GetObjectGuid();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiIntroTimer)
        {
            if (m_uiIntroTimer <= uiDiff)
            {
                switch (m_uiIntroPhase)
                {
                case 0:
                    if (m_pInstance)
                        DoScriptText(SAY_TECHNICIAN_RUN, m_pInstance->GetGoblinGroup1()->front());

                    m_creature->SummonCreature(NPC_LORD_NEFARIAN, aNefariusSpawnLoc[0], aNefariusSpawnLoc[1], aNefariusSpawnLoc[2], aNefariusSpawnLoc[3], TEMPSUMMON_TIMED_DESPAWN, 25000);
                    m_uiIntroTimer = 1000;
                    break;
                case 1:
                    if (Creature* pNefarius = m_creature->GetMap()->GetCreature(m_nefariusGuid))
                    {
                        m_creature->SetPassiveToSpells(true);

                        pNefarius->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                        pNefarius->GetMotionMaster()->Clear();
                        pNefarius->GetMotionMaster()->MoveIdle();
                        pNefarius->SetFacingToObject(m_creature);

                        pNefarius->CastSpell(m_creature, SPELL_NEFARIUS_CORRUPTION, false);


                        m_bIsBeingCorrupted = true;
                        DoScriptText(SAY_NEFARIUS_CORRUPT_1, pNefarius);
                    }
                    m_uiIntroTimer = 16000;
                    break;
                case 2:
                    if (Creature* pNefarius = m_creature->GetMap()->GetCreature(m_nefariusGuid))
                    {
                        DoScriptText(SAY_NEFARIUS_CORRUPT_2, pNefarius);
                        pNefarius->InterruptSpell(CURRENT_CHANNELED_SPELL, false);
                        pNefarius->RemoveAllAttackers();
                        pNefarius->getThreatManager().clearReferences();
                        pNefarius->CombatStop(true);
                    }

                    m_creature->SetPassiveToSpells(false);

                    m_bIsBeingCorrupted = false;
                    m_bIsCorrupted = true;

                    m_creature->SetHealth((uint32)(m_creature->GetMaxHealth() * .3f));

                    // Set npc flags now
                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    m_uiIntroTimer = 0;
                    break;
                }
                ++m_uiIntroPhase;
            }
            else
                m_uiIntroTimer -= uiDiff;

            // Linearly remove Vaelastrasz health while he's being corrupted.
            if (m_bIsBeingCorrupted)
                m_creature->SetHealth((uint32)(m_creature->GetMaxHealth() * (0.00004375f * m_uiIntroTimer + 0.3f)));
        }

        // Speech
        if (m_uiSpeechTimer)
        {
            if (m_uiSpeechTimer <= uiDiff)
            {
                switch (m_uiSpeechNum)
                {
                case 0:
                    // 16 seconds till next line
                    DoScriptText(SAY_LINE_2, m_creature);
                    m_uiSpeechTimer = 16000;
                    ++m_uiSpeechNum;
                    break;
                case 1:
                    // This one is actually 16 seconds but we only go to 10 seconds because he starts attacking after he says "I must fight this!"
                    DoScriptText(SAY_LINE_3, m_creature);
                    m_uiSpeechTimer = 10000;
                    ++m_uiSpeechNum;
                    break;
                case 2:
                    m_creature->setFaction(FACTION_HOSTILE);

                    if (m_playerGuid)
                    {
                        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                            AttackStart(pPlayer);
                    }
                    m_uiSpeechTimer = 0;
                    break;
                }
            }
            else
                m_uiSpeechTimer -= uiDiff;
        }

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Yell if hp lower than 15%
        if (m_creature->GetHealthPercent() < 15.0f && !m_bHasYelled)
        {
            DoScriptText(SAY_HALFLIFE, m_creature);
            m_bHasYelled = true;
        }

        // Cleave Timer
        if (m_uiCleaveTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = 15000;
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Flame Breath Timer
        if (m_uiFlameBreathTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FLAME_BREATH) == CAST_OK)
                m_uiFlameBreathTimer = urand(4000, 8000);
        }
        else
            m_uiFlameBreathTimer -= uiDiff;

        // Burning Adrenaline Caster Timer
        if (m_uiBurningAdrenalineCasterTimer < uiDiff)
        {
            std::vector<Unit*> vManaPlayers;

            // Scan for mana targets in threat list
            ThreatList const& tList = m_creature->getThreatManager().getThreatList();
            vManaPlayers.reserve(tList.size());
            for (ThreatList::const_iterator iter = tList.begin(); iter != tList.end(); ++iter)
            {
                Unit* pTempTarget = m_creature->GetMap()->GetUnit((*iter)->getUnitGuid());

                if (pTempTarget && pTempTarget->getPowerType() == POWER_MANA && pTempTarget->GetTypeId() == TYPEID_PLAYER)
                    vManaPlayers.push_back(pTempTarget);
            }

            if (!vManaPlayers.empty())
            {
                Unit* pTarget = vManaPlayers[urand(0, vManaPlayers.size() - 1)];
                pTarget->CastSpell(pTarget, SPELL_BURNING_ADRENALINE, true, NULL, NULL, m_creature->GetObjectGuid());

                m_uiBurningAdrenalineCasterTimer = 15000;
            }
        }
        else
            m_uiBurningAdrenalineCasterTimer -= uiDiff;

        // Burning Adrenaline Tank Timer
        if (m_uiBurningAdrenalineTankTimer < uiDiff)
        {
            // have the victim cast the spell on himself otherwise the third effect aura will be applied
            // to Vael instead of the player
            m_creature->getVictim()->CastSpell(m_creature->getVictim(), SPELL_BURNING_ADRENALINE, true, NULL, NULL, m_creature->GetObjectGuid());

            m_uiBurningAdrenalineTankTimer = 45000;
        }
        else
            m_uiBurningAdrenalineTankTimer -= uiDiff;

        // Fire Nova Timer
        if (m_uiFireNovaTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FIRE_NOVA) == CAST_OK)
                m_uiFireNovaTimer = urand(1500, 3000);
        }
        else
            m_uiFireNovaTimer -= uiDiff;

        // Tail Sweep Timer
        if (m_uiTailSweepTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TAIL_SWEEP) == CAST_OK)
                m_uiTailSweepTimer = 20000;
        }
        else
            m_uiTailSweepTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool GossipSelect_boss_vaelastrasz(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_VAEL_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_VAEL_2, pCreature->GetObjectGuid());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        pPlayer->CLOSE_GOSSIP_MENU();
        if (boss_vaelastraszAI* pVaelAI = dynamic_cast<boss_vaelastraszAI*>(pCreature->AI()))
            pVaelAI->BeginSpeech(pPlayer);
        break;
    }

    return true;
}

bool GossipHello_boss_vaelastrasz(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_VAEL_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_VAEL_1, pCreature->GetObjectGuid());

    return true;
}

CreatureAI* GetAI_boss_vaelastrasz(Creature* pCreature)
{
    return new boss_vaelastraszAI(pCreature);
}

bool AreaTrigger_at_vaelastrasz(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    const float goblin_spawn_points[6][3] = {{ -7501.f, -1026.f, 409.f }, { -7496.f, -1023.f, 409.f }, { -7499.f, -1020.f, 409.f },
                                             { -7479.f, -1010.f, 409.f }, { -7473.f, -1007.f, 409.f }, { -7477.f, -1006.f, 409.f }};

    const float stairs1[3] = { -7487.f, -1071.f, 409.f };
    const float stairs2[3] = { -7431.f, -1032.f, 409.f };

    if (pAt->id == AREATRIGGER_VAEL_INTRO)
    {
        if (pPlayer->isGameMaster() || pPlayer->isDead())
            return false;


        if (instance_blackwing_lair* pInstance = (instance_blackwing_lair*)pPlayer->GetInstanceData())
        {
            // Handle intro event
            if (pInstance->GetData(TYPE_VAELASTRASZ) == NOT_STARTED)
            {
                if (Creature* pVaelastrasz = pInstance->GetSingleCreatureFromStorage(NPC_VAELASTRASZ))
                {
                    for (int i = 0; i < 6; i++)
                    {
                        Creature* summoned_goblin = pVaelastrasz->SummonCreature(MOB_BLACKWING_TECHNICIAN, goblin_spawn_points[i][0], goblin_spawn_points[i][1], goblin_spawn_points[i][2], 0.f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                        if (summoned_goblin)
                        {
                            summoned_goblin->setFaction(103);
                            summoned_goblin->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            summoned_goblin->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

                            if (i < 3)
                            {
                                summoned_goblin->GetMotionMaster()->MovePoint(0, stairs1[0], stairs1[1], stairs1[2]);
                                pInstance->GetGoblinGroup1()->push_back(summoned_goblin);
                            }
                            else
                            {
                                summoned_goblin->GetMotionMaster()->MovePoint(1, stairs2[0], stairs2[1], stairs2[2]);
                                pInstance->GetGoblinGroup2()->push_back(summoned_goblin);
                            }

                        }

                    }

                    if (boss_vaelastraszAI* pVaelAI = dynamic_cast<boss_vaelastraszAI*>(pVaelastrasz->AI()))
                        pVaelAI->BeginIntro();


                }
            }

        }
    }

    return false;
}

bool QuestAccept_boss_vaelastrasz(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pPlayer && pCreature && pQuest && pQuest->GetQuestId() == 8730)
    {
        instance_blackwing_lair *bwlInstance = dynamic_cast<instance_blackwing_lair*>(pCreature->GetInstanceData());

        if(bwlInstance)
        {
            bwlInstance->m_uiNefariusScepterSpeechTimer = urand(3600000, 5400000);
            Creature *pNef = bwlInstance->GetSingleCreatureFromStorage(10162);

            if(pNef)
            {
                pNef->MonsterYellToZone(-1720177, LANG_UNIVERSAL, pPlayer);
            }
        }
    }
    return true;
}


void AddSC_boss_vaelastrasz()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_vaelastrasz";
    pNewScript->GetAI = &GetAI_boss_vaelastrasz;
    pNewScript->pGossipHello = &GossipHello_boss_vaelastrasz;
    pNewScript->pGossipSelect = &GossipSelect_boss_vaelastrasz;
    pNewScript->pQuestAcceptNPC = &QuestAccept_boss_vaelastrasz;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_vaelastrasz";
    pNewScript->pAreaTrigger = &AreaTrigger_at_vaelastrasz;
    pNewScript->RegisterSelf();
}
