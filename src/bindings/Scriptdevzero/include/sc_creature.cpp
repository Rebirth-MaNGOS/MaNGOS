/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#include "precompiled.h"
#include "Item.h"
#include "Spell.h"
#include "WorldPacket.h"
#include "ObjectMgr.h"

// Spell summary for ScriptedAI::SelectSpell
struct TSpellSummary
{
    uint8 Targets;                                          // set of enum SelectTarget
    uint8 Effects;                                          // set of enum SelectEffect
} *SpellSummary;

ScriptedAI::ScriptedAI(Creature* pCreature) : CreatureAI(pCreature),
    m_bCombatMovement(true),
    m_uiEvadeCheckCooldown(2500)
{}

void ScriptedAI::GetAIInformation(ChatHandler& reader)
{
    reader.PSendSysMessage("ScriptedAI, combat movement is %s", reader.GetOnOffStr(m_bCombatMovement));
}

bool ScriptedAI::IsVisible(Unit* pWho) const
{
    if (!pWho)
        return false;

    return m_creature->IsWithinDist(pWho, VISIBLE_RANGE) && pWho->isVisibleForOrDetect(m_creature, m_creature, true);
}

void ScriptedAI::MoveInLineOfSight(Unit* pWho)
{
    if (m_creature->CanInitiateAttack() && pWho->isTargetableForAttack() &&
            m_creature->IsHostileTo(pWho) && pWho->isInAccessablePlaceFor(m_creature))
    {
        /*if (!m_creature->CanFly() && m_creature->GetDistanceZ(pWho) > CREATURE_Z_ATTACK_RANGE)
            return;*/

        float attackDistance = m_creature->GetAttackDistance(pWho);
        this->OverrideAttackDistance(attackDistance, pWho);

        if (m_creature->IsWithinDistInMap(pWho, attackDistance) && m_creature->IsWithinLOSInMap(pWho))
        {
            if (!m_creature->getVictim())
            {
                if (m_creature->GetCreatureInfo()->rank == CREATURE_ELITE_WORLDBOSS)
                {
                    pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                }
                AttackStart(pWho);
            }
            else if (m_creature->GetMap()->IsDungeon())
            {
                pWho->SetInCombatWith(m_creature);
                m_creature->AddThreat(pWho);
            }
        }
    }
}

void ScriptedAI::AttackStart(Unit* pWho)
{
    if (pWho && m_creature->Attack(pWho, true))
    {
        m_creature->AddThreat(pWho);
        m_creature->SetInCombatWith(pWho);
        pWho->SetInCombatWith(m_creature);

        if (IsCombatMovement())
            m_creature->GetMotionMaster()->MoveChase(pWho);
    }
}

void ScriptedAI::EnterCombat(Unit* pEnemy)
{
    if (pEnemy)
        Aggro(pEnemy);
}

void ScriptedAI::Aggro(Unit* /*pEnemy*/)
{
}

void ScriptedAI::UpdateAI(const uint32 /*uiDiff*/)
{
    //Check if we have a current target
    if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        return;

    DoMeleeAttackIfReady();
}

void ScriptedAI::ResetToHome()
{
    m_creature->RemoveAllAuras();
    m_creature->DeleteThreatList();
    m_creature->CombatStop(true);
    m_creature->LoadCreatureAddon();

    if (m_creature->isAlive())
        m_creature->GetMotionMaster()->MoveTargetedHome();

    m_creature->SetLootRecipient(NULL);

    Reset();
}

void ScriptedAI::JustRespawned()
{
    Reset();
}

void ScriptedAI::DoStartMovement(Unit* pVictim, float fDistance, float fAngle)
{
    if (pVictim)
        m_creature->GetMotionMaster()->MoveChase(pVictim, fDistance, fAngle);
}

void ScriptedAI::DoStartNoMovement(Unit* pVictim)
{
    if (!pVictim)
        return;

    m_creature->GetMotionMaster()->MoveIdle();
    m_creature->StopMoving();
}

void ScriptedAI::DoStopAttack()
{
    if (m_creature->getVictim())
        m_creature->AttackStop();
}

void ScriptedAI::DoCast(Unit* pTarget, uint32 uiSpellId, bool bTriggered)
{
    if (m_creature->IsNonMeleeSpellCasted(false) && !bTriggered)
        return;

    m_creature->CastSpell(pTarget, uiSpellId, bTriggered);
}

void ScriptedAI::DoCastSpell(Unit* pTarget, SpellEntry const* pSpellInfo, bool bTriggered)
{
    if (m_creature->IsNonMeleeSpellCasted(false) && !bTriggered)
        return;

    m_creature->CastSpell(pTarget, pSpellInfo, bTriggered);
}

void ScriptedAI::DoPlaySoundToSet(WorldObject* pSource, uint32 uiSoundId)
{
    if (!pSource)
        return;

    if (!GetSoundEntriesStore()->LookupEntry(uiSoundId))
    {
        error_log("SD0: Invalid soundId %u used in DoPlaySoundToSet (Source: TypeId %u, GUID %u)", uiSoundId, pSource->GetTypeId(), pSource->GetGUIDLow());
        return;
    }

    pSource->PlayDirectSound(uiSoundId);
}

Creature* ScriptedAI::DoSpawnCreature(uint32 uiId, float fX, float fY, float fZ, float fAngle, uint32 uiType, uint32 uiDespawntime)
{
    return m_creature->SummonCreature(uiId,m_creature->GetPositionX()+fX, m_creature->GetPositionY()+fY, m_creature->GetPositionZ()+fZ, fAngle, (TempSummonType)uiType, uiDespawntime);
}

SpellEntry const* ScriptedAI::SelectSpell(Unit* pTarget, int32 /*uiSchool*/, int32 uiMechanic, SelectTarget selectTargets, uint32 uiPowerCostMin, uint32 uiPowerCostMax, float fRangeMin, float fRangeMax, SelectEffect selectEffects)
{
    //No target so we can't cast
    if (!pTarget)
        return nullptr;

    //Silenced so we can't cast
    if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
        return nullptr;

    //Using the extended script system we first create a list of viable spells
    SpellEntry const* apSpell[4];
    memset(apSpell, 0, sizeof(SpellEntry*)*4);

    uint32 uiSpellCount = 0;

    SpellEntry const* pTempSpell;
    SpellRangeEntry const* pTempRange;

    //Check if each spell is viable(set it to null if not)
    for (uint8 i = 0; i < 4; ++i)
    {
        pTempSpell = GetSpellStore()->LookupEntry(m_creature->m_spells[i]);

        //This spell doesn't exist
        if (!pTempSpell)
            continue;

        // Targets and Effects checked first as most used restrictions
        //Check the spell targets if specified
        if (selectTargets && !(SpellSummary[m_creature->m_spells[i]].Targets & (1 << (selectTargets-1))))
            continue;

        //Check the type of spell if we are looking for a specific spell type
        if (selectEffects && !(SpellSummary[m_creature->m_spells[i]].Effects & (1 << (selectEffects-1))))
            continue;

        //Check for school if specified
        /*if (uiSchool >= 0 && pTempSpell->SchoolMask & uiSchool)
            continue;*/

        //Check for spell mechanic if specified
        if (uiMechanic >= 0 && (int32) pTempSpell->Mechanic != uiMechanic)
            continue;

        //Make sure that the spell uses the requested amount of power
        if (uiPowerCostMin &&  pTempSpell->manaCost < uiPowerCostMin)
            continue;

        if (uiPowerCostMax && pTempSpell->manaCost > uiPowerCostMax)
            continue;

        //Continue if we don't have the mana to actually cast this spell
        if (pTempSpell->manaCost > m_creature->GetPower((Powers)pTempSpell->powerType))
            continue;

        //Get the Range
        pTempRange = GetSpellRangeStore()->LookupEntry(pTempSpell->rangeIndex);

        //Spell has invalid range store so we can't use it
        if (!pTempRange)
            continue;

        //Check if the spell meets our range requirements
        if (fRangeMin && pTempRange->maxRange < fRangeMin)
            continue;

        if (fRangeMax && pTempRange->maxRange > fRangeMax)
            continue;

        //Check if our target is in range
        if (SpellSummary[m_creature->m_spells[i]].Effects & SELECT_EFFECT_AURA)
        {
            //Don't cast a buff/debuff if the target already has it
            if (pTarget->HasAura(m_creature->m_spells[i]))
                continue;
        }

        //Check if our target is in range- creatures can always cast on themselves if the min range is 0
        if ( (m_creature == pTarget && pTempRange->minRange <= 0) || (!m_creature->IsWithinDistInMap(pTarget, pTempRange->minRange) && m_creature->IsWithinDistInMap(pTarget, pTempRange->maxRange)))
        {
            //All good so lets add it to the spell list
            apSpell[uiSpellCount] = pTempSpell;
            ++uiSpellCount;
        }
    }

    //We got our usable spells so now lets randomly pick one
    if (!uiSpellCount)
        return NULL;

    return apSpell[urand(0, uiSpellCount -1)];
}

bool ScriptedAI::CanCast(Unit* pTarget, SpellEntry const* pSpellEntry, bool bTriggered)
{
    //No target so we can't cast
    if (!pTarget || !pSpellEntry)
        return false;

    //Silenced so we can't cast
    if (!bTriggered && m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
        return false;

    //Check for power
    if (!bTriggered && m_creature->GetPower((Powers)pSpellEntry->powerType) < pSpellEntry->manaCost)
        return false;

    SpellRangeEntry const* pTempRange = GetSpellRangeStore()->LookupEntry(pSpellEntry->rangeIndex);

    //Spell has invalid range store so we can't use it
    if (!pTempRange)
        return false;

    //Unit is out of range of this spell
    if (!m_creature->IsInRange(pTarget, pTempRange->minRange, pTempRange->maxRange))
        return false;

    return true;
}

void FillSpellSummary()
{
    SpellSummary = new TSpellSummary[GetSpellStore()->GetNumRows()];

    SpellEntry const* pTempSpell;

    for (uint32 i = 0; i < GetSpellStore()->GetNumRows(); ++i)
    {
        SpellSummary[i].Effects = 0;
        SpellSummary[i].Targets = 0;

        pTempSpell = GetSpellStore()->LookupEntry(i);
        //This spell doesn't exist
        if (!pTempSpell)
            continue;

        for (uint8 j = 0; j < 3; ++j)
        {
            //Spell targets self
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_SELF)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SELF-1);

            //Spell targets a single enemy
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_CHAIN_DAMAGE ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_CURRENT_ENEMY_COORDINATES)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_ENEMY-1);

            //Spell targets AoE at enemy
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_ENEMY_IN_AREA ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_ENEMY_IN_AREA_INSTANT ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_CASTER_COORDINATES ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_ENEMY_IN_AREA_CHANNELED)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_ENEMY-1);

            //Spell targets an enemy
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_CHAIN_DAMAGE ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_CURRENT_ENEMY_COORDINATES ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_ENEMY_IN_AREA ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_ENEMY_IN_AREA_INSTANT ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_CASTER_COORDINATES ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_ENEMY_IN_AREA_CHANNELED)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_ENEMY-1);

            //Spell targets single friend
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_SINGLE_FRIEND ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_SINGLE_PARTY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_ALLY-1);


            //Spell targets a single friend(or self)
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_SELF ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_SINGLE_FRIEND ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_SINGLE_PARTY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_FRIEND-1);

            //Spell targets aoe friends
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_PARTY_AROUND_CASTER ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_AREAEFFECT_PARTY ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_CASTER_COORDINATES)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_FRIEND-1);

            //Spell targets any friend(or self)
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_SELF ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_SINGLE_FRIEND ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_SINGLE_PARTY ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_PARTY_AROUND_CASTER ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_AREAEFFECT_PARTY ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_CASTER_COORDINATES)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_FRIEND-1);

            //Spell targets any friend
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_SINGLE_FRIEND ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_SINGLE_PARTY ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_PARTY_AROUND_CASTER ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_AREAEFFECT_PARTY ||
                    pTempSpell->EffectImplicitTargetA[j] == TARGET_CASTER_COORDINATES)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_ALLY-1);


            //Make sure that this spell includes a damage effect
            if (pTempSpell->Effect[j] == SPELL_EFFECT_SCHOOL_DAMAGE ||
                    pTempSpell->Effect[j] == SPELL_EFFECT_INSTAKILL ||
                    pTempSpell->Effect[j] == SPELL_EFFECT_ENVIRONMENTAL_DAMAGE ||
                    pTempSpell->Effect[j] == SPELL_EFFECT_HEALTH_LEECH)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_DAMAGE-1);

            //Make sure that this spell includes a healing effect (or an apply aura with a periodic heal)
            if (pTempSpell->Effect[j] == SPELL_EFFECT_HEAL ||
                    pTempSpell->Effect[j] == SPELL_EFFECT_HEAL_MAX_HEALTH ||
                    pTempSpell->Effect[j] == SPELL_EFFECT_HEAL_MECHANICAL ||
                    (pTempSpell->Effect[j] == SPELL_EFFECT_APPLY_AURA  && pTempSpell->EffectApplyAuraName[j]== 8))
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_HEALING-1);

            //Make sure that this spell applies an aura
            if (pTempSpell->Effect[j] == SPELL_EFFECT_APPLY_AURA)
            {
                if (SpellSummary[i].Targets & (1 << (SELECT_TARGET_SELF-1)))
                {
                    //Certain aura abilities are actually HEALS, on account of we should use them below 30%
                    if (pTempSpell->Id == 13578 /* Jadefire */ ||
                            pTempSpell->Id == 3019 /* Enrage */ ||
                            pTempSpell->Id == 8599 /* Enrage */ ||
                            pTempSpell->Id == 8269 /* Enrage */ ||
                            pTempSpell->Id == 12795 /* Enrage */ ||
                            pTempSpell->Id == 15716 /* Enrage */ ||
                            pTempSpell->Id == 18501 /* Enrage */ ||
                            pTempSpell->Id == 28468 /* Enrage */ ||
                            pTempSpell->Id == 28747 /* Enrage */)
                    {
                        SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_HEALING-1);
                    } else
                    {
                        SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_AURA-1);
                    }
                } else
                {
                    SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_AURA-1);
                }
            }
        }
    }
}

void ScriptedAI::DoResetThreat()
{
    if (!m_creature->CanHaveThreatList() || m_creature->getThreatManager().isThreatListEmpty())
    {
        error_log("SD0: DoResetThreat called for creature that either cannot have threat list or has empty threat list (m_creature entry = %d)", m_creature->GetEntry());
        return;
    }

    ThreatList const& tList = m_creature->getThreatManager().getThreatList();
    for (ThreatList::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
    {
        Unit* pUnit = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid());

        if (pUnit && m_creature->getThreatManager().getThreat(pUnit))
            m_creature->getThreatManager().modifyThreatPercent(pUnit, -100);
    }
}

void ScriptedAI::DoTeleportPlayer(Unit* pUnit, float fX, float fY, float fZ, float fO)
{
    if (!pUnit)
        return;

    if (pUnit->GetTypeId() != TYPEID_PLAYER)
    {
        error_log("SD0: %s tried to teleport non-player (%s) to x: %f y:%f z: %f o: %f. Aborted.", m_creature->GetGuidStr().c_str(), pUnit->GetGuidStr().c_str(), fX, fY, fZ, fO);
        return;
    }

    ((Player*)pUnit)->TeleportTo(pUnit->GetMapId(), fX, fY, fZ, fO, TELE_TO_NOT_LEAVE_COMBAT);
}

Unit* ScriptedAI::DoSelectLowestHpFriendly(float fRange, uint32 uiMinHPDiff)
{
    Unit* pUnit = NULL;

    MaNGOS::MostHPMissingInRangeCheck u_check(m_creature, fRange, uiMinHPDiff);
    MaNGOS::UnitLastSearcher<MaNGOS::MostHPMissingInRangeCheck> searcher(pUnit, u_check);

    Cell::VisitGridObjects(m_creature, searcher, fRange);

    return pUnit;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyCC(float fRange)
{
    std::list<Creature*> pList;

    MaNGOS::FriendlyCCedInRangeCheck u_check(m_creature, fRange);
    MaNGOS::CreatureListSearcher<MaNGOS::FriendlyCCedInRangeCheck> searcher(pList, u_check);

    Cell::VisitGridObjects(m_creature, searcher, fRange);

    return pList;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyMissingBuff(float fRange, uint32 uiSpellId)
{
    std::list<Creature*> pList;

    MaNGOS::FriendlyMissingBuffInRangeCheck u_check(m_creature, fRange, uiSpellId);
    MaNGOS::CreatureListSearcher<MaNGOS::FriendlyMissingBuffInRangeCheck> searcher(pList, u_check);

    Cell::VisitGridObjects(m_creature, searcher, fRange);

    return pList;
}

std::list<Creature*> ScriptedAI::DoFindHostile(float range)
{
    std::list<Creature*> pList;

    MaNGOS::AnyHostileUnitInObjectRangeCheck check(m_creature, m_creature, range);
    MaNGOS::CreatureListSearcher<MaNGOS::AnyHostileUnitInObjectRangeCheck> searcher(pList, check);

    Cell::VisitGridObjects(m_creature, searcher, range);

    return pList;
}


Player* ScriptedAI::GetPlayerAtMinimumRange(float fMinimumRange)
{
    Player* pPlayer = NULL;

    MaNGOS::AnyPlayerInObjectRangeCheck check(m_creature, fMinimumRange);
    MaNGOS::PlayerSearcher<MaNGOS::AnyPlayerInObjectRangeCheck> searcher(pPlayer, check);

    Cell::VisitWorldObjects(m_creature, searcher, fMinimumRange);

    return pPlayer;
}

Player* ScriptedAI::GetPlayerFarthestAwayFromThreatlistWithinRange(float max_distance)
{
    if (!m_creature->getVictim())
        return nullptr;

    GUIDVector vGuids;
    m_creature->FillGuidsListFromThreatList(vGuids);
    if (vGuids.empty())
        return nullptr;


    Player* farthest_player = nullptr;
    float farthest_dist = 0;

    for (GUIDVector::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
    {
        if ((*itr).GetHigh() == HIGHGUID_PLAYER)
        {
            Player* current_player = m_creature->GetMap()->GetPlayer(*itr);
            if (current_player)
            {
                float current_distance = current_player->GetDistance(m_creature);
                if(current_player->isAlive() && current_distance < max_distance  && current_distance > farthest_dist && current_player->IsWithinLOSInMap(m_creature))
                {
                    farthest_player = current_player;
                    farthest_dist = current_distance;
                }
            }
        }
    }

    return farthest_player;
}

void ScriptedAI::SetEquipmentSlots(bool bLoadDefault, int32 uiMainHand, int32 uiOffHand, int32 uiRanged)
{
    if (bLoadDefault)
    {
        m_creature->LoadEquipment(m_creature->GetCreatureInfo()->equipmentId,true);
        return;
    }

    if (uiMainHand >= 0)
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(uiMainHand));

    if (uiOffHand >= 0)
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, uint32(uiOffHand));

    if (uiRanged >= 0)
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, uint32(uiRanged));
}

void ScriptedAI::SetCombatMovement(bool bCombatMove)
{
    m_bCombatMovement = bCombatMove;
}

enum eBosses
{
    NPC_BROODLORD   = 12017,
    NPC_HAKKAR      = 14834,
};

bool ScriptedAI::EnterEvadeIfOutOfCombatArea(const uint32 uiDiff)
{
    if (m_uiEvadeCheckCooldown < uiDiff)
        m_uiEvadeCheckCooldown = 2500;
    else
    {
        m_uiEvadeCheckCooldown -= uiDiff;
        return false;
    }

    if ( (m_creature->GetMotionMaster() && m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == HOME_MOTION_TYPE) || !m_creature->getVictim())
        return false;

    float fZ = m_creature->GetPositionZ();

    switch(m_creature->GetEntry())
    {
    case NPC_BROODLORD:                                 // broodlord (not move down stairs)
        if (fZ > 448.60f)
            return false;
        break;
    case NPC_HAKKAR:
        if (fZ > 51.0f)                                 // Hakkar (not move down stairs)
            return false;
        break;
    default:
        error_log("SD0: EnterEvadeIfOutOfCombatArea used for creature entry %u, but does not have any definition.", m_creature->GetEntry());
        return false;
    }

    ResetToHome();
    return true;
}
void Scripted_NoMovementAI::GetAIInformation(ChatHandler& reader)
{
    reader.PSendSysMessage("Subclass of Scripted_NoMovementAI");
}

void Scripted_NoMovementAI::AttackStart(Unit* pWho)
{
    if (pWho && m_creature->Attack(pWho, true))
    {
        m_creature->AddThreat(pWho);
        m_creature->SetInCombatWith(pWho);
        pWho->SetInCombatWith(m_creature);

        DoStartNoMovement(pWho);
    }
}

Unit* ScriptedAI::SelectUnitWithPower(Powers power)
{
    GUIDList lTargets;

    GUIDVector vGuids;
    m_creature->FillGuidsListFromThreatList(vGuids);

    if (vGuids.empty())
        return NULL;

    for (GUIDVector::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
    {
        Unit* pTarget = m_creature->GetMap()->GetUnit(*itr);

        if (pTarget && pTarget->getPowerType() == power)
            lTargets.push_back(pTarget->GetObjectGuid());
    }

    if (lTargets.empty())
        return NULL;

    GUIDList::iterator itr = lTargets.begin();
    advance(itr, urand(0, lTargets.size()-1));

    return m_creature->GetMap()->GetUnit((*itr));
}

Player* ScriptedAI::GetRandomPlayerInCurrentMap(float max_distance)    {
    Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();
    std::vector<Player*> playersInRange;

    if (max_distance == 0)
        for (auto player = players.begin(); player != players.end(); ++player) playersInRange.push_back(player->getSource());
    else
        for (auto player = players.begin(); player != players.end(); ++player)
        {
            float currentDistance = m_creature->GetDistance(player->getSource());
            if (max_distance >= currentDistance) playersInRange.push_back(player->getSource());
        }

    if (playersInRange.empty())
        return NULL;

    return playersInRange[urand(0, playersInRange.size() - 1)];

}

Unit* ScriptedAI::GetRandomTargetFromThreatList()
{
    ThreatList const& threatList = m_creature->getThreatManager().getThreatList();
    if (threatList.size() > 0)
    {
        uint32 random = urand(0, threatList.size() - 1);
        auto target_itr = threatList.begin();
        std::advance(target_itr, random);

        return (*target_itr)->getTarget();
    }

    return NULL;
}


void ScriptedAI::MakeTauntImmune()
{
    m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
    m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
}


//AI for patroling NPC
PatAI::PatAI(Creature* creature) : ScriptedAI(creature)
{
    Adds.clear();
    m_creature->SetVisibility(VISIBILITY_OFF);
    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
    m_creature->GetMotionMaster()->Clear(false, true);
    m_creature->GetMotionMaster()->MoveIdle();
    Reset();
}

void PatAI::Reset()
{
    init = false;
    updatetimer = 500;
    for(uint8 i = 0; i < Adds.size(); ++i)
    {
        if (Adds[i] && Adds[i]->IsInWorld() && Adds[i]->isAlive())
        {
            //Adds[i]->GetMotionMaster()->Clear(false,false);
            if (Adds[i]->GetOwnerGuid() == m_creature->GetObjectGuid())
                Adds[i]->SetOwnerGuid(ObjectGuid());
        }
    }
}

void PatAI::Aggro(Unit* /*U*/)
{
    for(uint8 i = 0; i < Adds.size(); ++i)
    {
        if (Adds[i] && Adds[i]->isAlive() && !Adds[i]->GetCharmerOrOwner())
            Adds[i]->SetOwnerGuid(m_creature->GetObjectGuid());
    }
}

void PatAI::UpdateAI(const uint32 diff)
{
    if(updatetimer <= diff)
    {
        if (!Adds.empty())
        {
            uint8 n = 0;
            Creature* C = 0;
            for(uint8 i = 0; i < Adds.size(); ++i)
            {
                if (!Adds[i] || !Adds[i]->IsInWorld() || !Adds[i]->isAlive() || Adds[i]->GetVisibility() == VISIBILITY_OFF
                        || (Adds[i]->GetCharmerOrOwner() && Adds[i]->GetCharmerOrOwner() != m_creature) )
                    n++;
                else
                {
                    if (!C)
                        C = Adds[i];
                    //if (Adds[i]->GetVisibility() == VISIBILITY_OFF && Adds[i]->GetDistance(m_creature) < 8.0f)
                    //	Adds[i]->SetVisibility(VISIBILITY_ON);
                    if (m_creature->getFaction() != 35 && (!m_creature->SelectHostileTarget() || !m_creature->getVictim()))
                    {
                        if (Adds[i]->SelectHostileTarget() && Adds[i]->getVictim()) //enter combat if add is in combat
                            AttackStart(Adds[i]->getVictim());
                    }
                }
            }
            if (m_creature->getFaction() != 35)
            {
                if (n == Adds.size())	//all adds dead, enter neutral mode
                {
                    float x,y,z;
                    init = false;
                    m_creature->GetRespawnCoord(x,y,z);
                    m_creature->Relocate(x,y,z,0);
                    m_creature->GetMotionMaster()->Clear(false, true);
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->setFaction(35);
                    ResetToHome();
                }
            }
            else if (C && !C->getVictim())
            {
                if (init)	//enter normal mode
                {
                    init = false;
                    m_creature->SetSpeedRate(MOVE_WALK,C->GetSpeedRate(MOVE_WALK));
                    m_creature->setFaction(C->getFaction());
                    m_creature->SetLevel(C->getLevel());
                    m_creature->GetMotionMaster()->Initialize();
                }
                else if (n != Adds.size())	//at least one add spawned, set timer to enter normal mode
                {
                    init = true;
                    m_creature->InitFormationMovement();
                }
            }
        }
        else //List empty, get adds from Formation Data
            m_creature->GetFormationMembers(Adds);

        updatetimer = 1500;
    }
    else updatetimer-=diff;
}

CreatureAI* GetAI_patrol(Creature* pCreature)
{
    return new PatAI(pCreature);
}

void AddSC_patrol()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "mob_patrol";
    pNewscript->GetAI = &GetAI_patrol;
    pNewscript->RegisterSelf();
}
