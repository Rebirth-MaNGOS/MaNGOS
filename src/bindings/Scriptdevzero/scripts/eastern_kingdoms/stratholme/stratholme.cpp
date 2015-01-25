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
SDName: Stratholme
SD%Complete: 100
SDComment:
SDCategory: Stratholme
EndScriptData */

/* ContentData
go_blacksmithing_plans
go_postbox
go_stratholme_gate
mob_abomination
mob_freed_soul
mob_restless_soul
mobs_spectral_ghostly_citizen
mob_the_unforgiven
mob_eye_of_naxxramas
npc_aurius
npc_stratholme_trigger
EndContentData */

#include "precompiled.h"
#include "stratholme.h"

/*######
## go_blacksmithing_plans
######*/

bool GOUse_go_blacksmithing_plans(Player* pPlayer, GameObject* pGo)
{
    // Use GO only one time
    if (pGo->GetUseCount() != 0)
        return false;

    float x, y, z;
    pGo->GetPosition(x, y, z);
    Creature* pSummoned = NULL;

    switch(pGo->GetEntry())
    {
        case 176325:        // Living Stratholme side, Summon "Crimson Hammersmith"
            if ((pSummoned = pGo->SummonCreature(NPC_CRIMSON_HAMMERSMITH, x, y, z, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0)))
                pSummoned->AI()->AttackStart(pPlayer);
            break;
        case 176327:        // Undead Stratholme side, Summon "Black Guard Swordsmith"
            if ((pSummoned = pGo->SummonCreature(NPC_BLACK_GUARD_SWORDSMITH, x, y, z, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0)))
                pSummoned->AI()->AttackStart(pPlayer);
            break;
        default:
            error_log("SD0: Stratholme: Script for gameobject \"Blacksmithing Plans\" used by another object, than should be! GUID: %u ENTRY: %u", pGo->GetObjectGuid().GetCounter(), pGo->GetEntry());
    }

    pGo->AddUse();
    return false;
}

/*######
## go_stratholme_gate
######*/

bool GOUse_go_stratholme_gate(Player* /*pPlayer*/, GameObject* pGo)
{
    instance_stratholme* m_pInstance = (instance_stratholme*)pGo->GetInstanceData();

    if (!m_pInstance)
        return false;

    switch(pGo->GetEntry())
    {
        case GO_GAUNTLET_INNER_GATE:
            if (m_pInstance->GetData(TYPE_BARON_RUN) == NOT_STARTED)
                m_pInstance->SetData(TYPE_BARON_RUN, IN_PROGRESS);
            break;
        case GO_SERVICE_ENTRANCE:
            if (m_pInstance->GetData(TYPE_MAGISTRATE) == NOT_STARTED)
                m_pInstance->SetData(TYPE_MAGISTRATE, IN_PROGRESS);
            break;
    }

    return false;
}

/*######
## go_postbox
######*/

bool GOUse_go_postbox(Player* pPlayer, GameObject* pGo)
{
    instance_stratholme* m_pInstance = (instance_stratholme*)pGo->GetInstanceData();

    if (!m_pInstance)
        return false;

    uint32 m_uiNoBox = m_pInstance->GetData(TYPE_POSTBOX);
    if (m_uiNoBox < 3)
    {
        float fX, fY, fZ;
        pGo->GetPosition(fX, fY, fZ);

        for(uint32 i = 0; i < 3; ++i)
        {
            if (Creature* pPostman = pGo->SummonCreature(NPC_UNDEAD_POSTMAN, fX+irand(-5,5), fY+irand(-5,5), fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 5000))
                pPostman->AI()->AttackStart(pPlayer);
        }

        if (m_uiNoBox == 2)
        {
            if (Creature* pMalown = pGo->SummonCreature(NPC_POSTMASTER_MALOWN, fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 5000))
                pMalown->AI()->AttackStart(pPlayer);
        }
        m_pInstance->SetData(TYPE_POSTBOX, ++m_uiNoBox);
    }

    return false;
}

/*######
## mob_abomination
######*/

enum Spells
{
    SPELL_SLOWING_POISON      = 7992,
    SPELL_VENOM_SPIT          = 16866
};

struct MANGOS_DLL_DECL mob_abominationAI : public ScriptedAI
{
    mob_abominationAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    bool m_bInitialSpawn;
    uint32 m_uiEvadeTimer;
    uint32 m_uiSlowingPoisonTimer;
    uint32 m_uiSummonBileTimer;
    uint32 m_uiVenomSpitTimer;    

    void Reset()
    {
        m_bInitialSpawn = true;
        m_uiEvadeTimer = 0;
        m_uiSlowingPoisonTimer = urand(0,2000);
        m_uiSummonBileTimer = urand(4000,5000);
        m_uiVenomSpitTimer = urand(4000,5000);
    }

    void JustReachedHome()
    {
        m_uiEvadeTimer = 0;
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SLAUGHTER_SQUARE, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_SLAUGHTER_SQUARE) != IN_PROGRESS)
            m_pInstance->SetData(TYPE_SLAUGHTER_SQUARE, IN_PROGRESS);

        // Safety check to deactivate evade timer entering combat
        if (m_uiEvadeTimer)
            m_uiEvadeTimer = 0;
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SLAUGHTER_SQUARE, IN_PROGRESS);

        if (m_creature->GetEntry() == NPC_BILE_SPEWER)
        {
            for(uint8 i = 0; i < 4; ++i)
                DoSpawnCreature(NPC_BILE_SLIME, 0, 0, 0, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    void MovementInform(uint32 /*uiMotionType*/, uint32 uiPointId)
    {
        if (uiPointId == 0)
            m_uiEvadeTimer = 15000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_uiEvadeTimer)
            {
                if (m_uiEvadeTimer <= uiDiff)
                {
                    if (m_pInstance)
                        m_pInstance->SetData(TYPE_SLAUGHTER_SQUARE, NOT_STARTED);

                    m_uiEvadeTimer = 0;
                }
                else
                    m_uiEvadeTimer -= uiDiff;
            }
            return;
        }

        switch(m_creature->GetEntry())
        {
            case NPC_BILE_SPEWER:
                // Summon Bile Slime
                if (m_uiSummonBileTimer <= uiDiff)
                {
                    if (m_bInitialSpawn)
                    {
                        for(uint8 i = 0; i < 3; ++i)
                            DoSpawnCreature(NPC_BILE_SLIME, 0, 0, 0, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
                        
                        m_bInitialSpawn = false;
                    }
                    else
                        DoSpawnCreature(NPC_BILE_SLIME, 0, 0, 0, 0, TEMPSUMMON_DEAD_DESPAWN, 0);

                    m_uiSummonBileTimer = urand(10000,15000);
                }
                else
                    m_uiSummonBileTimer -= uiDiff;
                break;
            case NPC_VENOM_BELCHER:
                // Slowing Poison spell
                if (m_uiSlowingPoisonTimer <= uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SLOWING_POISON);
                    m_uiSlowingPoisonTimer = urand(6000,8000);
                }
                else
                    m_uiSlowingPoisonTimer -= uiDiff;

                // Venom Spit spell
                if (m_uiVenomSpitTimer <= uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_VENOM_SPIT);
                    m_uiVenomSpitTimer = urand(4000,7000);
                }
                else
                    m_uiVenomSpitTimer -= uiDiff;
                break;
            default:
                break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_abomination(Creature* pCreature)
{
    return new mob_abominationAI(pCreature);
}

/*######
## mob_freed_soul
######*/

enum
{
    SAY_ZAPPED0         = -1329013,
    SAY_ZAPPED1         = -1329014,
    SAY_ZAPPED2         = -1329015,
    SAY_ZAPPED3         = -1329016
};

struct MANGOS_DLL_DECL mob_freed_soulAI : public ScriptedAI
{
    mob_freed_soulAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        switch(urand(0,3))
        {
            case 0:
                DoScriptText(SAY_ZAPPED0, m_creature);
                break;
            case 1:
                DoScriptText(SAY_ZAPPED1, m_creature);
                break;
            case 2:
                DoScriptText(SAY_ZAPPED2, m_creature);
                break;
            case 3:
                DoScriptText(SAY_ZAPPED3, m_creature);
                break;
        }
    }
};

CreatureAI* GetAI_mob_freed_soul(Creature* pCreature)
{
    return new mob_freed_soulAI(pCreature);
}

/*######
## mob_restless_soul
######*/

enum
{
    SPELL_EGAN_BLASTER        = 17368,
    SPELL_SOUL_FREED          = 17370,
    QUEST_RESTLESS_SOUL       = 5282,
    NPC_RESTLESS              = 11122,
    NPC_FREED                 = 11136
};

struct MANGOS_DLL_DECL mob_restless_soulAI : public ScriptedAI
{
    mob_restless_soulAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool m_bTagged;
    uint32 m_uiDieTimer;
    ObjectGuid m_uiTagger;

    void Reset()
    {
        m_bTagged = false;
        m_uiDieTimer = 5000;
		m_uiTagger.Clear();
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pCaster->GetTypeId() == TYPEID_PLAYER)
        {
            if (!m_bTagged && pSpell->Id == SPELL_EGAN_BLASTER && ((Player*)pCaster)->GetQuestStatus(QUEST_RESTLESS_SOUL) == QUEST_STATUS_INCOMPLETE)
            {
                m_bTagged = true;
				m_uiTagger = pCaster->GetObjectGuid();
            }
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->CastSpell(pSummoned, SPELL_SOUL_FREED, false);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_bTagged)
            DoSpawnCreature(NPC_FREED, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bTagged)
        {
            if (m_uiDieTimer <= uiDiff)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiTagger))
                    pPlayer->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
            else
                m_uiDieTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_mob_restless_soul(Creature* pCreature)
{
    return new mob_restless_soulAI(pCreature);
}

/*######
## mobs_spectral_ghostly_citizen
######*/

enum
{
    SPELL_HAUNTING_PHANTOM  = 16336,
    SPELL_SLAP              = 6754
};

struct MANGOS_DLL_DECL mobs_spectral_ghostly_citizenAI : public ScriptedAI
{
    mobs_spectral_ghostly_citizenAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool m_bTagged;
    uint32 m_uiDieTimer;

    void Reset()
    {
        m_bTagged = false;
        m_uiDieTimer = 5000;
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell)
    {
        if (!m_bTagged && pSpell->Id == SPELL_EGAN_BLASTER)
            m_bTagged = true;
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_bTagged)
        {
            for(uint32 i = 1; i <= 4; ++i)
            {
                float fX, fY, fZ;
                m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);

                //100%, 50%, 33%, 25% chance to spawn
                if (urand(1,i) == 1)
                    m_creature->SummonCreature(NPC_RESTLESS, fX, fY, fZ, 0, TEMPSUMMON_CORPSE_DESPAWN, 600000);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bTagged)
        {
            if (m_uiDieTimer <= uiDiff)
            {
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            } else m_uiDieTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        switch(emote)
        {
            case TEXTEMOTE_DANCE:
                ResetToHome();
                break;
            case TEXTEMOTE_RUDE:
                if (m_creature->IsWithinDistInMap(pPlayer, INTERACTION_DISTANCE))
                    m_creature->CastSpell(pPlayer, SPELL_SLAP, false);
                else
                    m_creature->HandleEmote(EMOTE_ONESHOT_RUDE);
                break;
            case TEXTEMOTE_WAVE:
                m_creature->HandleEmote(EMOTE_ONESHOT_WAVE);
                break;
            case TEXTEMOTE_BOW:
                m_creature->HandleEmote(EMOTE_ONESHOT_BOW);
                break;
            case TEXTEMOTE_KISS:
                m_creature->HandleEmote(EMOTE_ONESHOT_FLEX);
                break;
        }
    }
};

CreatureAI* GetAI_mobs_spectral_ghostly_citizen(Creature* pCreature)
{
    return new mobs_spectral_ghostly_citizenAI(pCreature);
}

/*######
## mob_the_unforgiven
######*/

enum
{
    SPELL_FROST_NOVA          = 14907
};

struct MANGOS_DLL_DECL mob_the_unforgivenAI : public ScriptedAI
{
    mob_the_unforgivenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    uint32 m_uiFrostNovaTimer;

    void Reset()
    {
        m_uiFrostNovaTimer = urand(2000,4000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THE_UNFORGIVEN, FAIL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THE_UNFORGIVEN, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Frost Nova spell
        if (m_uiFrostNovaTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_NOVA);
            m_uiFrostNovaTimer = urand(4000,8000);
        }
        else
            m_uiFrostNovaTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_the_unforgiven(Creature* pCreature)
{
    return new mob_the_unforgivenAI(pCreature);
}

/*######
## mob_eye_of_naxxramas
######*/

enum
{
	YELL_LIVING					= -1329021,
	SPELL_ROOT_SELF             = 23973,	//taken from rag
    SPELL_STEALTH				= 16380,
	SPELL_SUMMON				= 16381,
	NPC_ROCKWING_GARGOYLE		= 10408,
	
};

struct MANGOS_DLL_DECL mob_eye_of_naxxramasAI : public ScriptedAI
{
    mob_eye_of_naxxramasAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        SetCombatMovement(false);
		Reset();
    }

    instance_stratholme* m_pInstance;

    uint32 m_uiSummonTimer;
	uint32 m_uiChannelTimer;

    void Reset()
    {
		DoCastSpellIfCan(m_creature, SPELL_STEALTH);
		SetCombatMovement(false);
		m_creature->RemoveAurasDueToSpell(SPELL_ROOT_SELF);
        m_uiSummonTimer = 9000;
		m_uiChannelTimer = 500;
    }

	void Aggro(Unit* /*pAttacker*/)
	{
		m_creature->RemoveAurasDueToSpell(SPELL_STEALTH);
		m_creature->CastSpell(m_creature, SPELL_ROOT_SELF, true);
		DoScriptText(YELL_LIVING, m_creature);
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		// Cast timer
        if (m_uiChannelTimer <= uiDiff)
        {
			DoCastSpellIfCan(m_creature, SPELL_SUMMON);			// Fake cast, should never finish casting this spell
		}
		else 
			m_uiChannelTimer -= uiDiff;

        // Summon timer
        if (m_uiSummonTimer <= uiDiff)
        {
			float fX, fY, fZ;
            m_creature->GetPosition(fX, fY, fZ);
			for(uint8 i = 0; i < 2; ++i)
                    if (Creature* pGargoyle = m_creature->SummonCreature(NPC_ROCKWING_GARGOYLE, fX+irand(-3,3), fY+irand(-3,3), fZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000))
                        pGargoyle->SetInCombatWithZone();
						
			m_creature->ForcedDespawn();
            m_uiSummonTimer = 900000;
        }
        else
            m_uiSummonTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_eye_of_naxxramas(Creature* pCreature)
{
    return new mob_eye_of_naxxramasAI(pCreature);
}

/*######
## npc_aurius
######*/

enum eAurius
{
    QUEST_THE_MEDALLION_OF_FAITH  = 5122,

    SPELL_CRUSADER_STRIKE         = 14517,
    SPELL_DIVINE_SHIELD           = 13874,
    SPELL_EXORCISM                = 17149,
    SPELL_HOLY_LIGHT              = 13952,
};

struct MANGOS_DLL_DECL npc_auriusAI : public ScriptedAI
{
    npc_auriusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiCrusaderStrikeTimer;
    uint32 m_uiDivineShieldTimer;
    uint32 m_uiExorcismTimer;
    uint32 m_uiHolyLightTimer;
    uint32 m_uiLeaveTimer;

    void Reset()
    {
        m_uiCrusaderStrikeTimer = urand(5000,8000);
        m_uiDivineShieldTimer = urand(30000,40000);
        m_uiExorcismTimer = urand(12000,16000);
        m_uiHolyLightTimer = urand(20000,30000);
        m_uiLeaveTimer = 0;
    }

    void Leave()
    {
        m_uiLeaveTimer = 45000;
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId)
    {
        if (uiMotionType == POINT_MOTION_TYPE && uiPointId == 0)
            m_creature->RemoveFromWorld();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiLeaveTimer)
        {
            if (m_uiLeaveTimer <= uiDiff)
            {
                m_uiLeaveTimer = 0;
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                m_creature->GetMotionMaster()->MovePoint(0, 3666.92f, -3624.59f, 139.958f);
            }
            else
                m_uiLeaveTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Crusader Strike spell
        if (m_uiCrusaderStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CRUSADER_STRIKE);
            m_uiCrusaderStrikeTimer = urand(6000,12000);
        }
        else
            m_uiCrusaderStrikeTimer -= uiDiff;

        // Divine Shield spell
        if (m_uiDivineShieldTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_DIVINE_SHIELD);
            m_uiDivineShieldTimer = urand(30000,60000);
        }
        else
            m_uiDivineShieldTimer -= uiDiff;

        // Exorcism spell
        if (m_uiExorcismTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_EXORCISM);
            m_uiExorcismTimer = urand(10000,15000);
        }
        else
            m_uiExorcismTimer -= uiDiff;

        // Holy Light spell
        if (m_uiHolyLightTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectRandomFriendlyTarget(NULL, 30.0f);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature, SPELL_HOLY_LIGHT);
            m_uiHolyLightTimer = urand(15000,30000);
        }
        else
            m_uiHolyLightTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_aurius(Creature* pCreature)
{
    return new npc_auriusAI(pCreature);
}

bool QuestRewarded_npc_aurius(Player* /*pPlayer*/, Creature* pCreature, const Quest* pQuest)
{
    instance_stratholme* m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
    
    if (m_pInstance && m_pInstance->GetData(TYPE_AURIUS) != DONE && pQuest->GetQuestId() == QUEST_THE_MEDALLION_OF_FAITH)
    {
        m_pInstance->SetData(TYPE_AURIUS, DONE);
        if (npc_auriusAI* pAuriusAI = dynamic_cast<npc_auriusAI*>(pCreature->AI()))
            pAuriusAI->Leave();
    }
    return true;
}

/*######
## npc_stratholme_trigger         (not 100% blizzlike, but currently using for some mini-events)
######*/

const uint32 aTrapMob[3] = {NPC_PLAGUED_INSECT, NPC_PLAGUED_MAGGOT, NPC_PLAGUED_RAT};

static Locations Trigger[]=
{
    {0, 0, 0, 0, 0},                                                                  // Default blank
    {3612.33f, -3335.29f, 124.06f, GO_PORT_TRAP_SQUARE1, GO_PORT_TRAP_SQUARE2},       // Square trap
    {3920.06f, -3547.07f, 134.21f, GO_PORT_TRAP_GAUNTLET1, GO_PORT_TRAP_GAUNTLET2},   // Gauntlet trap
    {3712.44f, -3428.50f, 131.06f, TYPE_THE_UNFORGIVEN, 0},                           // The Unforgiven passage
};

struct MANGOS_DLL_DECL npc_stratholme_triggerAI : public Scripted_NoMovementAI
{
    npc_stratholme_triggerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    uint32 m_uiNo;
    uint32 m_uiRecoveryTimer;
    int m_uiSummonCount;
    uint32 m_uiSummonEntry;

    void Reset()
    {
        m_uiNo = 0;

        for(uint32 i = 0; i < sizeof(Trigger)/sizeof(Locations); ++i)
        {
            if (m_creature->GetDistance2d(Trigger[i].x, Trigger[i].y) < 5.0f)
            {
                m_uiNo = i;
                break;
            }
        }

        m_uiRecoveryTimer = 0;
        m_uiSummonCount = 0;
        m_uiSummonEntry = 0;
    }

    void JustSummoned(Creature* /*pSummoned*/)
    {
        ++m_uiSummonCount;
    }

    void SummonedCreatureDespawn(Creature* /*pDespawned*/)
    {
        --m_uiSummonCount;

        if (m_uiSummonCount == 0)
        {
            switch(m_uiNo)
            {
                case 1:
                case 2:
                    m_uiRecoveryTimer = urand(4000,6000);

                    if (GameObject* pBar1 = GetClosestGameObjectWithEntry(m_creature, Trigger[m_uiNo].add1, 15.0f))
                        pBar1->SetGoState(GO_STATE_ACTIVE);
                    if (GameObject* pBar2 = GetClosestGameObjectWithEntry(m_creature, Trigger[m_uiNo].add2, 15.0f))
                        pBar2->SetGoState(GO_STATE_ACTIVE); 
                    break;
                case 3:
                    m_uiRecoveryTimer = urand(5400000,9000000);
                    break;
                default:
                    break;
            }
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER || ((Player*)pWho)->isGameMaster())
            return;

        if (m_uiSummonCount != 0 || m_uiRecoveryTimer != 0)
            return;

        if (m_creature->IsWithinDistInMap(pWho, m_uiNo > 2 ? 15.0f : ATTACK_DISTANCE))
        {
            float fX, fY, fZ;
            m_creature->GetPosition(fX, fY, fZ);

            switch(m_uiNo)
            {
                case 1:
                case 2:
                    m_uiSummonEntry = aTrapMob[urand(0,2)];
                    for(uint32 i = 0; i < 40; ++i)
                    {
                        if (Creature* pMob = m_creature->SummonCreature(m_uiSummonEntry, fX+irand(-4,4), fY+irand(-4,4), fZ+1, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000))
                            pMob->AI()->AttackStart(pWho);
                    }

                    if (GameObject* pBar1 = GetClosestGameObjectWithEntry(m_creature, Trigger[m_uiNo].add1, 15.0f))
                        pBar1->SetGoState(GO_STATE_READY);
                    if (GameObject* pBar2 = GetClosestGameObjectWithEntry(m_creature, Trigger[m_uiNo].add2, 15.0f))
                        pBar2->SetGoState(GO_STATE_READY);
                    break;
                case 3:
                    if (!m_pInstance || m_pInstance->GetData(Trigger[m_uiNo].add1) != NOT_STARTED)
                        return;

                    for(uint8 i = 0; i < 4; ++i)
                    {
                        if (Creature* pPhantom = m_creature->SummonCreature(NPC_VENGEFUL_PHANTOM, fX+irand(-10,10), fY+irand(-10,10), fZ+1, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000))
                            pPhantom->AI()->AttackStart(pWho);
                    }

                    if (Creature* pUnforgiven = m_creature->SummonCreature(NPC_THE_UNFORGIVEN, fX+irand(-10,10), fY+irand(-10,10), fZ+1, 0, TEMPSUMMON_DEAD_DESPAWN, 5000))
                        pUnforgiven->AI()->AttackStart(pWho);

                    m_pInstance->SetData(Trigger[m_uiNo].add1, IN_PROGRESS);
                    break;
                default:
                    break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiRecoveryTimer)
        {
            if (m_uiRecoveryTimer <= uiDiff)
            {
                m_uiRecoveryTimer = 0;
                if (m_pInstance && m_uiNo == 2)
                    m_pInstance->SetData(Trigger[m_uiNo].add1, NOT_STARTED);
            }
            else
                m_uiRecoveryTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_stratholme_trigger(Creature* pCreature)
{
    return new npc_stratholme_triggerAI(pCreature);
}

void AddSC_stratholme()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "go_blacksmithing_plans";
    pNewscript->pGOUse = &GOUse_go_blacksmithing_plans;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_postbox";
    pNewscript->pGOUse = &GOUse_go_postbox;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_stratholme_gate";
    pNewscript->pGOUse = &GOUse_go_stratholme_gate;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_abomination";
    pNewscript->GetAI = &GetAI_mob_abomination;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_freed_soul";
    pNewscript->GetAI = &GetAI_mob_freed_soul;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_restless_soul";
    pNewscript->GetAI = &GetAI_mob_restless_soul;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mobs_spectral_ghostly_citizen";
    pNewscript->GetAI = &GetAI_mobs_spectral_ghostly_citizen;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "mob_the_unforgiven";
    pNewscript->GetAI = &GetAI_mob_the_unforgiven;
    pNewscript->RegisterSelf();

	 pNewscript = new Script;
    pNewscript->Name = "mob_eye_of_naxxramas";
    pNewscript->GetAI = &GetAI_mob_eye_of_naxxramas;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_aurius";
    pNewscript->GetAI = &GetAI_npc_aurius;
    pNewscript->pQuestRewardedNPC = &QuestRewarded_npc_aurius;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_stratholme_trigger";
    pNewscript->GetAI = &GetAI_npc_stratholme_trigger;
    pNewscript->RegisterSelf();
}
