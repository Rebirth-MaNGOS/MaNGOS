/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos-zero>
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

#include "ThreatManager.h"
#include "Unit.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "Map.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "UnitEvents.h"

//==============================================================
//================= ThreatCalcHelper ===========================
//==============================================================

// The pHatingUnit is not used yet
float ThreatCalcHelper::CalcThreat(Unit* pHatedUnit, Unit* /*pHatingUnit*/, float threat, bool crit, SpellSchoolMask schoolMask, SpellEntry const *pThreatSpell)
{
    // all flat mods applied early
    if (!threat)
        return 0.0f;

    if (pThreatSpell)
    {
        if (pThreatSpell->AttributesEx & SPELL_ATTR_EX_NO_THREAT
			&& (pThreatSpell->SpellVisual != 72 || pThreatSpell->SpellIconID != 331))		//Fade
            return 0.0f;

        if (Player* modOwner = pHatedUnit->GetSpellModOwner())
            modOwner->ApplySpellMod(pThreatSpell->Id, SPELLMOD_THREAT, threat);

        if (crit)
            threat *= pHatedUnit->GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_CRITICAL_THREAT, schoolMask);
    }

    threat = pHatedUnit->ApplyTotalThreatModifier(threat, schoolMask);
    return threat;
}

//============================================================
//================= HostileReference ==========================
//============================================================

HostileReference::HostileReference(Unit* pUnit, ThreatManager *pThreatManager, float pThreat)
{
    iThreat = pThreat;
    iTempThreatModifyer = 0.0f;
    link(pUnit, pThreatManager);
    iUnitGuid = pUnit->GetObjectGuid();
    iOnline = true;
    iAccessible = true;
}

//============================================================
// Tell our refTo (target) object that we have a link
void HostileReference::targetObjectBuildLink()
{
    getTarget()->addHatedBy(this);
}

//============================================================
// Tell our refTo (taget) object, that the link is cut
void HostileReference::targetObjectDestroyLink()
{
    getTarget()->removeHatedBy(this);
}

//============================================================
// Tell our refFrom (source) object, that the link is cut (Target destroyed)

void HostileReference::sourceObjectDestroyLink()
{
    setOnlineOfflineState(false);
}

//============================================================
// Inform the source, that the status of the reference changed

void HostileReference::fireStatusChanged(ThreatRefStatusChangeEvent& pThreatRefStatusChangeEvent)
{
    if(getSource())
        getSource()->processThreatEvent(&pThreatRefStatusChangeEvent);
}

//============================================================

void HostileReference::addThreat(float pMod)
{
    iThreat += pMod;
    // the threat is changed. Source and target unit have to be availabe
    // if the link was cut before relink it again
    if(!isOnline())
        updateOnlineStatus();
    if(pMod != 0.0f)
    {
        ThreatRefStatusChangeEvent event(UEV_THREAT_REF_THREAT_CHANGE, this, pMod);
        fireStatusChanged(event);
    }

    if(isValid() && pMod >= 0)
    {
        Unit* victim_owner = getTarget()->GetOwner();
        if(victim_owner && victim_owner->isAlive())
            getSource()->addThreat(victim_owner, 0.0f);     // create a threat to the owner of a pet, if the pet attacks
    }
}

//============================================================
// check, if source can reach target and set the status

void HostileReference::updateOnlineStatus()
{
    bool online = false;
    bool accessible = false;

    if (!isValid())
    {
        if (Unit* target = ObjectAccessor::GetUnit(*getSourceUnit(), getUnitGuid()))
            link(target, getSource());
    }
    // only check for online status if
    // ref is valid
    // target is no player or not gamemaster
    // target is not in flight
    if(isValid() &&
        ((getTarget()->GetTypeId() != TYPEID_PLAYER || !((Player*)getTarget())->isGameMaster()) ||
        !getTarget()->IsTaxiFlying()))
    {
        Creature* creature = (Creature* ) getSourceUnit();
        online = getTarget()->isInAccessablePlaceFor(creature);
        if(!online)
        {
            if(creature->AI()->canReachByRangeAttack(getTarget()))
                online = true;                              // not accessable but stays online
        }
        else
            accessible = true;

    }

    // Don't remove a player from the threat list while it's falling.
    Player* pPlayer = dynamic_cast<Player*>(getTarget());
    if (pPlayer && pPlayer->m_movementInfo.fallTime > 0)
        return;

    setAccessibleState(accessible);
    setOnlineOfflineState(online);
}

//============================================================
// set the status and fire the event on status change

void HostileReference::setOnlineOfflineState(bool pIsOnline)
{
    if(iOnline != pIsOnline)
    {
        iOnline = pIsOnline;
        if(!iOnline)
            setAccessibleState(false);                      // if not online that not accessable as well

        ThreatRefStatusChangeEvent event(UEV_THREAT_REF_ONLINE_STATUS, this);
        fireStatusChanged(event);
    }
}

//============================================================

void HostileReference::setAccessibleState(bool pIsAccessible)
{
    if(iAccessible != pIsAccessible)
    {
        iAccessible = pIsAccessible;

        ThreatRefStatusChangeEvent event(UEV_THREAT_REF_ASSECCIBLE_STATUS, this);
        fireStatusChanged(event);
    }
}

//============================================================
// prepare the reference for deleting
// this is called be the target

void HostileReference::removeReference()
{
    invalidate();

    ThreatRefStatusChangeEvent event(UEV_THREAT_REF_REMOVE_FROM_LIST, this);
    fireStatusChanged(event);
}

//============================================================

Unit* HostileReference::getSourceUnit()
{
    return (getSource()->getOwner());
}

//============================================================
//================ ThreatContainer ===========================
//============================================================

void ThreatContainer::clearReferences()
{
    for(ThreatList::const_iterator i = iThreatList.begin(); i != iThreatList.end(); ++i)
    {
        (*i)->unlink();
        delete (*i);
    }
    iThreatList.clear();
}

//============================================================
// Return the HostileReference of NULL, if not found
HostileReference* ThreatContainer::getReferenceByTarget(Unit* pVictim)
{
    HostileReference* result = NULL;
    ObjectGuid guid = pVictim->GetObjectGuid();
    for(ThreatList::const_iterator i = iThreatList.begin(); i != iThreatList.end(); ++i)
    {
        if ((*i)->getUnitGuid() == guid)
        {
            result = (*i);
            break;
        }
    }

    return result;
}

//============================================================
// Add the threat, if we find the reference

HostileReference* ThreatContainer::addThreat(Unit* pVictim, float pThreat)
{
    HostileReference* ref = getReferenceByTarget(pVictim);
    if(ref)
        ref->addThreat(pThreat);
    return ref;
}

//============================================================

void ThreatContainer::modifyThreatPercent(Unit *pVictim, int32 pPercent)
{
    if(HostileReference* ref = getReferenceByTarget(pVictim))
    {
        if(pPercent < -100)
        {
            ref->removeReference();
            delete ref;
        }
        else
            ref->addThreatPercent(pPercent);
    }
}

//============================================================

bool HostileReferenceSortPredicate(const HostileReference* lhs, const HostileReference* rhs)
{
    // std::list::sort ordering predicate must be: (Pred(x,y)&&Pred(y,x))==false
    return lhs->getThreat() > rhs->getThreat();             // reverse sorting
}

//============================================================
// Check if the list is dirty and sort if necessary

void ThreatContainer::update()
{
    if(iDirty && iThreatList.size() >1)
    {
        iThreatList.sort(HostileReferenceSortPredicate);
    }
    iDirty = false;
}

//============================================================
// return the next best victim
// could be the current victim

HostileReference* ThreatContainer::selectNextVictim(Creature* pAttacker, HostileReference* pCurrentVictim)
{
    HostileReference* currentRef = NULL;

    ThreatList::const_iterator lastRef = iThreatList.end();
    lastRef--;

	if (pCurrentVictim != NULL && (pCurrentVictim->getTarget()->GetCharmerOrOwnerOrSelf()->IsFriendlyTo(pAttacker) || 
									pCurrentVictim->getTarget()->hasUnitState(UNIT_STAT_LOST_CONTROL) || 
									pCurrentVictim->getTarget()->IsImmunedToDamage(pAttacker->GetMeleeDamageSchoolMask()) || 
									pCurrentVictim->getTarget()->hasNegativeAuraWithInterruptFlag(AURA_INTERRUPT_FLAG_DAMAGE) ||
                                    pAttacker->IsOutOfThreatArea(pCurrentVictim->getTarget()) ||
                                    (pAttacker->HasAuraType(SPELL_AURA_MOD_ROOT) && !pAttacker->CanReachWithMeleeAttack(pCurrentVictim->getTarget()))))
    {
		//Our current target is kind of crappy, don't prioritize it above other targets
		pCurrentVictim = NULL;
	}

	//Get our "tank" target - the highest non-friendly item on our threat list - if we can't find one,
	//we should reset our threat list and go home.  If we can, then we'll fall back on them if everyone has
	//lost control / is immune
	HostileReference *tankRef = NULL;
	bool foundEnemyTarget = false;

	for (ThreatList::const_iterator iter = iThreatList.begin(); iter != iThreatList.end();++iter)
        {
		Unit *target = (*iter)->getTarget();

		if (!target->GetCharmerOrOwnerOrSelf()->IsFriendlyTo(pAttacker))
            {
			foundEnemyTarget = true;
			if (!pAttacker->IsOutOfThreatArea(target) && ((*iter) == pCurrentVictim || tankRef == NULL))
			{
				tankRef = (*iter);
				if ((*iter) == pCurrentVictim)
					break;
            }
		}
	}

	//Everyone is mind controlled- reset
	if (!foundEnemyTarget)
    {
		// Clean the threat list and return nullptr.
		for(ThreatList::const_iterator i = iThreatList.begin(); i != iThreatList.end(); ++i)
		{
			(*i)->unlink();
			delete (*i);
		}
		iThreatList.clear();

		return nullptr;
	}

	//Go through targets in our threat list now & find someone with high enough threat to use
	//Ignore targets with no control in this pass
    for(ThreatList::const_iterator iter = iThreatList.begin(); iter != iThreatList.end();++iter)
    {
        HostileReference *iterTarget = (*iter);

        Unit* target = iterTarget->getTarget();
        MANGOS_ASSERT(target);                              // if the ref has status online the target must be there !

		//Outright ignore targets with no control of themselves (or pally bubble, or break-on-hit debuff) - we'll go after the "tank" determined above if everyone is jacked
		if (target->GetCharmerOrOwnerOrSelf()->IsFriendlyTo(pAttacker) || target->hasUnitState(UNIT_STAT_LOST_CONTROL) || 
			target->IsImmunedToDamage(pAttacker->GetMeleeDamageSchoolMask()) || target->hasNegativeAuraWithInterruptFlag(AURA_INTERRUPT_FLAG_DAMAGE) ||
            (pAttacker->HasAuraType(SPELL_AURA_MOD_ROOT) && !pAttacker->CanReachWithMeleeAttack(target)))
		{
                continue;
            }

        if (!pAttacker->IsOutOfThreatArea(target))          // skip non attackable currently targets
        {
            if (pCurrentVictim)                             // select 1.3/1.1 better target in comparison current target
            {
                // list sorted and and we check current target, then this is best case
                if(pCurrentVictim == iterTarget || iterTarget->getThreat() <= 1.1f * pCurrentVictim->getThreat() )
                {
                    currentRef = pCurrentVictim;            // for second case
                    break;
                }

                if (iterTarget->getThreat() > 1.3f * pCurrentVictim->getThreat() ||
                     (iterTarget->getThreat() > 1.1f * pCurrentVictim->getThreat() &&
                     pAttacker->CanReachWithMeleeAttack(target)) )
                {                                           //implement 110% threat rule for targets in melee range
                    currentRef = iterTarget;                //and 130% rule for targets in ranged distances
                    break;                                  //for selecting alive targets
                }
            }
            else                                            // select any
            {
                currentRef = iterTarget;
                break;
            }
        }
    }

    if (!currentRef)
		currentRef = tankRef;

    return currentRef;
}

//============================================================
//=================== ThreatManager ==========================
//============================================================

ThreatManager::ThreatManager(Unit* owner) : iCurrentVictim(NULL), iOwner(owner)
{
}

//============================================================

void ThreatManager::clearReferences()
{
    iThreatContainer.clearReferences();
    iThreatOfflineContainer.clearReferences();
    iCurrentVictim = NULL;
}

//============================================================

void ThreatManager::addThreat(Unit* pVictim, float pThreat, bool crit, SpellSchoolMask schoolMask, SpellEntry const *pThreatSpell)
{
    //function deals with adding threat and adding players and pets into ThreatList
    //mobs, NPCs, guards have ThreatList and HateOfflineList
    //players and pets have only InHateListOf
    //HateOfflineList is used co contain unattackable victims (in-flight, in-water, GM etc.)

    // not to self
    if (pVictim == getOwner())
        return;

    // not to GM
    if (!pVictim || (pVictim->GetTypeId() == TYPEID_PLAYER && ((Player*)pVictim)->isGameMaster()) )
        return;

    // not to dead and not for dead
    if(!pVictim->isAlive() || !getOwner()->isAlive() )
        return;

    MANGOS_ASSERT(getOwner()->GetTypeId()== TYPEID_UNIT);

    float threat = ThreatCalcHelper::CalcThreat(pVictim, iOwner, pThreat, crit, schoolMask, pThreatSpell);

    addThreatDirectly(pVictim, threat);
}

void ThreatManager::addThreatDirectly(Unit* pVictim, float threat)
{
    HostileReference* ref = iThreatContainer.addThreat(pVictim, threat);
    // Ref is not in the online refs, search the offline refs next
    if (!ref)
        ref = iThreatOfflineContainer.addThreat(pVictim, threat);

    if(!ref)                                                // there was no ref => create a new one
    {
        // threat has to be 0 here
        HostileReference* hostileReference = new HostileReference(pVictim, this, 0);
        iThreatContainer.addReference(hostileReference);
        hostileReference->addThreat(threat);                // now we add the real threat
        if(pVictim->GetTypeId() == TYPEID_PLAYER && ((Player*)pVictim)->isGameMaster())
            hostileReference->setOnlineOfflineState(false); // GM is always offline
    }
}

//============================================================

void ThreatManager::modifyThreatPercent(Unit *pVictim, int32 pPercent)
{
    iThreatContainer.modifyThreatPercent(pVictim, pPercent);
}

//============================================================

Unit* ThreatManager::getHostileTarget()
{
    iThreatContainer.update();
    HostileReference* nextVictim = iThreatContainer.selectNextVictim((Creature*) getOwner(), getCurrentVictim());
    setCurrentVictim(nextVictim);
    return getCurrentVictim() != NULL ? getCurrentVictim()->getTarget() : NULL;
}

//============================================================

float ThreatManager::getThreat(Unit *pVictim, bool pAlsoSearchOfflineList)
{
    float threat = 0.0f;
    HostileReference* ref = iThreatContainer.getReferenceByTarget(pVictim);
    if(!ref && pAlsoSearchOfflineList)
        ref = iThreatOfflineContainer.getReferenceByTarget(pVictim);
    if(ref)
        threat = ref->getThreat();
    return threat;
}

//============================================================

void ThreatManager::tauntApply(Unit* pTaunter)
{
    if(HostileReference* ref = iThreatContainer.getReferenceByTarget(pTaunter))
    {
        if(getCurrentVictim() && (ref->getThreat() < getCurrentVictim()->getThreat()))
        {
            // Ok, temp threat is unused
            if(ref->getTempThreatModifyer() == 0.0f)
            {
                ref->setTempThreat(getCurrentVictim()->getThreat());
            }
        }
    }
}

//============================================================

void ThreatManager::tauntFadeOut(Unit *pTaunter)
{
    if(HostileReference* ref = iThreatContainer.getReferenceByTarget(pTaunter))
    {
        ref->resetTempThreat();
    }
}

//============================================================

void ThreatManager::setCurrentVictim(HostileReference* pHostileReference)
{
    iCurrentVictim = pHostileReference;
}

//============================================================
// The hated unit is gone, dead or deleted
// return true, if the event is consumed

void ThreatManager::processThreatEvent(ThreatRefStatusChangeEvent* threatRefStatusChangeEvent)
{
    threatRefStatusChangeEvent->setThreatManager(this);     // now we can set the threat manager

    HostileReference* hostileReference = threatRefStatusChangeEvent->getReference();

    switch(threatRefStatusChangeEvent->getType())
    {
        case UEV_THREAT_REF_THREAT_CHANGE:
            if((getCurrentVictim() == hostileReference && threatRefStatusChangeEvent->getFValue()<0.0f) ||
                (getCurrentVictim() != hostileReference && threatRefStatusChangeEvent->getFValue()>0.0f))
                setDirty(true);                             // the order in the threat list might have changed
            break;
        case UEV_THREAT_REF_ONLINE_STATUS:
            if(!hostileReference->isOnline())
            {
                if (hostileReference == getCurrentVictim())
                {
                    setCurrentVictim(NULL);
                    setDirty(true);
                }
                iThreatContainer.remove(hostileReference);
                iThreatOfflineContainer.addReference(hostileReference);
            }
            else
            {
                if(getCurrentVictim() && hostileReference->getThreat() > (1.1f * getCurrentVictim()->getThreat()))
                    setDirty(true);
                iThreatContainer.addReference(hostileReference);
                iThreatOfflineContainer.remove(hostileReference);
            }
            break;
        case UEV_THREAT_REF_REMOVE_FROM_LIST:
            if (hostileReference == getCurrentVictim())
            {
                setCurrentVictim(NULL);
                setDirty(true);
            }
            if(hostileReference->isOnline())
            {
                iThreatContainer.remove(hostileReference);
            }
            else
                iThreatOfflineContainer.remove(hostileReference);
            break;
    }
}
