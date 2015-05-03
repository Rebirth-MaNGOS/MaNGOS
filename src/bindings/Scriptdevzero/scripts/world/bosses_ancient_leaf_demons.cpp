/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: bosses_ancient_leaf_demons
SD%Complete: 90
SDComment: They reset to start after transform into demon, should continue D:
SDCategory: Ancient Leaf Demons
EndScriptData */

/* ContentData
bosses_ancient_leaf_demon -- Superclass for the four demons
boss_klinfran_the_crazed
boss_solenor_the_slayer
boss_artorius_the_doombringer
boss_simone_the_seductress
mob_precious

EndContentData */

#include "precompiled.h"
#include "patrol_ai.h"

/*######
## bosses_ancient_leaf_demons -- Superclass for the four demons
######*/

enum eAncientLeafDemons
{
	SAY_INTERFERE					= -1000666,								// all shared abilities/things
	
	SPELL_FOOLS_PLIGHT				= 23504,
	SPELL_PRECIOUS_TRANSFORM_TO_FELHOUND_DND    = 23200,

	NPC_THE_CLEANER					= 14503,
	NPC_KLINFRAN_THE_CRAZED			= 14534,
	NPC_SOLENOR_THE_SLAYER			= 14530,
	NPC_ARTORIUS_THE_DOOMBRINGER	= 14535,
	NPC_SIMONE_THE_SEDUCTRESS		= 14533,
	NPC_PRECIOUS_THE_DEVOURER       = 14538,
	NPC_PRECIOUS                    = 14528,

    QUEST_STAVE_OF_THE_ANCIENTS     = 7636,
};

#define GOSSIP_ITEM_DEMON_CHALLENGE "I came for your head demon. I challenge you!"

struct MANGOS_DLL_DECL bosses_ancient_leaf_demonsAI : public npc_patrolAI
{
    bosses_ancient_leaf_demonsAI(Creature* pCreature) : npc_patrolAI(pCreature, 0.f, true)
    {
		m_uiDefaultEntry    = pCreature->GetEntry();
        m_uiResetOOCTimer   = 0;
        m_uiDemonTransTimer = 0;
		m_uiChallengerGUID.Clear();
		m_uiThreatListSize = 0;
		m_bDoneSpecial = false;			// otherwise they'll keep despawning at respawn
		StartPatrol(1, false);	

        Reset();
    }

	uint32 m_uiDefaultEntry;
    uint32 m_uiResetOOCTimer;
    uint32 m_uiDemonTransTimer;
	uint32 m_uiSummonCount;
	uint32 m_uiThreatListSize;

	ObjectGuid m_uiChallengerGUID;
    ObjectGuid m_uiPreciousPetGUID;

	bool m_bCanSay;
	bool m_bCleanerSay;
	bool m_bDoneSpecial;
	//bool hasStart;

    void Reset()					// only do these once
    {
		npc_patrolAI::Reset();

        if (m_creature->isAlive() && !m_creature->isActiveObject())
            m_creature->SetActiveObjectState(true);

		m_creature->SetSplineFlags(SPLINEFLAG_WALKMODE);				// make sure they don't run after OoC reset
		m_uiSummonCount = 0;
		m_bCanSay = true;
		m_bCleanerSay = true;
		//hasStart = false;
    }

    void JustReachedHome()
    {
    }

    void KilledUnit(Unit* /*pWho*/)
    {
        // Only one player can attack demon, so ..
        CompleteDemonChallenge();
    }

    void Aggro(Unit* pAttacker)
    {
		//if (!hasStart)				// do spell + despawn if demon challenge didn't start
		//{
		//	DoCastSpellIfCan(pAttacker, SPELL_FOOLS_PLIGHT, CAST_TRIGGERED);			// not needed for now since they're friendly
		//	if (!m_bDoneSpecial)
		//		DoSpecialDemonAbility();
		//}
        // The pAttacker has to be a player and must be a hunter
        // The hunter's pet is not allowed
        if ((!pAttacker->IsCharmerOrOwnerPlayerOrPlayerItself()) || (pAttacker->getClass() != CLASS_HUNTER))
        {
			if (!m_bDoneSpecial)
				DoSpecialDemonAbility();
            return;
        }
        ScriptedAI::Aggro(pAttacker);
    }

	void StartDemonChallenge(ObjectGuid pChallengerGUID)
    {
        if (!pChallengerGUID)
            return;

        m_uiChallengerGUID = pChallengerGUID;

        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        
        m_uiDemonTransTimer = 3000;
        m_uiResetOOCTimer   = 60000 + m_uiDemonTransTimer;
		m_creature->GetMotionMaster()->MovementExpired();
		m_creature->GetMotionMaster()->MoveIdle();

		//hasStart = true;

		PausePatrol(true);
    }

	void CompleteDemonChallenge(bool despawn = false)
    {
        if (despawn)
        {
            if (Creature* pPrecious = GetPreciousPet())
            {
                pPrecious->UpdateEntry(NPC_PRECIOUS);   // restore default entry
                pPrecious->ForcedDespawn(2000);
            }
			m_uiChallengerGUID.Clear();
			m_uiPreciousPetGUID.Clear();
            m_uiDemonTransTimer = 0;
            m_uiResetOOCTimer   = 0;
            m_creature->UpdateEntry(m_uiDefaultEntry);
            m_creature->ForcedDespawn(2000);
        }
        else
        {
            if (Creature* pPrecious = GetPreciousPet())
            {
                if (pPrecious->isAlive())
                {
                    if (pPrecious->isInCombat())
                        pPrecious->AI()->ResetToHome();
                    pPrecious->SetDeathState(JUST_DIED);
                    pPrecious->SetHealth(0);
                }
                pPrecious->UpdateEntry(NPC_PRECIOUS);
                pPrecious->Respawn();
            }

			m_uiChallengerGUID.Clear();
			m_uiPreciousPetGUID.Clear();
            m_uiDemonTransTimer = 0;
            m_uiResetOOCTimer   = 0;

            m_creature->UpdateEntry(m_uiDefaultEntry);
            m_creature->SetHealth(0);
            m_creature->SetDeathState(JUST_DIED);
            m_creature->Respawn();
        }
		Reset();
    }

	void OocTimerEndEvent()					// transform back from demon and continue waypointmovement and don't reset
	{
		m_uiChallengerGUID.Clear();
		m_uiPreciousPetGUID.Clear();
        m_uiDemonTransTimer = 0;
        m_uiResetOOCTimer   = 0;
		m_creature->UpdateEntry(m_uiDefaultEntry);
		PausePatrol(false);
		
		if (Creature* pPrecious = GetPreciousPet())
            pPrecious->UpdateEntry(NPC_PRECIOUS);   // restore default entry
	}

	Creature* GetPreciousPet()
    {
        Creature* pPrecious = NULL;

        if (m_uiPreciousPetGUID)
            pPrecious = m_creature->GetMap()->GetCreature(m_uiPreciousPetGUID);
        
        if (!pPrecious)
            pPrecious = GetClosestCreatureWithEntry(m_creature, NPC_PRECIOUS, DEFAULT_VISIBILITY_DISTANCE);

        if (!pPrecious)
            pPrecious = GetClosestCreatureWithEntry(m_creature, NPC_PRECIOUS_THE_DEVOURER, DEFAULT_VISIBILITY_DISTANCE);

        return pPrecious;
    }

	void SummonTheCleaner()
    {
		if (m_uiSummonCount < 1)	// max 1 Cleaner
        {
			float fX, fY, fZ;
			m_creature->GetPosition(fX, fY, fZ);
			for(uint8 i = 0; i < 1; ++i)
				if (Creature* pCleaner = m_creature->SummonCreature(NPC_THE_CLEANER, fX, fY, fZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 2000))
				{ 
					pCleaner->SetRespawnDelay(-10);				// to stop him from randomly respawning
				}
				CompleteDemonChallenge(true);
		}
    }

	virtual bool DoUniqueTransform() = 0;

    // Return true, if succeeded
    virtual bool DoSpecialDemonAbility() = 0;

    // Return true to handle shared timers and MeleeAttack
    virtual bool UpdateDemonAI(const uint32 /*uiDiff*/) { return true; }

    void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiDemonTransTimer)
        {
            // Transformation into Demon (event start)
            if (m_uiDemonTransTimer <= uiDiff)
            {
                m_uiDemonTransTimer = 0;

                if (Player* pChallenger = m_creature->GetMap()->GetPlayer(m_uiChallengerGUID))
                {
                    if (!pChallenger)
                    {
						OocTimerEndEvent();
                        //CompleteDemonChallenge();
						return;
                    }
                }
				// transform into the right demon
				DoUniqueTransform();
				m_bDoneSpecial = false;
            }
            else
                m_uiDemonTransTimer -= uiDiff;
        }

		if (m_creature->getThreatManager().getThreatList().size() > 1)
        {
			if (!m_bDoneSpecial)
				DoSpecialDemonAbility();
			return;
        }
		else if (Creature* pPrecious = GetPreciousPet())
        {
            if (pPrecious->getThreatManager().getThreatList().size() > 1)
            {
				if (!m_bDoneSpecial)
					DoSpecialDemonAbility();
				return;
            }
        }

		if (m_uiResetOOCTimer)
        {
            // Reset "Out of Combat"
            if (m_uiResetOOCTimer <= uiDiff)
            {
                if (!m_creature->isInCombat())
                {
                    m_uiResetOOCTimer = 0;
                    OocTimerEndEvent();
                }
                else
                    m_uiResetOOCTimer = 60000;
            }
            else
                m_uiResetOOCTimer -= uiDiff;
        }

		// Return since we have no target
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

		// keep updating so the timers for spells work
		UpdateDemonAI(uiDiff);

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_klinfran_the_crazed
######*/

enum eKlinfranTheCrazed
{
	SPELL_DEMONIC_FRENZY = 23257,
	SPELL_ENTROPIC_STING = 23260,
	SPELL_SCORPIDSTING = 14277,
};

// Klinfran the Crazed script
struct MANGOS_DLL_DECL boss_klinfran_the_crazedAI : public bosses_ancient_leaf_demonsAI
{
    boss_klinfran_the_crazedAI(Creature* pCreature) : bosses_ancient_leaf_demonsAI(pCreature) {Reset(); }			// Should he not enrage while stunned in trap?

	bool m_bEnraged;
	uint32 m_uiEnrageTimer;

    void Reset()
    {
        bosses_ancient_leaf_demonsAI::Reset();

		m_uiEnrageTimer = 15000;
		m_bEnraged = false;
    }

	bool DoUniqueTransform()
	{
		m_creature->UpdateEntry(NPC_KLINFRAN_THE_CRAZED);
		return true;
	}

    bool DoSpecialDemonAbility()
    {
		if (m_bCanSay)
		{
			DoScriptText(SAY_INTERFERE, m_creature);
			m_bCanSay = false;
		}
		m_creature->ForcedDespawn(2000);
		m_bDoneSpecial = true;
		return true;
    }

	void SpellHit(Unit* pCaster, SpellEntry const* pSpell)							// remove enrage with scorpidsting
    {
        if (pSpell->Id == SPELL_SCORPIDSTING)
        {
			 DoCastSpellIfCan(m_creature, SPELL_ENTROPIC_STING, CAST_TRIGGERED);
		}
		if (pSpell->Id == SPELL_DEMONIC_FRENZY)
        {
			m_creature->GenericTextEmote("Klinfran the Crazed goes into a killing frenzy!", NULL, false);
		}
	}

    bool UpdateDemonAI(const uint32 uiDiff)
    {
		if (m_creature->HasAura(SPELL_SCORPIDSTING))			// if hit by scorpid sting set melee damage to 1
		{
			 m_creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, 1);
             m_creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, 1);
             m_creature->UpdateDamagePhysical(BASE_ATTACK);
			 m_bEnraged = false;
		}
		else 
		{
			const CreatureInfo* cinfo = m_creature->GetCreatureInfo();
			m_creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg +(cinfo->mindmg/100)));
            m_creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg +(cinfo->maxdmg/100)));
            m_creature->UpdateDamagePhysical(BASE_ATTACK);
		}

		if (m_uiEnrageTimer <= uiDiff && !m_bEnraged)
        {
			DoCastSpellIfCan(m_creature, SPELL_DEMONIC_FRENZY, CAST_AURA_NOT_PRESENT);
			m_uiEnrageTimer = 15000;
			m_bEnraged = true;
        }
        else
            m_uiEnrageTimer -= uiDiff;

		if (m_uiEnrageTimer <= uiDiff && m_bEnraged)			// to make sure he always does enrage
			m_uiEnrageTimer = 15000;

		return true;
	}
};

CreatureAI* GetAI_boss_klinfran_the_crazed(Creature* pCreature)
{
    return new boss_klinfran_the_crazedAI(pCreature);
}

bool GossipHello_boss_klinfran_the_crazed(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_STAVE_OF_THE_ANCIENTS) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMON_CHALLENGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_boss_klinfran_the_crazed(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiSender == GOSSIP_SENDER_MAIN && uiAction == (GOSSIP_ACTION_INFO_DEF + 1))
        if (boss_klinfran_the_crazedAI* pKlinfranAI = dynamic_cast<boss_klinfran_the_crazedAI*>(pCreature->AI()))
			pKlinfranAI->StartDemonChallenge(pPlayer->GetObjectGuid());

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## boss_solenor_the_slayer
######*/

enum eSolenorTheSlayer
{
	SPELL_DREADFUL_FRIGHT = 23275,			// if player is not in melee, cast this
	SPELL_CREEPING_DOOM = 23589,			// if player is not in melee, cast this
	NPC_CREEPING_DOOM = 14761,
	SPELL_WINGCLIP = 14268,
	SPELL_CRIPPLING_CLIP = 23279,
	SPELL_SOUL_FLAME = 23272,
};

// Solenor the Slayer script
struct MANGOS_DLL_DECL boss_solenor_the_slayerAI : public bosses_ancient_leaf_demonsAI
{
    boss_solenor_the_slayerAI(Creature* pCreature) : bosses_ancient_leaf_demonsAI(pCreature) 
	{
		Reset();
	}

	uint32 m_uiDreadfulFrightTimer;
	uint32 m_uiCreepingDoomTimer;
	uint8 m_uiTargetsInRangeCount;
	uint32 m_uiCanCastTimer;

    void Reset()
    {
        bosses_ancient_leaf_demonsAI::Reset();
		m_uiDreadfulFrightTimer = urand(15000, 20000);
		m_uiCreepingDoomTimer = urand(8000, 12000);
		m_uiTargetsInRangeCount = 0;
		m_uiCanCastTimer = 0;
    }

	void SpellHit(Unit* pCaster, SpellEntry const* pSpell)							// freeze at place if wingclip
    {
        if (pSpell->Id == SPELL_WINGCLIP)
        {
			 DoCastSpellIfCan(m_creature, SPELL_CRIPPLING_CLIP, CAST_AURA_NOT_PRESENT);
		}

		if (pSpell->Id == SPELL_CRIPPLING_CLIP)										// Do emote if crippling crip is applied
        {
			 m_creature->GenericTextEmote("Solenor the Slayer is immobilized.", NULL, false);
		}
	}

    bool DoSpecialDemonAbility()
    {
		if (m_bCanSay)
		{
			DoScriptText(SAY_INTERFERE, m_creature);
			m_bCanSay = false;
		}
		m_creature->ForcedDespawn(2000);
		m_bDoneSpecial = true;
		return true;
    }

	bool DoUniqueTransform()
	{
		m_creature->UpdateEntry(NPC_SOLENOR_THE_SLAYER);
		DoCastSpellIfCan(m_creature, SPELL_SOUL_FLAME);
		return true;
	}

	void JustSummoned(Creature* pSummoned)
    {
		/*if (pSummoned->GetEntry() == NPC_CREEPING_DOOM)			// moved to DB
		{
		pSummoned->SetFactionTemporary(90);
        pSummoned->SetMaxHealth(urand(190,200));
		pSummoned->SetSpeedRate(MOVE_RUN, 0.35f, true);
		const CreatureInfo* cinfo = pSummoned->GetCreatureInfo();
        pSummoned->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE,250);
        pSummoned->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE,350);
		pSummoned->UpdateDamagePhysical(BASE_ATTACK);*/

        if (Unit* pTarget = m_creature->getVictim())
            pSummoned->AI()->AttackStart(pTarget);
		//}
	}

	void JustDied(Unit* /*pKiller*/)			// despawn creeping doom if boss dies
	{
		if (m_creature->IsTemporarySummon())
			m_creature->ForcedDespawn();
	}

    bool UpdateDemonAI(const uint32 uiDiff)
    {
		if (m_uiDreadfulFrightTimer <= uiDiff)
		{
			m_uiTargetsInRangeCount = 0;
			{
				for(uint8 i = 0; i < 5; ++i)
				{
					if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO,i))
						if (m_creature->CanReachWithMeleeAttack(pTarget))
							++m_uiTargetsInRangeCount;
				}
				
				if (m_uiTargetsInRangeCount < 1)			// if no player within melee cast fear
				{
					DoCastSpellIfCan(m_creature->getVictim(), SPELL_DREADFUL_FRIGHT);
					m_uiDreadfulFrightTimer = urand(15000, 20000);
				}
				else 
					m_uiDreadfulFrightTimer = urand(15000, 20000);
			}
		}
		else
			m_uiDreadfulFrightTimer -= uiDiff;

		// Summon Bugs
		if (m_uiCreepingDoomTimer <= uiDiff)
		{
			m_uiTargetsInRangeCount = 0;
		
			for(uint8 i = 0; i < 10; ++i)
			{
				if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO,i))
					if (m_creature->CanReachWithMeleeAttack(pTarget))
						++m_uiTargetsInRangeCount;
			}
		
			if (m_uiTargetsInRangeCount < 1)			// if no player within melee cast fear
			{
				float fX, fY, fZ;
				m_creature->GetPosition(fX, fY, fZ);
				for(uint8 i = 0; i < 1; ++i)
					if (Creature* pCreepingDoom = m_creature->SummonCreature(NPC_CREEPING_DOOM, fX+irand(-3,3), fY+irand(-3,3), fZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 2000))
					{ 
						pCreepingDoom->SetRespawnDelay(-10);				// to stop them from randomly respawning
						m_uiCreepingDoomTimer = urand(8000, 12000);
					}
			}
			else
				m_uiCreepingDoomTimer = urand(8000, 12000);
		}
		else
			m_uiCreepingDoomTimer -= uiDiff;

		return true;
	}
};

CreatureAI* GetAI_boss_solenor_the_slayer(Creature* pCreature)
{
    return new boss_solenor_the_slayerAI(pCreature);
}

bool GossipHello_boss_solenor_the_slayer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_STAVE_OF_THE_ANCIENTS) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMON_CHALLENGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_boss_solenor_the_slayer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiSender == GOSSIP_SENDER_MAIN && uiAction == (GOSSIP_ACTION_INFO_DEF + 1))
        if (boss_solenor_the_slayerAI* pSolenorAI = dynamic_cast<boss_solenor_the_slayerAI*>(pCreature->AI()))
			pSolenorAI->StartDemonChallenge(pPlayer->GetObjectGuid());

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## boss_artorius_the_doombringer
######*/

enum eArtoriustheDoombringer
{
	SPELL_DEMONIC_DOOM = 23298,
	SPELL_STINGING_TRAUMA = 23299,
	SPELL_SERPENTSTING_R8 = 13555,
	SPELL_SERPENTSTING_R9 = 25295,
};

// Artorius the Doombringer script
struct MANGOS_DLL_DECL boss_artorius_the_doombringerAI : public bosses_ancient_leaf_demonsAI
{
    boss_artorius_the_doombringerAI(Creature* pCreature) : bosses_ancient_leaf_demonsAI(pCreature) {Reset(); }			// Should he not enrage while stunned in trap?

	uint32 m_uiDemonicDoomTimer;
	uint32 m_uiEnrageTimer;

    void Reset()
    {
        bosses_ancient_leaf_demonsAI::Reset();
		m_uiDemonicDoomTimer = urand(10000, 20000);
		m_uiEnrageTimer = 15000;
    }

    bool DoSpecialDemonAbility()
    {
		if (m_bCanSay)
		{
			DoScriptText(SAY_INTERFERE, m_creature);
			m_bCanSay = false;
		}
		m_creature->ForcedDespawn(2000);
		m_bDoneSpecial = true;
		return true;
    }

	bool DoUniqueTransform()
	{
		m_creature->UpdateEntry(NPC_ARTORIUS_THE_DOOMBRINGER);
		return true;
	}

	void SpellHit(Unit* pCaster, SpellEntry const* pSpell)							// serpentsting should stack an invisible debuff that makes serpentsting do lots of damage
    {
        if (pSpell->Id == SPELL_SERPENTSTING_R9 || pSpell->Id == SPELL_SERPENTSTING_R8)
        {
			 DoCastSpellIfCan(m_creature, SPELL_STINGING_TRAUMA, CAST_TRIGGERED);
		}
		if (pSpell->Id == SPELL_DEMONIC_FRENZY)
        {
			m_creature->GenericTextEmote("Artorius the Doombringer goes into a killing frenzy!", NULL, false);
		}
	}

    bool UpdateDemonAI(const uint32 uiDiff)
    {
		if (m_creature->HasAura(SPELL_DEMONIC_FRENZY))			// Make sure he has about the same speed while enraged and not
		{
			m_creature->SetSpeedRate(MOVE_RUN, 1.1f);
		}
		else
			m_creature->SetSpeedRate(MOVE_RUN, 1.15f);

		if (m_uiEnrageTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_DEMONIC_FRENZY, CAST_TRIGGERED);
			m_uiEnrageTimer = 33000;
        }
        else
            m_uiEnrageTimer -= uiDiff;

		if (m_uiDemonicDoomTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEMONIC_DOOM, CAST_AURA_NOT_PRESENT);
			m_uiDemonicDoomTimer = 5000;
        }
        else
            m_uiDemonicDoomTimer -= uiDiff;

		return true;
	}
};

CreatureAI* GetAI_boss_artorius_the_doombringer(Creature* pCreature)
{
    return new boss_artorius_the_doombringerAI(pCreature);
}

bool GossipHello_boss_artorius_the_doombringer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_STAVE_OF_THE_ANCIENTS) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMON_CHALLENGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_boss_artorius_the_doombringer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiSender == GOSSIP_SENDER_MAIN && uiAction == (GOSSIP_ACTION_INFO_DEF + 1))
        if (boss_artorius_the_doombringerAI* pArtoriusAI = dynamic_cast<boss_artorius_the_doombringerAI*>(pCreature->AI()))
			pArtoriusAI->StartDemonChallenge(pPlayer->GetObjectGuid());

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## boss_simone_the_seductress
######*/

enum eSimonetheSeductress
{
	SPELL_TEMPTRESS_KISS = 23205,
	SPELL_VIPERSTING = 14280,
	SPELL_CHAIN_LIGHTNING = 23106,
};

// Simone the Seductress script
struct MANGOS_DLL_DECL boss_simone_the_seductressAI : public bosses_ancient_leaf_demonsAI
{
    boss_simone_the_seductressAI(Creature* pCreature) : bosses_ancient_leaf_demonsAI(pCreature) {Reset(); }

	uint32 m_uiChainLightningTimer;
	uint32 m_uiSeductressKissTimer;

	bool m_bisSilence;

    void Reset()
    {
        bosses_ancient_leaf_demonsAI::Reset();
		m_uiChainLightningTimer = urand(6000, 16000);
		m_uiSeductressKissTimer = 5000;
		m_bisSilence = false;
    }

	void JustSummoned(Creature* pSummoned)	
    {
		++m_uiSummonCount;
	}

	void JustDied(Unit* pKiller)
    {
        if (pKiller->GetObjectGuid() != m_uiChallengerGUID)			// if someone else but the hunter kills the demon, summon cleaner and don't let anyone loot head
        {    
			SummonTheCleaner();
			m_creature->SetLootRecipient(NULL);
		}
    }

    bool DoSpecialDemonAbility()
    {
		if (m_bCanSay)
		{
			DoScriptText(SAY_INTERFERE, m_creature);
			m_bCanSay = false;
		}
		SummonTheCleaner();
		m_bDoneSpecial = true;
		return true;
    }
	
	bool DoUniqueTransform()
	{
		m_creature->UpdateEntry(NPC_SIMONE_THE_SEDUCTRESS);
		if (Creature* pPrecious = GetPreciousPet())
           {
				m_uiPreciousPetGUID = pPrecious->GetObjectGuid();
                pPrecious->UpdateEntry(NPC_PRECIOUS_THE_DEVOURER);
                pPrecious->CastSpell(pPrecious, SPELL_PRECIOUS_TRANSFORM_TO_FELHOUND_DND, false);
            }
            else
				error_log("SD0: UnGoro: Hunter epic quest challenge event started, but Simone's pet Precious has been not handled.");

		return true;
	}

    bool UpdateDemonAI(const uint32 uiDiff)
    {
		if (m_creature->HasAura(SPELL_VIPERSTING))			// if hit by viper sting, do not cast any spells
		{
			m_bisSilence = true;
		}
		else
			m_bisSilence = false;

		if (m_uiChainLightningTimer <= uiDiff)
        {
			if (!m_bisSilence)
			{
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAIN_LIGHTNING);
			m_uiChainLightningTimer = urand(6000, 16000);
			}
			else
				m_uiChainLightningTimer = urand(6000, 16000);
        }
        else
            m_uiChainLightningTimer -= uiDiff;

		if (m_uiSeductressKissTimer <= uiDiff)
        {
			if (!m_bisSilence)
			{
				DoCastSpellIfCan(m_creature->getVictim(), SPELL_TEMPTRESS_KISS, CAST_AURA_NOT_PRESENT);
				m_uiSeductressKissTimer = 5000;
			}
			else
				m_uiSeductressKissTimer = 5000;
        }
        else
            m_uiSeductressKissTimer -= uiDiff;

		return true;
	}
};

CreatureAI* GetAI_boss_simone_the_seductress(Creature* pCreature)
{
    return new boss_simone_the_seductressAI(pCreature);
}

bool GossipHello_boss_simone_the_seductress(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_STAVE_OF_THE_ANCIENTS) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMON_CHALLENGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_boss_simone_the_seductress(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiSender == GOSSIP_SENDER_MAIN && uiAction == (GOSSIP_ACTION_INFO_DEF + 1))
        if (boss_simone_the_seductressAI* pSimoneAI = dynamic_cast<boss_simone_the_seductressAI*>(pCreature->AI()))
			pSimoneAI->StartDemonChallenge(pPlayer->GetObjectGuid());

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## mob_precious
######*/

// Precious' script
struct MANGOS_DLL_DECL mob_preciousAI : public ScriptedAI
{
    mob_preciousAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiDefaultEntry = pCreature->GetEntry();
        Reset();
    }

	uint32 m_uiDefaultEntry;

	void Reset()
    {
        if (m_creature->isAlive() && !m_creature->isActiveObject())
            m_creature->SetActiveObjectState(true);
	}
	
	void Aggro(Unit* pAttacker)
    {
        if (Creature* pSimone = GetClosestCreatureWithEntry(m_creature, NPC_SIMONE_THE_SEDUCTRESS, DEFAULT_VISIBILITY_DISTANCE))
        {
            if (boss_simone_the_seductressAI* pSimoneAI = dynamic_cast<boss_simone_the_seductressAI*>(pSimone->AI()))
            {
                if (!pSimone->SelectHostileTarget() && !m_creature->getVictim())
                    pSimoneAI->Aggro(pAttacker);
            }
        }
        ScriptedAI::Aggro(pAttacker);
    }

    void JustDied(Unit* pKiller)
    {
		m_creature->RemoveAllAuras();
	    m_creature->UpdateEntry(m_uiDefaultEntry);
        ScriptedAI::JustDied(pKiller);
    }

	void UpdateAI(const uint32 uiDiff)
    {
		// Return since we have no target
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_precious(Creature* pCreature)
{
    return new mob_preciousAI(pCreature);
}

void AddSC_bosses_ancient_leaf_demons()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_klinfran_the_crazed";
    pNewScript->GetAI = &GetAI_boss_klinfran_the_crazed;
	pNewScript->pGossipHello = &GossipHello_boss_klinfran_the_crazed;
    pNewScript->pGossipSelect = &GossipSelect_boss_klinfran_the_crazed;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "boss_solenor_the_slayer";
    pNewScript->GetAI = &GetAI_boss_solenor_the_slayer;
	pNewScript->pGossipHello = &GossipHello_boss_solenor_the_slayer;
    pNewScript->pGossipSelect = &GossipSelect_boss_solenor_the_slayer;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "boss_artorius_the_doombringer";
    pNewScript->GetAI = &GetAI_boss_artorius_the_doombringer;
	pNewScript->pGossipHello = &GossipHello_boss_artorius_the_doombringer;
    pNewScript->pGossipSelect = &GossipSelect_boss_artorius_the_doombringer;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "boss_simone_the_seductress";
    pNewScript->GetAI = &GetAI_boss_simone_the_seductress;
	pNewScript->pGossipHello = &GossipHello_boss_simone_the_seductress;
    pNewScript->pGossipSelect = &GossipSelect_boss_simone_the_seductress;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "mob_precious";
    pNewScript->GetAI = &GetAI_mob_precious;
    pNewScript->RegisterSelf();
}
