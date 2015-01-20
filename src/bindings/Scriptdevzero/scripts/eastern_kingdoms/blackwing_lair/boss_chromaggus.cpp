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
SDName: Boss_Chromaggus
SD%Complete: 95
SDComment: Chromatic Mutation disabled due to lack of core support
SDCategory: Blackwing Lair
EndScriptData */

#include "precompiled.h"
#include "blackwing_lair.h"

enum
{
    EMOTE_GENERIC_FRENZY_KILL   = -1000001,
    EMOTE_SHIMMER               = -1469003,

    // These spells are actually called elemental shield
    // What they do is decrease all damage by 75% then they increase
    // One school of damage by 1100%
    SPELL_FIRE_VULNERABILITY    = 22277,
    SPELL_FROST_VULNERABILITY   = 22278,
    SPELL_SHADOW_VULNERABILITY  = 22279,
    SPELL_NATURE_VULNERABILITY  = 22280,
    SPELL_ARCANE_VULNERABILITY  = 22281,

    MAX_BREATHS                 = 5,
    SPELL_INCINERATE            = 23308,                    // Incinerate 23308,23309
    SPELL_TIME_LAPSE            = 23310,                    // Time lapse 23310, 23311(old threat mod that was removed in 2.01)
    SPELL_CORROSIVE_ACID        = 23313,                    // Corrosive Acid 23313, 23314
    SPELL_IGNITE_FLESH          = 23315,                    // Ignite Flesh 23315,23316
    SPELL_FROST_BURN            = 23187,                    // Frost burn 23187, 23189

    // Brood Affliction 23173 - Scripted Spell that cycles through all targets within 100 yards and has a chance to cast one of the afflictions on them
    // Since Scripted spells arn't coded I'll just write a function that does the same thing
    SPELL_BROODAF_BLUE          = 23153,                    // Blue affliction 23153
    SPELL_BROODAF_BLACK         = 23154,                    // Black affliction 23154
    SPELL_BROODAF_RED           = 23155,                    // Red affliction 23155 (23168 on death)
    SPELL_BROODAF_BRONZE        = 23170,                    // Bronze Affliction  23170
    SPELL_BROODAF_GREEN         = 23169,                    // Brood Affliction Green 23169

    SPELL_CHROMATIC_MUT_1       = 23174,                    // Spell cast on player if they get all 5 debuffs

    SPELL_FRENZY                = 28371,                    // The frenzy spell may be wrong
    SPELL_ENRAGE                = 28747
};

static const uint32 aPossibleBreaths[MAX_BREATHS] = {SPELL_INCINERATE, SPELL_TIME_LAPSE, SPELL_CORROSIVE_ACID, SPELL_IGNITE_FLESH, SPELL_FROST_BURN};

struct MANGOS_DLL_DECL boss_chromaggusAI : public ScriptedAI
{
    boss_chromaggusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        // Select the 2 different breaths that we are going to use until despawned
        // 5 possiblities for the first breath, 4 for the second, 20 total possiblites

        // select two different numbers between 0..MAX_BREATHS-1
        uint8 uiPos1 = urand(0, MAX_BREATHS - 1);
        uint8 uiPos2 = (uiPos1 + urand(1, MAX_BREATHS - 1)) % MAX_BREATHS;

        m_uiBreathOneSpell = aPossibleBreaths[uiPos1];
        m_uiBreathTwoSpell = aPossibleBreaths[uiPos2];

        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

        m_uiCurrentVulnerabilitySpell = 0;                  // We use this to store our last vulnerability spell so we can remove it later

        m_uiShimmerTimer    = 0;                            // Time till we change vurlnerabilites
        m_uiBreathOneTimer  = 30000;                        // First breath is 30 seconds
        m_uiBreathTwoTimer  = 60000;                        // Second is 1 minute so that we can alternate
        m_uiAfflictionTimer = 10000;                        // This is special - 5 seconds means that we cast this on 1 pPlayer every 5 sconds
        m_uiMutationCheckTimer = 1000;
        m_uiFrenzyTimer     = 15000;

        m_bEnraged          = false;
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBreathOneSpell;
    uint32 m_uiBreathTwoSpell;
    uint32 m_uiCurrentVulnerabilitySpell;

    uint32 m_uiShimmerTimer;
    uint32 m_uiBreathOneTimer;
    uint32 m_uiBreathTwoTimer;
    uint32 m_uiAfflictionTimer;
    uint32 m_uiMutationCheckTimer;
    uint32 m_uiFrenzyTimer;
    bool m_bEnraged;

    void Reset()
    {
        m_uiCurrentVulnerabilitySpell = 0;                  // We use this to store our last vulnerability spell so we can remove it later

        m_uiShimmerTimer    = 0;                            // Time till we change vurlnerabilites
        m_uiBreathOneTimer  = 30000;                        // First breath is 30 seconds
        m_uiBreathTwoTimer  = 60000;                        // Second is 1 minute so that we can alternate
        m_uiAfflictionTimer = 10000;                        // This is special - 5 seconds means that we cast this on 1 pPlayer every 5 sconds
        m_uiMutationCheckTimer = 1000;
        m_uiFrenzyTimer     = 15000;

        m_bEnraged          = false;

        GameObject* pDoor = GetClosestGameObjectWithEntry(m_creature, 179116, 1000.f);
        if (pDoor && pDoor->GetGoState() == GO_STATE_ACTIVE)
            m_creature->GetMotionMaster()->MovePoint(0, -7488.f, -1073.f, 477.f); // Chromaggus should stand in the middle of the room after a wipe.
    }

    void MovementInform(uint32 /*pointID*/, uint32 /*movementType*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHROMAGGUS, FAIL);
        
        m_creature->SetFacingTo(0.5699f);
        m_creature->RelocateCreature(-7488.f, -1073.f, 477.f, 0.5699f);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHROMAGGUS, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHROMAGGUS, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHROMAGGUS, FAIL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Make Chromaggus skip targets affected by Time Lapse.
        if (m_creature->getVictim()->HasAura(23310))
        {
            const ThreatList& threatList = m_creature->getThreatManager().getThreatList();
            for (HostileReference* currentReference : threatList)
            {
                Unit* target = currentReference->getTarget();
                if (target && !target->HasAura(23310))
                {
                    m_creature->Attack(target, true);

                    if (!m_creature->getVictim())
                        return;

                    break;
                }

            }
        }

        // Shimmer Timer Timer
        if (m_uiShimmerTimer < uiDiff)
        {
            // Remove old vulnerability spell
            if (m_uiCurrentVulnerabilitySpell)
                m_creature->RemoveAurasDueToSpell(m_uiCurrentVulnerabilitySpell);

            // Cast new random vurlnabilty on self
            uint32 uiSpell;
            switch(urand(0, 4))
            {
            case 0:
                uiSpell = SPELL_FIRE_VULNERABILITY;
                break;
            case 1:
                uiSpell = SPELL_FROST_VULNERABILITY;
                break;
            case 2:
                uiSpell = SPELL_SHADOW_VULNERABILITY;
                break;
            case 3:
                uiSpell = SPELL_NATURE_VULNERABILITY;
                break;
            case 4:
                uiSpell = SPELL_ARCANE_VULNERABILITY;
                break;
            }

            m_creature->CastSpell(m_creature, uiSpell, false);

            m_uiCurrentVulnerabilitySpell = uiSpell;

            DoScriptText(EMOTE_SHIMMER, m_creature);
            m_uiShimmerTimer = 45000;
        }
        else
            m_uiShimmerTimer -= uiDiff;

        // Breath One Timer
        if (m_uiBreathOneTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature, m_uiBreathOneSpell, false);
            m_uiBreathOneTimer = 60000;
        }
        else
            m_uiBreathOneTimer -= uiDiff;

        // Breath Two Timer
        if (m_uiBreathTwoTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature, m_uiBreathTwoSpell, false);
            m_uiBreathTwoTimer = 60000;
        }
        else
            m_uiBreathTwoTimer -= uiDiff;

        // Affliction Timer
        if (m_uiAfflictionTimer < uiDiff)
        {
            uint32 m_uiSpellAfflict = 0;

            switch(urand(0, 4))
            {
            case 0:
                m_uiSpellAfflict = SPELL_BROODAF_BLUE;
                break;
            case 1:
                m_uiSpellAfflict = SPELL_BROODAF_BLACK;
                break;
            case 2:
                m_uiSpellAfflict = SPELL_BROODAF_RED;
                break;
            case 3:
                m_uiSpellAfflict = SPELL_BROODAF_BRONZE;
                break;
            case 4:
                m_uiSpellAfflict = SPELL_BROODAF_GREEN;
                break;
            }

            // Get all the players in the current map.
            std::vector<Player*> playerList;
            for (auto currentPlayerRef = m_creature->GetMap()->GetPlayers().begin(); currentPlayerRef != m_creature->GetMap()->GetPlayers().end(); ++currentPlayerRef)
            {
                Player* currentPlayer = currentPlayerRef->getSource();
                if (currentPlayer && currentPlayer->isAlive())
                    playerList.push_back(currentPlayer);
            }

            std::random_shuffle(playerList.begin(), playerList.end());

            // If we have more than 15 players we simply cast on the first 15 in the list.
            if (playerList.size() >= 15)
            {
                for(unsigned short i = 0; i < 15; i++)
                    m_creature->CastSpell(playerList[i], m_uiSpellAfflict, true);
            }
            else if (playerList.size() > 0)
            {
                // When there are less than 15 players we distribute the 15 casts between those players that are present.
                unsigned short numberOfCasts = 15 / playerList.size();
                unsigned short spill = 15 % playerList.size();

                for (unsigned short i = 0; i < playerList.size(); i++)
                {
                    for(unsigned short j = 0; j < numberOfCasts; j++)
                        m_creature->CastSpell(playerList[i], m_uiSpellAfflict, true);
                }

                for (unsigned short i = 0; i < spill; i++)
                    m_creature->CastSpell(playerList[i], m_uiSpellAfflict, true);
            }


            m_uiAfflictionTimer = 10000;
        }
        else
            m_uiAfflictionTimer -= uiDiff;

        if (m_uiMutationCheckTimer <= uiDiff)
        {
            std::vector<ObjectGuid> vGuids;
            m_creature->FillGuidsListFromThreatList(vGuids);
            for (std::vector<ObjectGuid>::const_iterator i = vGuids.begin(); i != vGuids.end(); ++i)
            {
                Unit* pUnit = m_creature->GetMap()->GetUnit(*i);

                // Chromatic mutation if target is effected by all afflictions
                if (pUnit->HasAura(SPELL_BROODAF_BLUE, EFFECT_INDEX_0)
                        && pUnit->HasAura(SPELL_BROODAF_BLACK, EFFECT_INDEX_0)
                        && pUnit->HasAura(SPELL_BROODAF_RED, EFFECT_INDEX_0)
                        && pUnit->HasAura(SPELL_BROODAF_BRONZE, EFFECT_INDEX_0)
                        && pUnit->HasAura(SPELL_BROODAF_GREEN, EFFECT_INDEX_0))
                {
                    pUnit->RemoveAllAuras();
                    m_creature->CastSpell(pUnit,SPELL_CHROMATIC_MUT_1, false);
                }
            }

            m_uiMutationCheckTimer = 1000;
        }
        else
            m_uiMutationCheckTimer -= uiDiff;

        // Frenzy Timer
        if (m_uiFrenzyTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature, SPELL_FRENZY, false);

            DoScriptText(EMOTE_GENERIC_FRENZY_KILL, m_creature);
            m_uiFrenzyTimer = urand(10000, 15000);

        }
        else
            m_uiFrenzyTimer -= uiDiff;

        // Enrage if not already enraged and below 20%
        if (!m_bEnraged && m_creature->GetHealthPercent() < 20.0f)
        {
            m_creature->CastSpell(m_creature, SPELL_ENRAGE, false);
            m_bEnraged = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_chromaggus(Creature* pCreature)
{
    return new boss_chromaggusAI(pCreature);
}

void AddSC_boss_chromaggus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_chromaggus";
    pNewScript->GetAI = &GetAI_boss_chromaggus;
    pNewScript->RegisterSelf();
}
