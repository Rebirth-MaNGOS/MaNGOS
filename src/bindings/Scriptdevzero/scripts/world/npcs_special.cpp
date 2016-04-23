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
SDName: Npcs_Special
SD%Complete: 100
SDComment: To be used for special NPCs that are located globally.
SDCategory: NPCs
EndScriptData
*/

#include "precompiled.h"
#include "escort_ai.h"
#include "follower_ai.h"
#include "ObjectMgr.h"
#include "GameEventMgr.h"

/* ContentData
npc_chicken_cluck           100%    support for quest 3861 (Cluck!)
npc_guardian                100%    guardianAI used to prevent players from accessing off-limits areas. Not in use by SD2
npc_garments_of_quests      80%     NPC's related to all Garments of-quests 5621, 5624, 5625, 5648, 5650
npc_injured_patient         100%    patients for triage-quests (6622 and 6624)
npc_doctor                  100%    Gustaf Vanhowzen and Gregory Victor, quest 6622 and 6624 (Triage)
npc_lunaclaw_spirit         100%    Appears at two different locations, quest 6001/6002
npc_mount_vendor            100%    Regular mount vendors all over the world. Display gossip if player doesn't meet the requirements to buy
npc_rogue_trainer           80%     Scripted trainers, so they are able to offer item 17126 for class quest 6681
npc_sayge                   100%    Darkmoon event fortune teller, buff player based on answers given
mob_deeprun_rat             100%
mob_shahram                 100%
npc_darrowshire_poltergeist 100%
mob_the_cleaner             90%     The Cleaner spawns when someone besides the hunter attacks Simone the Seductress during the level 60 hunter quest Stave of the Ancients
mob_mangy_wolf              100%    Classic Wolf, but for quest Garments of the Light is needed to ignore aggro of Guard Roberts
EndContentData */


/*########
# npc_chicken_cluck
#########*/

enum
{
    EMOTE_A_HELLO           = -1000204,
    EMOTE_H_HELLO           = -1000205,
    EMOTE_CLUCK_TEXT2       = -1000206,

    QUEST_CLUCK             = 3861,
    FACTION_FRIENDLY        = 35,
    FACTION_CHICKEN         = 31
};

struct MANGOS_DLL_DECL npc_chicken_cluckAI : public ScriptedAI
{
    npc_chicken_cluckAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiResetFlagTimer;

    void Reset()
    {
        m_uiResetFlagTimer = 20000;

        m_creature->setFaction(FACTION_CHICKEN);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER + UNIT_NPC_FLAG_GOSSIP);
    }

    void ReceiveEmote(Player* pPlayer, uint32 uiEmote)
    {
        if (uiEmote == TEXTEMOTE_CHICKEN && m_creature->isAlive())
        {
            if (!urand(0, 29))
            {
                if (pPlayer->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_NONE)
                {
                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER + UNIT_NPC_FLAG_GOSSIP);
                    m_creature->setFaction(FACTION_FRIENDLY);

                    DoScriptText(EMOTE_A_HELLO, m_creature);

                    /* are there any difference in texts, after 3.x ?
                    if (pPlayer->GetTeam() == HORDE)
                        DoScriptText(EMOTE_H_HELLO, m_creature);
                    else
                        DoScriptText(EMOTE_A_HELLO, m_creature);
                    */
                }
            }
        }

		if (uiEmote == TEXTEMOTE_CHEER && m_creature->isAlive())
        {
            if (pPlayer->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_COMPLETE)
            {
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER + UNIT_NPC_FLAG_GOSSIP);
                m_creature->setFaction(FACTION_FRIENDLY);
                DoScriptText(EMOTE_CLUCK_TEXT2, m_creature);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Reset flags after a certain time has passed so that the next player has to start the 'event' again
        if (m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER + UNIT_NPC_FLAG_GOSSIP))
        {
            if (m_uiResetFlagTimer < uiDiff)
                ResetToHome();
            else
                m_uiResetFlagTimer -= uiDiff;
        }

        if (m_creature->SelectHostileTarget() && m_creature->getVictim())
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_chicken_cluck(Creature* pCreature)
{
    return new npc_chicken_cluckAI(pCreature);
}

bool QuestAccept_npc_chicken_cluck(Player* /*pPlayer*/, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CLUCK)
    {
        if (npc_chicken_cluckAI* pChickenAI = dynamic_cast<npc_chicken_cluckAI*>(pCreature->AI()))
            pChickenAI->Reset();
    }

    return true;
}

bool QuestRewarded_npc_chicken_cluck(Player* /*pPlayer*/, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CLUCK)
    {
        if (npc_chicken_cluckAI* pChickenAI = dynamic_cast<npc_chicken_cluckAI*>(pCreature->AI()))
            pChickenAI->Reset();
    }

    return true;
}

/*######
## Triage quest
######*/

enum eDoctors
{
    SAY_DOC1                            = -1000201,
    SAY_DOC2                            = -1000202,
    SAY_DOC3                            = -1000203,

    QUEST_TRIAGE_H                      = 6622,
    QUEST_TRIAGE_A                      = 6624,

    DOCTOR_ALLIANCE                     = 12939,
    DOCTOR_HORDE                        = 12920,
    ALLIANCE_COORDS                     = 7,
    HORDE_COORDS                        = 6,

    SPELL_HEAVY_MAGEWEAVE_BANDAGE       = 10841,
    SPELL_RUNECLOTH_BANDAGE             = 18629,
    SPELL_HEAVY_RUNECLOTH_BANDAGE       = 18630,
    SPELL_LEARN_HEAVY_MAGEWEAVE_BANDAGE = 10843,
    SPELL_LEARN_RUNECLOTH_BANDAGE       = 18631,
    SPELL_LEARN_HEAVY_RUNECLOTH_BANDAGE = 18632,
};

struct Location
{
    float x, y, z, o;
};

static Location AllianceCoords[]=
{
    {-3757.38f, -4533.05f, 14.16f, 3.62f},                      // Top-far-right bunk as seen from entrance
    {-3754.36f, -4539.13f, 14.16f, 5.13f},                      // Top-far-left bunk
    {-3749.54f, -4540.25f, 14.28f, 3.34f},                      // Far-right bunk
    {-3742.10f, -4536.85f, 14.28f, 3.64f},                      // Right bunk near entrance
    {-3755.89f, -4529.07f, 14.05f, 0.57f},                      // Far-left bunk
    {-3749.51f, -4527.08f, 14.07f, 5.26f},                      // Mid-left bunk
    {-3746.37f, -4525.35f, 14.16f, 5.22f},                      // Left bunk near entrance
};

//alliance run to where
#define A_RUNTOX -3742.96f
#define A_RUNTOY -4531.52f
#define A_RUNTOZ 11.91f

static Location HordeCoords[]=
{
    {-1013.75f, -3492.59f, 62.62f, 4.34f},                      // Left, Behind
    {-1017.72f, -3490.92f, 62.62f, 4.34f},                      // Right, Behind
    {-1015.77f, -3497.15f, 62.82f, 4.34f},                      // Left, Mid
    {-1019.51f, -3495.49f, 62.82f, 4.34f},                      // Right, Mid
    {-1017.25f, -3500.85f, 62.98f, 4.34f},                      // Left, front
    {-1020.95f, -3499.21f, 62.98f, 4.34f}                       // Right, Front
};

//horde run to where
#define H_RUNTOX -1016.44f
#define H_RUNTOY -3508.48f
#define H_RUNTOZ 62.96f

const uint32 AllianceSoldierId[3] =
{
    12938,                                                  // 12938 Injured Alliance Soldier
    12936,                                                  // 12936 Badly injured Alliance Soldier
    12937                                                   // 12937 Critically injured Alliance Soldier
};

const uint32 HordeSoldierId[3] =
{
    12923,                                                  // 12923 Injured Soldier
    12924,                                                  // 12924 Badly injured Soldier
    12925                                                   // 12925 Critically injured Soldier
};

/*######
## npc_doctor (handles both Gustaf Vanhowzen and Gregory Victor)
######*/

bool GossipHello_npc_doctor(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->HasSkill(SKILL_FIRST_AID))
    {
        if (pPlayer->GetSkillValue(SKILL_FIRST_AID) >= 240 && !pPlayer->HasSpell(SPELL_HEAVY_MAGEWEAVE_BANDAGE))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me Heavy Mageweave Bandage", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if (pPlayer->GetSkillValue(SKILL_FIRST_AID) >= 260 && !pPlayer->HasSpell(SPELL_RUNECLOTH_BANDAGE))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me Runecloth Bandage", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        if (pPlayer->GetSkillValue(SKILL_FIRST_AID) >= 290 && !pPlayer->HasSpell(SPELL_HEAVY_RUNECLOTH_BANDAGE))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Teach me Heave Runecloth Bandage", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_doctor(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pCreature->CastSpell(pPlayer, SPELL_LEARN_HEAVY_MAGEWEAVE_BANDAGE, false);
            break;    
        case GOSSIP_ACTION_INFO_DEF + 2:
            pCreature->CastSpell(pPlayer, SPELL_LEARN_RUNECLOTH_BANDAGE, false);
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pCreature->CastSpell(pPlayer, SPELL_LEARN_HEAVY_RUNECLOTH_BANDAGE, false);
            break;
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

struct MANGOS_DLL_DECL npc_doctorAI : public ScriptedAI
{
    npc_doctorAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    ObjectGuid Playerguid;

    uint32 SummonPatient_Timer;
    uint32 SummonPatientCount;
    uint32 PatientDiedCount;
    uint32 PatientSavedCount;

    bool Event;

    GUIDList Patients;
    std::vector<Location*> Coordinates;

    void Reset()
    {
		Playerguid.Clear();

        SummonPatient_Timer = 10000;
        SummonPatientCount = 0;
        PatientDiedCount = 0;
        PatientSavedCount = 0;

        Patients.clear();
        Coordinates.clear();

        Event = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void BeginEvent(Player* pPlayer);
    void PatientDied(Location* Point);
    void PatientSaved(Creature* soldier, Player* pPlayer, Location* Point);
    void UpdateAI(const uint32 diff);
};

/*#####
## npc_injured_patient (handles all the patients, no matter Horde or Alliance)
#####*/

struct MANGOS_DLL_DECL npc_injured_patientAI : public ScriptedAI
{
    npc_injured_patientAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    ObjectGuid Doctorguid;
    Location* Coord;

    void Reset()
    {
		Doctorguid.Clear();
        Coord = NULL;

        //no select
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        //no regen health
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
        //to make them lay with face down
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        uint32 mobId = m_creature->GetEntry();

        switch (mobId)
        {                                                   //lower max health
            case 12923:
            case 12938:                                     //Injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.75));
                break;
            case 12924:
            case 12936:                                     //Badly injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.50));
                break;
            case 12925:
            case 12937:                                     //Critically injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.25));
                break;
        }
    }

    void MoveInLineOfSight(Unit* /*pWho*/)
    {
        // empty == ignore aggro
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER && m_creature->isAlive() && spell->Id == 20804)
        {
            if ((((Player*)caster)->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (((Player*)caster)->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
            {
                if (Doctorguid)
                {
                    if (Creature* pDoctor = m_creature->GetMap()->GetCreature(Doctorguid))
                    {
                        if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pDoctor->AI()))
                            pDocAI->PatientSaved(m_creature, (Player*)caster, Coord);
                    }
                }
            }
            //make not selectable
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            //regen health
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            //stand up
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            switch(urand(0, 2))
            {
                case 0: DoScriptText(SAY_DOC1,m_creature); break;
                case 1: DoScriptText(SAY_DOC2,m_creature); break;
                case 2: DoScriptText(SAY_DOC3,m_creature); break;
            }

            m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);

            uint32 mobId = m_creature->GetEntry();

            switch (mobId)
            {
                case 12923:
                case 12924:
                case 12925:
                    m_creature->GetMotionMaster()->MovePoint(0, H_RUNTOX, H_RUNTOY, H_RUNTOZ);
                    break;
                case 12936:
                case 12937:
                case 12938:
                    m_creature->GetMotionMaster()->MovePoint(0, A_RUNTOX, A_RUNTOY, A_RUNTOZ);
                    break;
            }
        }
    }

    void UpdateAI(const uint32 /*diff*/)
    {
        //lower HP on every world tick makes it a useful counter, not officlone though
        if (m_creature->isAlive() && m_creature->GetHealth() > 6)
        {
            m_creature->SetHealth(uint32(m_creature->GetHealth()-5));
        }

        if (m_creature->isAlive() && m_creature->GetHealth() <= 6)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetDeathState(JUST_DIED);
            m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);

            if (Doctorguid)
            {
                if (Creature* pDoctor = m_creature->GetMap()->GetCreature(Doctorguid))
                {
                    if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pDoctor->AI()))
                        pDocAI->PatientDied(Coord);
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_injured_patient(Creature* pCreature)
{
    return new npc_injured_patientAI(pCreature);
}

/*
npc_doctor (continue)
*/

void npc_doctorAI::BeginEvent(Player* pPlayer)
{
	Playerguid = pPlayer->GetObjectGuid();

    SummonPatient_Timer = 10000;
    SummonPatientCount = 0;
    PatientDiedCount = 0;
    PatientSavedCount = 0;

    switch(m_creature->GetEntry())
    {
        case DOCTOR_ALLIANCE:
            for(uint8 i = 0; i < ALLIANCE_COORDS; ++i)
                Coordinates.push_back(&AllianceCoords[i]);
            break;
        case DOCTOR_HORDE:
            for(uint8 i = 0; i < HORDE_COORDS; ++i)
                Coordinates.push_back(&HordeCoords[i]);
            break;
    }

    Event = true;
    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
}

void npc_doctorAI::PatientDied(Location* Point)
{
    Player* pPlayer = m_creature->GetMap()->GetPlayer(Playerguid);

    if (pPlayer && ((pPlayer->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)))
    {
        ++PatientDiedCount;

        if (PatientDiedCount > 5 && Event)
        {
            if (pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(QUEST_TRIAGE_A);
            else if (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(QUEST_TRIAGE_H);

            Reset();
            return;
        }

        Coordinates.push_back(Point);
    }
    else
        // If no player or player abandon quest in progress
        Reset();
}

void npc_doctorAI::PatientSaved(Creature* /*soldier*/, Player* pPlayer, Location* Point)
{
    if (pPlayer && Playerguid == pPlayer->GetObjectGuid())
    {
        if ((pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE))
        {
            ++PatientSavedCount;

            if (PatientSavedCount == 15)
            {
                if (!Patients.empty())
                {
                    GUIDList::iterator itr;
                    for(itr = Patients.begin(); itr != Patients.end(); ++itr)
                    {
                        if (Creature* Patient = m_creature->GetMap()->GetCreature(*itr))
                            Patient->SetDeathState(JUST_DIED);
                    }
                }

                if (pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->GroupEventHappens(QUEST_TRIAGE_A, m_creature);
                else if (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->GroupEventHappens(QUEST_TRIAGE_H, m_creature);

                Reset();
                return;
            }

            Coordinates.push_back(Point);
        }
    }
}

void npc_doctorAI::UpdateAI(const uint32 diff)
{
    if (Event && SummonPatientCount >= 20)
    {
        Reset();
        return;
    }

    if (Event)
    {
        if (SummonPatient_Timer < diff)
        {
            Creature* Patient = NULL;
            Location* Point = NULL;

            if (Coordinates.empty())
                return;

            std::vector<Location*>::iterator itr = Coordinates.begin()+rand()%Coordinates.size();
            uint32 patientEntry = 0;

            switch(m_creature->GetEntry())
            {
                case DOCTOR_ALLIANCE: patientEntry = AllianceSoldierId[urand(0, 2)]; break;
                case DOCTOR_HORDE:    patientEntry = HordeSoldierId[urand(0, 2)]; break;
                default:
                    error_log("SD2: Invalid entry for Triage doctor. Please check your database");
                    return;
            }

            Point = *itr;

            Patient = m_creature->SummonCreature(patientEntry, Point->x, Point->y, Point->z, Point->o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);

            if (Patient)
            {
                
                Patients.push_back(Patient->GetObjectGuid());
                npc_injured_patientAI* pPatientAI = dynamic_cast<npc_injured_patientAI*>(Patient->AI());

                if (pPatientAI)
                {
                    pPatientAI->Doctorguid = m_creature->GetObjectGuid();

                    if (Point)
                        pPatientAI->Coord = Point;
                }

                Coordinates.erase(itr);
            }
            SummonPatient_Timer = 10000;
            ++SummonPatientCount;
        }else SummonPatient_Timer -= diff;
    }
}

bool QuestAccept_npc_doctor(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if ((pQuest->GetQuestId() == QUEST_TRIAGE_A) || (pQuest->GetQuestId() == QUEST_TRIAGE_H))
    {
        if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pCreature->AI()))
            pDocAI->BeginEvent(pPlayer);
    }

    return true;
}

CreatureAI* GetAI_npc_doctor(Creature* pCreature)
{
    return new npc_doctorAI(pCreature);
}

/*######
## npc_garments_of_quests
######*/

//TODO: get text for each NPC

enum
{
    SPELL_LESSER_HEAL_R2    = 2052,
    SPELL_FORTITUDE_R1      = 1243,

    QUEST_MOON              = 5621,
    QUEST_LIGHT_1           = 5624,
    QUEST_LIGHT_2           = 5625,
    QUEST_SPIRIT            = 5648,
    QUEST_DARKNESS          = 5650,

    ENTRY_SHAYA             = 12429,
    ENTRY_ROBERTS           = 12423,
    ENTRY_DOLF              = 12427,
    ENTRY_KORJA             = 12430,
    ENTRY_DG_KEL            = 12428,

    SAY_COMMON_HEALED       = -1000231,
    SAY_DG_KEL_THANKS       = -1000232,
    SAY_DG_KEL_GOODBYE      = -1000233,
    SAY_ROBERTS_THANKS      = -1000256,
    SAY_ROBERTS_GOODBYE     = -1000257,
    SAY_KORJA_THANKS        = -1000258,
    SAY_KORJA_GOODBYE       = -1000259,
    SAY_DOLF_THANKS         = -1000260,
    SAY_DOLF_GOODBYE        = -1000261,
    SAY_SHAYA_THANKS        = -1000262,
    SAY_SHAYA_GOODBYE       = -1000263,
};

struct MANGOS_DLL_DECL npc_garments_of_questsAI : public npc_escortAI
{
    npc_garments_of_questsAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

    ObjectGuid caster;

    bool bIsHealed;
    bool bCanRun;

    uint32 RunAwayTimer;

    void Reset()
    {
		caster.Clear();

        bIsHealed = false;
        bCanRun = false;

        RunAwayTimer = 5000;

        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
        //expect database to have RegenHealth=0
        m_creature->SetHealth(int(m_creature->GetMaxHealth()*0.7));
    }

    void SpellHit(Unit* pCaster, const SpellEntry *Spell)
    {
        if (Spell->Id == SPELL_LESSER_HEAL_R2 || Spell->Id == SPELL_FORTITUDE_R1)
        {
            //not while in combat
            if (m_creature->isInCombat())
                return;

            //nothing to be done now
            if (bIsHealed && bCanRun)
                return;

            if (pCaster->GetTypeId() == TYPEID_PLAYER)
            {
                switch(m_creature->GetEntry())
                {
                    case ENTRY_SHAYA:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_MOON) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_SHAYA_THANKS,m_creature,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,m_creature,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_ROBERTS:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_LIGHT_1) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_ROBERTS_THANKS,m_creature,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,m_creature,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DOLF:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_LIGHT_2) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DOLF_THANKS,m_creature,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,m_creature,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_KORJA:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_SPIRIT) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_KORJA_THANKS,m_creature,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,m_creature,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DG_KEL:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_DARKNESS) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DG_KEL_THANKS,m_creature,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,m_creature,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                }

                //give quest credit, not expect any special quest objectives
                if (bCanRun)
                    ((Player*)pCaster)->TalkedToCreature(m_creature->GetEntry(),m_creature->GetObjectGuid());
            }
        }
    }

    void WaypointReached(uint32 /*uiPoint*/)
    {
    }

    void MoveInLineOfSight(Unit* /*pWho*/)
    {
        // Must to be empty to ignore aggro
    }

    void UpdateEscortAI(const uint32 diff)
    {
        if (bCanRun && !m_creature->isInCombat())
        {
            if (RunAwayTimer <= diff)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(caster))
                {
                    switch(m_creature->GetEntry())
                    {
                        case ENTRY_SHAYA: DoScriptText(SAY_SHAYA_GOODBYE,m_creature,pPlayer); break;
                        case ENTRY_ROBERTS: DoScriptText(SAY_ROBERTS_GOODBYE,m_creature,pPlayer); break;
                        case ENTRY_DOLF: DoScriptText(SAY_DOLF_GOODBYE,m_creature,pPlayer); break;
                        case ENTRY_KORJA: DoScriptText(SAY_KORJA_GOODBYE,m_creature,pPlayer); break;
                        case ENTRY_DG_KEL: DoScriptText(SAY_DG_KEL_GOODBYE,m_creature,pPlayer); break;
                    }

                    Start(true);
                }
                else
                    ResetToHome();                       //something went wrong

                RunAwayTimer = 30000;
            }else RunAwayTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_garments_of_quests(Creature* pCreature)
{
    return new npc_garments_of_questsAI(pCreature);
}

/*######
## npc_guardian
######*/

#define SPELL_DEATHTOUCH                5

struct MANGOS_DLL_DECL npc_guardianAI : public ScriptedAI
{
    npc_guardianAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void UpdateAI(const uint32 /*diff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->isAttackReady())
        {
            m_creature->CastSpell(m_creature->getVictim(),SPELL_DEATHTOUCH, true);
            m_creature->resetAttackTimer();
        }
    }
};

CreatureAI* GetAI_npc_guardian(Creature* pCreature)
{
    return new npc_guardianAI(pCreature);
}

/*######
## npc_lunaclaw
######*/

#define SPELL_TRASH                 3391
#define CREATURE_LUNACLAW_SPIRIT    12144

struct MANGOS_DLL_DECL npc_lunaclawAI : public ScriptedAI
{
    npc_lunaclawAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}
    
    uint32 Trash_Timer;
    
    void Reset()
    {
        Trash_Timer = urand(1000,2000);
    }

    void Aggro(Unit* /*who*/)
    {
    }
    
    void JustDied (Unit* killer)
    {
		killer->SummonCreature(CREATURE_LUNACLAW_SPIRIT, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        //Trash spell
        if (Trash_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_TRASH);
            Trash_Timer = urand(4000,5000);
        } else Trash_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_lunaclaw(Creature *pCreature)
{
    return new npc_lunaclawAI (pCreature);
}

/*######
## npc_lunaclaw_spirit
######*/

enum
{
    QUEST_BODY_HEART_A      = 6001,
    QUEST_BODY_HEART_H      = 6002,

    TEXT_ID_DEFAULT         = 4714,
    TEXT_ID_PROGRESS        = 4715
};

#define GOSSIP_ITEM_GRANT   "You have fought well, spirit. I ask you to grant me the strength of your body and the strength of your heart."

bool GossipHello_npc_lunaclaw_spirit(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_BODY_HEART_A) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_BODY_HEART_H) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_DEFAULT, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_lunaclaw_spirit(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_PROGRESS, pCreature->GetObjectGuid());
        pPlayer->AreaExploredOrEventHappens((pPlayer->GetTeam() == ALLIANCE) ? QUEST_BODY_HEART_A : QUEST_BODY_HEART_H);
    }
    return true;
}

/*######
## npc_mount_vendor
######*/

bool GossipHello_npc_mount_vendor(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    bool canBuy;
    canBuy = false;
    uint32 vendor = pCreature->GetEntry();
    uint8 race = pPlayer->getRace();

    switch (vendor)
    {
        case 384:                                           //Katie Hunter
        case 1460:                                          //Unger Statforth
        case 2357:                                          //Merideth Carlson
        case 4885:                                          //Gregor MacVince
            if (pPlayer->GetReputationRank(72) != REP_EXALTED && race != RACE_HUMAN)
                pPlayer->SEND_GOSSIP_MENU(5855, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 1261:                                          //Veron Amberstill
            if (pPlayer->GetReputationRank(47) != REP_EXALTED && race != RACE_DWARF)
                pPlayer->SEND_GOSSIP_MENU(5856, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 3362:                                          //Ogunaro Wolfrunner
            if (pPlayer->GetReputationRank(76) != REP_EXALTED && race != RACE_ORC)
                pPlayer->SEND_GOSSIP_MENU(5841, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 3685:                                          //Harb Clawhoof
            if (pPlayer->GetReputationRank(81) != REP_EXALTED && race != RACE_TAUREN)
                pPlayer->SEND_GOSSIP_MENU(5843, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 4730:                                          //Lelanai
            if (pPlayer->GetReputationRank(69) != REP_EXALTED && race != RACE_NIGHTELF)
                pPlayer->SEND_GOSSIP_MENU(5844, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 4731:                                          //Zachariah Post
            if (pPlayer->GetReputationRank(68) != REP_EXALTED && race != RACE_UNDEAD)
                pPlayer->SEND_GOSSIP_MENU(5840, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 7952:                                          //Zjolnir
            if (pPlayer->GetReputationRank(530) != REP_EXALTED && race != RACE_TROLL)
                pPlayer->SEND_GOSSIP_MENU(5842, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 7955:                                          //Milli Featherwhistle
            if (pPlayer->GetReputationRank(54) != REP_EXALTED && race != RACE_GNOME)
                pPlayer->SEND_GOSSIP_MENU(5857, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
    }

    if (canBuy)
    {
        if (pCreature->isVendor())
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_npc_mount_vendor(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());

    return true;
}

/*######
## npc_rogue_trainer
######*/

bool GossipHello_npc_rogue_trainer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pCreature->isTrainer())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

    if (pCreature->CanTrainAndResetTalentsOf(pPlayer))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "I wish to unlearn my talents", GOSSIP_SENDER_MAIN, GOSSIP_OPTION_UNLEARNTALENTS);

    if (pPlayer->getClass() == CLASS_ROGUE && pPlayer->getLevel() >= 24 && !pPlayer->HasItemCount(17126,1) && !pPlayer->GetQuestRewardStatus(6681))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<Take the letter>", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(5996, pCreature->GetObjectGuid());
    } else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_rogue_trainer(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer,21100,false);
            break;
        case GOSSIP_ACTION_TRAIN:
            pPlayer->SEND_TRAINERLIST(pCreature->GetObjectGuid());
            break;
        case GOSSIP_OPTION_UNLEARNTALENTS:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->SendTalentWipeConfirm(pCreature->GetObjectGuid());
            break;
    }
    return true;
}

/*######
## npc_sayge
######*/

#define SPELL_DMG       23768                               //dmg
#define SPELL_RES       23769                               //res
#define SPELL_ARM       23767                               //arm
#define SPELL_SPI       23738                               //spi
#define SPELL_INT       23766                               //int
#define SPELL_STM       23737                               //stm
#define SPELL_STR       23735                               //str
#define SPELL_AGI       23736                               //agi
#define SPELL_FORTUNE   23765                               //faire fortune

bool GossipHello_npc_sayge(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->HasSpellCooldown(SPELL_INT) ||
        pPlayer->HasSpellCooldown(SPELL_ARM) ||
        pPlayer->HasSpellCooldown(SPELL_DMG) ||
        pPlayer->HasSpellCooldown(SPELL_RES) ||
        pPlayer->HasSpellCooldown(SPELL_STR) ||
        pPlayer->HasSpellCooldown(SPELL_AGI) ||
        pPlayer->HasSpellCooldown(SPELL_STM) ||
        pPlayer->HasSpellCooldown(SPELL_SPI))
        pPlayer->SEND_GOSSIP_MENU(7393, pCreature->GetObjectGuid());
    else
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Yes", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(7339, pCreature->GetObjectGuid());
    }

    return true;
}

void SendAction_npc_sayge(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Slay the Man",                      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Turn him over to liege",            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Confiscate the corn",               GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let him go and have the corn",      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(7340, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Execute your friend painfully",     GOSSIP_SENDER_MAIN+1, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Execute your friend painlessly",    GOSSIP_SENDER_MAIN+2, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let your friend go",                GOSSIP_SENDER_MAIN+3, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7341, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Confront the diplomat",             GOSSIP_SENDER_MAIN+4, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show not so quiet defiance",        GOSSIP_SENDER_MAIN+5, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Remain quiet",                      GOSSIP_SENDER_MAIN+2, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7361, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Speak against your brother openly", GOSSIP_SENDER_MAIN+6, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Help your brother in",              GOSSIP_SENDER_MAIN+7, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Keep your brother out without letting him know", GOSSIP_SENDER_MAIN+8, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7362, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take credit, keep gold",            GOSSIP_SENDER_MAIN+5, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take credit, share the gold",       GOSSIP_SENDER_MAIN+4, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let the knight take credit",        GOSSIP_SENDER_MAIN+3, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7363, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks",                            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            pPlayer->SEND_GOSSIP_MENU(7364, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pCreature->CastSpell(pPlayer, SPELL_FORTUNE, false);
            pPlayer->SEND_GOSSIP_MENU(7365, pCreature->GetObjectGuid());
            break;
    }
}

bool GossipSelect_npc_sayge(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiSender)
    {
        case GOSSIP_SENDER_MAIN:
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+1:
            pCreature->CastSpell(pPlayer, SPELL_DMG, false);
            pPlayer->AddSpellCooldown(SPELL_DMG,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+2:
            pCreature->CastSpell(pPlayer, SPELL_RES, false);
            pPlayer->AddSpellCooldown(SPELL_RES,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+3:
            pCreature->CastSpell(pPlayer, SPELL_ARM, false);
            pPlayer->AddSpellCooldown(SPELL_ARM,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+4:
            pCreature->CastSpell(pPlayer, SPELL_SPI, false);
            pPlayer->AddSpellCooldown(SPELL_SPI,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+5:
            pCreature->CastSpell(pPlayer, SPELL_INT, false);
            pPlayer->AddSpellCooldown(SPELL_INT,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+6:
            pCreature->CastSpell(pPlayer, SPELL_STM, false);
            pPlayer->AddSpellCooldown(SPELL_STM,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+7:
            pCreature->CastSpell(pPlayer, SPELL_STR, false);
            pPlayer->AddSpellCooldown(SPELL_STR,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+8:
            pCreature->CastSpell(pPlayer, SPELL_AGI, false);
            pPlayer->AddSpellCooldown(SPELL_AGI,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
    }
    return true;
}

/*######
## mob_deeprun_rat
######*/

enum eDeeprunRat
{
    SPELL_MELODIOUS_RAPTURE     = 21050,
    NPC_ENTHRALLED_DEEPRUN_RAT  = 13017,
};

struct MANGOS_DLL_DECL mob_deeprun_ratAI : public ScriptedAI
{
    mob_deeprun_ratAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}
	
    void Reset() {}
	
    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_MELODIOUS_RAPTURE)
        {
            //m_creature->SetEntry(NPC_ENTHRALLED_DEEPRUN_RAT);
            m_creature->UpdateEntry(NPC_ENTHRALLED_DEEPRUN_RAT);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveFollow(pCaster, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }
    }
};
CreatureAI* GetAI_mob_deeprun_rat(Creature* pCreature)
{
	return new mob_deeprun_ratAI (pCreature);
}

/*######
## mob_shahram
######*/

enum eShahram
{
    SPELL_FLAMES_OF_SHAHRAM     = 16596,
    SPELL_CURSE_OF_SHAHRAM      = 16597,
    SPELL_WILL_OF_SHAHRAM       = 16598,
    SPELL_BLESSING_OF_SHAHRAM   = 16599,
    SPELL_MIGHT_OF_SHAHRAM      = 16600,
    SPELL_FIST_OF_SHAHRAM       = 16601
};

struct MANGOS_DLL_DECL mob_shahramAI : public FollowerAI
{
    mob_shahramAI(Creature* pCreature) : FollowerAI(pCreature)
    {
        if (pCreature->GetOwner() && pCreature->GetOwner()->GetTypeId() == TYPEID_PLAYER)
            StartFollow((Player*)pCreature->GetOwner());

        m_uiCastSpell_Timer = 1000;
        m_uiDespawn_Timer = 2500;
        bSpellCasted = false;

        Reset();
    }

    uint32 m_uiCastSpell_Timer;
    uint32 m_uiDespawn_Timer;
    bool bSpellCasted;

    void Reset() {}

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        // Despawner
        if (m_uiDespawn_Timer <= uiDiff)
        {
            SetFollowComplete();
            m_creature->RemoveFromWorld();
        }
        else
            m_uiDespawn_Timer -= uiDiff;

        if (!bSpellCasted && GetLeaderForFollower())
        {
            if (m_uiCastSpell_Timer <= uiDiff)
            {
                bSpellCasted = true;
                m_creature->InterruptNonMeleeSpells(false);
                Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                Unit* pOwner = GetLeaderForFollower();

                // Select a radnom spell from Shahram spells offer and cast him
                switch(urand(1,6))
                {
                    case 1: DoCastSpellIfCan(target ? target : m_creature, SPELL_FLAMES_OF_SHAHRAM); break;
                    case 2: DoCastSpellIfCan(target ? target : m_creature, SPELL_CURSE_OF_SHAHRAM); break;
                    case 3: DoCastSpellIfCan(pOwner ? pOwner : m_creature, SPELL_WILL_OF_SHAHRAM); break;
                    case 4: DoCastSpellIfCan(pOwner ? pOwner : m_creature, SPELL_BLESSING_OF_SHAHRAM); break;
                    case 5: DoCastSpellIfCan(target ? target : m_creature, SPELL_MIGHT_OF_SHAHRAM); break;
                    case 6: DoCastSpellIfCan(pOwner ? pOwner : m_creature, SPELL_FIST_OF_SHAHRAM); break;
                }
            }
            else
                m_uiCastSpell_Timer -= uiDiff;
        }

        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_mob_shahram(Creature* pCreature)
{
    return new mob_shahramAI(pCreature);
}

/*######
## npc_darrowshire_poltergeist
######*/

enum eDarrowshirePoltergeist
{
    SAY_HAUNTED_1   = -1000577,
    SAY_HAUNTED_2   = -1000578,
    SAY_HAUNTED_3   = -1000579,
    SAY_HAUNTED_4   = -1000580,
    SAY_HAUNTED_5   = -1000581,
    SAY_HAUNTED_6   = -1000582,
    SAY_HAUNTED_7   = -1000583,
    SAY_HAUNTED_8   = -1000584,
};

struct MANGOS_DLL_DECL npc_darrowshire_poltergeistAI : public FollowerAI
{
    npc_darrowshire_poltergeistAI(Creature* pCreature) : FollowerAI(pCreature)
    {
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        if (pCreature->GetOwner())
            StartFollow((Player*)pCreature->GetOwner());
        Reset();
    }

    uint32 m_uiRandomSayTimer;

    void Reset()
    {
        m_uiRandomSayTimer = 5;
    }

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (m_uiRandomSayTimer <= uiDiff)
        {
            uint32 uiEntryToSay = 0;

            switch(urand(1,8))
            {
                case 1: uiEntryToSay = SAY_HAUNTED_1; break;
                case 2: uiEntryToSay = SAY_HAUNTED_2; break;
                case 3: uiEntryToSay = SAY_HAUNTED_3; break;
                case 4: uiEntryToSay = SAY_HAUNTED_4; break;
                case 5: uiEntryToSay = SAY_HAUNTED_5; break;
                case 6: uiEntryToSay = SAY_HAUNTED_6; break;
                case 7: uiEntryToSay = SAY_HAUNTED_7; break;
                case 8: uiEntryToSay = SAY_HAUNTED_8; break;
            }

            DoScriptText(uiEntryToSay, m_creature);

            m_uiRandomSayTimer = 30000;
        }
        else
            m_uiRandomSayTimer -= uiDiff;

        // no meele
    }
};

CreatureAI* GetAI_npc_darrowshire_poltergeist(Creature* pCreature)
{
    return new npc_darrowshire_poltergeistAI(pCreature);
}

/*######
## npc_metzen_the_reindeer
######*/

enum eMetzenTheReindeer
{
    GOSSIP_METZEN_START         = 8076,
    GOSSIP_METZEN_END           = 8077,
    ITEM_POUCH_OF_REINDEER_DUST = 21211,
    QUEST_METZEN_THE_REINDEER   = 8746,
    SPELL_REINDEER_DUST_EFFECT  = 25952,
};

bool GossipHello_npc_metzen_the_reindeer(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_METZEN_THE_REINDEER) == QUEST_STATUS_INCOMPLETE && pPlayer->HasItemCount(ITEM_POUCH_OF_REINDEER_DUST, 1))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Sparkle some of the reindeer dust onto Metzen", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_METZEN_START, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_metzen_the_reindeer(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pCreature->CastSpell(pCreature, SPELL_REINDEER_DUST_EFFECT, false);
        pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetObjectGuid());
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_METZEN_END, pCreature->GetObjectGuid());
    }
    return true;
}

/*####
## npc_winter_reveler
####*/

enum eWinterReveler
{
    SPELL_MISTLETOE         = 26218,
    SPELL_CREATE_MISTLETOE  = 26206,
    SPELL_CREATE_FRESH_HOLY = 26207,
};

struct MANGOS_DLL_DECL npc_winter_revelerAI : public ScriptedAI
{
    npc_winter_revelerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
	}

    void Reset() {}
    
    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        if (!pPlayer->HasAura(SPELL_MISTLETOE, EFFECT_INDEX_1) && emote == TEXTEMOTE_KISS)
        {
            pPlayer->CastSpell(pPlayer, SPELL_MISTLETOE, true);
            m_creature->CastSpell(pPlayer, urand(0,1) ? SPELL_CREATE_MISTLETOE : SPELL_CREATE_FRESH_HOLY, true);
        }
    }
};

CreatureAI* GetAI_npc_winter_reveler(Creature* pCreature)
{
	return new npc_winter_revelerAI(pCreature);
}

/*######
## mob_the_cleaner
######*/

enum eTheCleaner
{
    SAY_THE_CLEANER_SPAWN   = -1000667,
    SPELL_IMMUNE_ALL        = 1302,        // 1302 29230
};

struct MANGOS_DLL_DECL mob_the_cleanerAI : public ScriptedAI
{
    mob_the_cleanerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        DoScriptText(SAY_THE_CLEANER_SPAWN, pCreature);
        Reset();
    }

    uint32 m_uiImmuneAllTimer;

    void Reset()
    {
        m_uiImmuneAllTimer = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Immune All (spell 1302 in testing)
        if (m_uiImmuneAllTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_IMMUNE_ALL) == CAST_OK)
                m_uiImmuneAllTimer = 44000;
        }
        else
            m_uiImmuneAllTimer -= uiDiff;

        ScriptedAI::UpdateAI(uiDiff);
    }
};

CreatureAI* GetAI_mob_the_cleaner(Creature* pCreature)
{
    return new mob_the_cleanerAI(pCreature);
}

/*######
## mob_mangy_wolf
######*/

#define NPC_GUARD_ROBERTS 12423

struct MANGOS_DLL_DECL mob_mangy_wolfAI : public ScriptedAI
{
    mob_mangy_wolfAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetEntry() == NPC_GUARD_ROBERTS)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }
};
CreatureAI* GetAI_mob_mangy_wolf(Creature* pCreature)
{
    return new mob_mangy_wolfAI(pCreature);
}

/*######
## goblin_land_mine
######*/

struct MANGOS_DLL_DECL goblin_land_mineAI : public ScriptedAI
{
    goblin_land_mineAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		m_pMap = m_creature->GetMap();
		Reset();
	}

	//std::vector<ObjectGuid> L;
	GUIDList L;
	uint32 UpdateTimer;
	Map* m_pMap;

    void Reset()
    {
		L.clear();
		m_creature->SetMaxHealth(200);
		m_creature->SetHealth(200);
		SetCombatMovement(false);
		UpdateTimer = 750;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->IsHostileTo(m_creature))
            L.push_back(pWho->GetObjectGuid());
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_pMap)
		{
			m_pMap = m_creature->GetMap();
			return;
		}

		if (UpdateTimer <= uiDiff)
		{
			UpdateTimer = 750;
			//L.clear();
			//m_creature->FillGuidsListFromThreatList(L);
			//for (std::vector<ObjectGuid>::iterator itr = L.begin(); itr != L.end(); itr++)
			for (GUIDList::iterator itr = L.begin(); itr != L.end(); itr++)
			{
				Unit* U = m_pMap->GetUnit(*itr);
				if (U && m_creature->GetDistance(U) <= 5.0f)
				{
					m_creature->CastSpell(m_creature,4043,false);
					break;
				}
			}
		}
		else
			UpdateTimer -= uiDiff;
	}

};
CreatureAI* GetAI_goblin_land_mine(Creature* pCreature)
{
    return new goblin_land_mineAI(pCreature);
}

static uint32 hunterQuestEntries[12] = {6064, 6084, 6085, 6061, 6087, 6088, 6063, 6101, 6102, 6062, 6082, 6083};

static uint32 hunterQuestPetEntries [12] = {1126, 1201, 1196, 2956, 2959, 2970, 1998, 2043, 1996, 3099, 3126, 3107};

bool OnQuestRewarded_npc_hunterpetquest(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    std::map<uint32, uint32> hunterQuestPetEntryRel;

    for(int i = 0; i < 12; ++i)
        hunterQuestPetEntryRel[hunterQuestEntries[i]] = hunterQuestPetEntries[i];

	if(pPlayer && pQuest && pCreature)
    {
        uint32 m_creatureId = hunterQuestPetEntryRel.find(pQuest->GetQuestId())->second;

        std::list<Creature*> mPossiblePets;
        GetCreatureListWithEntryInGrid(mPossiblePets, pPlayer, m_creatureId, 20.0f);

        for(Creature* pPet : mPossiblePets)
        {
            if(pPet)
            {
                Unit* pOwner = pPet->GetCharmerOrOwner();

                if(pOwner)
                {
                    if(pOwner->GetGUIDLow() == pPlayer->GetGUIDLow())
                        pPet->ForcedDespawn();
                }
            }
        }
    }
	return true;
}

/* AddSC() */

void AddSC_npcs_special()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_chicken_cluck";
    pNewScript->GetAI = &GetAI_npc_chicken_cluck;
    pNewScript->pQuestAcceptNPC =   &QuestAccept_npc_chicken_cluck;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_npc_chicken_cluck;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_injured_patient";
    pNewScript->GetAI = &GetAI_npc_injured_patient;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_doctor";
    pNewScript->GetAI = &GetAI_npc_doctor;
    pNewScript->pGossipHello = &GossipHello_npc_doctor;
    pNewScript->pGossipSelect = &GossipSelect_npc_doctor;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_doctor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_garments_of_quests";
    pNewScript->GetAI = &GetAI_npc_garments_of_quests;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_guardian";
    pNewScript->GetAI = &GetAI_npc_guardian;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="npc_lunaclaw";
    pNewScript->GetAI = &GetAI_npc_lunaclaw;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lunaclaw_spirit";
    pNewScript->pGossipHello =  &GossipHello_npc_lunaclaw_spirit;
    pNewScript->pGossipSelect = &GossipSelect_npc_lunaclaw_spirit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mount_vendor";
    pNewScript->pGossipHello =  &GossipHello_npc_mount_vendor;
    pNewScript->pGossipSelect = &GossipSelect_npc_mount_vendor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rogue_trainer";
    pNewScript->pGossipHello =  &GossipHello_npc_rogue_trainer;
    pNewScript->pGossipSelect = &GossipSelect_npc_rogue_trainer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_sayge";
    pNewScript->pGossipHello = &GossipHello_npc_sayge;
    pNewScript->pGossipSelect = &GossipSelect_npc_sayge;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_deeprun_rat";
    pNewScript->GetAI = &GetAI_mob_deeprun_rat;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_shahram";
    pNewScript->GetAI = &GetAI_mob_shahram;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_darrowshire_poltergeist";
    pNewScript->GetAI = &GetAI_npc_darrowshire_poltergeist;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_metzen_the_reindeer";
    pNewScript->pGossipHello = &GossipHello_npc_metzen_the_reindeer;
    pNewScript->pGossipSelect = &GossipSelect_npc_metzen_the_reindeer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_winter_reveler";
    pNewScript->GetAI = &GetAI_npc_winter_reveler;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_the_cleaner";
    pNewScript->GetAI = &GetAI_mob_the_cleaner;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_mangy_wolf";
    pNewScript->GetAI = &GetAI_mob_mangy_wolf;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "goblin_land_mine";
    pNewScript->GetAI = &GetAI_goblin_land_mine;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_hunterpetquest";
	pNewScript->pQuestRewardedNPC = &OnQuestRewarded_npc_hunterpetquest;
    pNewScript->RegisterSelf();
}
