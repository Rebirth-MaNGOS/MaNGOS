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
SDName: Boss_Rajaxx
SD%Complete: 90
SDComment: Sometimes event bugs appear
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "ruins_of_ahnqiraj.h"
#include "TemporarySummon.h"

#define GOSSIP_START  "Let's find out."

enum eRajaxx
{
    SAY_ANDOROV_READY   = -1509003,
    SAY_ANDOROV_INTRO2  = -1509029,
    SAY_ANDOROV_ATTACK  = -1509030,
    SAY_ANDOROV_INTRO   = -1509004,
    SAY_WAVE3           = -1509005,
    SAY_WAVE4           = -1509006,
    SAY_WAVE5           = -1509007,
    SAY_WAVE6           = -1509008,
    SAY_WAVE7           = -1509009,
    SAY_WAVE8           = -1509010,
    SAY_UNK1            = -1509011,
    SAY_UNK2            = -1509012,
    SAY_UNK3            = -1509013,
    SAY_DEATH           = -1509014,
    SAY_DEAGGRO         = -1509015,
    SAY_KILLS_ANDOROV   = -1509016,
    SAY_COMPLETE_QUEST  = -1509017,                       // Yell when realm complete quest 8743 for world event
    EMOTE_FRENZY        = -1000001,

    // General Rajaxx
    SPELL_DISARM            = 6713,
    SPELL_FRENZY            = 8269,
    SPELL_SUMMON_PLAYER     = 20477,
    SPELL_THUNDERCRASH      = 25599,

    // General Andorov
    SPELL_AURA_OF_COMMAND   = 25516,
    SPELL_BASH              = 25515,
    SPELL_STRIKE            = 22591,
};

struct Move
{
    float x, y, z;
};

static Move Andorov[]=
{
    {-8872.0f, 1628.0f, 21.38f},
    {-8919.6f, 1550.0f, 21.38f},
    {-8939.8f, 1550.3f, 21.58f}
};


static Loc WaveOne[]=
{
    {-9021.57f, 1550.71f, 21.53f, 2.97f, NPC_CAPTAIN_QEEZ},
    {-9012.73f, 1559.70f, 21.38f, 3.07f, NPC_QIRAJI_WARRIOR},
    {-9013.22f, 1552.06f, 21.38f, 3.07f, NPC_QIRAJI_WARRIOR},
    {-9013.77f, 1543.49f, 21.38f, 3.07f, NPC_QIRAJI_WARRIOR},
    {-9014.38f, 1533.80f, 21.38f, 3.07f, NPC_QIRAJI_WARRIOR},
    {-9007.90f, 1543.82f, 21.38f, 3.10f, NPC_SWARMGUARD_NEEDLER},
    {-9007.60f, 1551.29f, 21.45f, 3.10f, NPC_SWARMGUARD_NEEDLER},
};

static Loc WaveTwo[]=
{
    {-9041.76f, 1517.72f, 21.38f, 2.06f, NPC_CAPTAIN_TUUBID},
    {-9029.46f, 1515.14f, 21.38f, 2.06f, NPC_QIRAJI_WARRIOR},
    {-9037.19f, 1510.98f, 21.39f, 2.06f, NPC_QIRAJI_WARRIOR},
    {-9045.89f, 1506.29f, 21.47f, 2.06f, NPC_QIRAJI_WARRIOR},
    {-9041.85f, 1498.79f, 21.90f, 2.06f, NPC_SWARMGUARD_NEEDLER},
    {-9034.38f, 1502.82f, 21.61f, 2.06f, NPC_SWARMGUARD_NEEDLER},
    {-9025.83f, 1507.42f, 21.44f, 2.06f, NPC_SWARMGUARD_NEEDLER},
};

static Loc WaveThree[]=
{
    {-9025.00f, 1595.24f, 21.38f, 4.03f, NPC_CAPTAIN_DRENN},
    {-9019.72f, 1601.85f, 21.53f, 4.03f, NPC_QIRAJI_WARRIOR},
    {-9012.63f, 1595.78f, 21.38f, 4.00f, NPC_SWARMGUARD_NEEDLER},
    {-9021.20f, 1589.84f, 21.38f, 4.03f, NPC_SWARMGUARD_NEEDLER},
    {-9030.41f, 1589.14f, 21.38f, 3.94f, NPC_SWARMGUARD_NEEDLER},
    {-9030.80f, 1598.04f, 21.38f, 3.97f, NPC_SWARMGUARD_NEEDLER},
    {-9027.72f, 1607.21f, 21.40f, 4.00f, NPC_SWARMGUARD_NEEDLER}, 
};

static Loc WaveFour[]=
{
    {-9084.72f, 1526.44f, 21.38f, 1.82f, NPC_CAPTAIN_XURREM},
    {-9083.12f, 1520.36f, 21.38f, 1.82f, NPC_SWARMGUARD_NEEDLER},
    {-9076.62f, 1522.07f, 21.38f, 1.82f, NPC_SWARMGUARD_NEEDLER},
    {-9090.98f, 1518.29f, 21.38f, 1.82f, NPC_SWARMGUARD_NEEDLER},
    {-9081.50f, 1513.86f, 21.38f, 1.82f, NPC_QIRAJI_WARRIOR},
    {-9089.45f, 1511.77f, 21.42f, 1.82f, NPC_QIRAJI_WARRIOR},
    {-9075.02f, 1515.57f, 21.38f, 1.82f, NPC_QIRAJI_WARRIOR},
};

static Loc WaveFive[]=
{
    {-9077.43f, 1625.86f, 21.38f, 3.37f, NPC_MAJOR_YEGGETH},
    {-9073.02f, 1623.79f, 21.38f, 3.36f, NPC_QIRAJI_WARRIOR},
    {-9074.51f, 1630.35f, 21.38f, 3.36f, NPC_QIRAJI_WARRIOR},
    {-9067.97f, 1616.70f, 21.39f, 3.36f, NPC_SWARMGUARD_NEEDLER},
    {-9069.66f, 1624.16f, 21.38f, 3.36f, NPC_SWARMGUARD_NEEDLER},
    {-9071.27f, 1631.26f, 21.38f, 3.36f, NPC_SWARMGUARD_NEEDLER},
    {-9072.92f, 1638.55f, 21.38f, 3.36f, NPC_SWARMGUARD_NEEDLER},
};

static Loc WaveSix[]=
{
    {-9136.52f, 1547.56f, 21.38f, 1.51f, NPC_MAJOR_PAKKON},
    {-9140.45f, 1542.85f, 21.38f, 1.53f, NPC_SWARMGUARD_NEEDLER},
    {-9134.11f, 1542.60f, 21.38f, 1.53f, NPC_SWARMGUARD_NEEDLER},
    {-9134.36f, 1536.43f, 21.38f, 1.53f, NPC_QIRAJI_WARRIOR},
    {-9141.08f, 1536.69f, 21.38f, 1.53f, NPC_QIRAJI_WARRIOR},
    {-9148.35f, 1536.99f, 21.38f, 1.53f, NPC_QIRAJI_WARRIOR},
    {-9126.90f, 1536.13f, 21.44f, 1.53f, NPC_QIRAJI_WARRIOR},
};

static Loc WaveSeven[]=
{
    {-9109.64f, 1586.35f, 21.38f, 2.57f, NPC_COLONEL_ZERRAN},
    {-9098.42f, 1589.87f, 21.38f, 2.53f, NPC_SWARMGUARD_NEEDLER},
    {-9103.71f, 1582.17f, 21.38f, 2.53f, NPC_SWARMGUARD_NEEDLER},
    {-9108.99f, 1574.48f, 21.40f, 2.53f, NPC_SWARMGUARD_NEEDLER},
    {-9102.06f, 1569.72f, 21.39f, 2.53f, NPC_QIRAJI_WARRIOR},
    {-9096.78f, 1577.42f, 21.38f, 2.53f, NPC_QIRAJI_WARRIOR},
    {-9091.70f, 1584.81f, 21.38f, 2.53f, NPC_QIRAJI_WARRIOR},
};

struct MANGOS_DLL_DECL boss_rajaxxAI : public ScriptedAI
{
    boss_rajaxxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        if (!m_creature->isActiveObject())
            m_creature->SetActiveObjectState(true);


        m_pInstance = (instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAJAXX, NOT_STARTED);

        m_bHasEnraged = false;

        m_uiDisarmTimer = 5000;
        m_uiThunderCrashTimer = 25000;

        RestartEvent();
    }    

    instance_ruins_of_ahnqiraj* m_pInstance;

    bool m_bHasEnraged;

    uint32 m_uiDeadWaveMemberCount;
    uint32 m_uiEventCheckTimer;
    uint32 m_uiDisarmTimer;
    uint32 m_uiThunderCrashTimer;
    uint32 m_uiWaveCount;
    uint32 m_uiWaveTimer;

    std::vector<ObjectGuid> m_uiSummonList;

    void Reset()
    {
        RestartEvent();

        m_bHasEnraged = false;

        m_uiDisarmTimer = 5000;
        m_uiThunderCrashTimer = 25000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAJAXX, FAIL);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAJAXX, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_RAJAXX, DONE);

        // Reward raid with 150 rep if Andorov lives (temp, will move to Andorov script later)
        // TODO: make RepMgr functions accessible
        /*Creature* pAndorov = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(DATA_ANDOROV));
        if (pAndorov && pAndorov->isAlive())
        {
            FactionEntry const* pFaction = sFactionStore.LookupEntry(609);

            if(!pFaction)
                return;

            Map::PlayerList const& lPlayers = m_pInstance->instance->GetPlayers();

            if (!lPlayers.isEmpty())
                for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                    if (Player* pPlayer = itr->getSource())
                        pPlayer->GetReputationMgr().ModifyReputation(pFaction, 150);
                }
        }*/
    }

    void KilledUnit(Unit* pVictim)
    {
        if (!m_creature->isInCombat())
            DoScriptText(SAY_DEAGGRO, m_creature, pVictim);
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        switch(pSummoned->GetEntry())
        {
            case NPC_CAPTAIN_QEEZ:
            case NPC_CAPTAIN_TUUBID:
            case NPC_CAPTAIN_DRENN:
            case NPC_CAPTAIN_XURREM:
            case NPC_MAJOR_YEGGETH:
            case NPC_MAJOR_PAKKON:
            case NPC_COLONEL_ZERRAN:
            case NPC_QIRAJI_WARRIOR:
            case NPC_SWARMGUARD_NEEDLER:
                ++m_uiDeadWaveMemberCount;
                break;
        }
    }

    void StartEvent()
    {
        if (!m_pInstance)
            return;

		if (Creature* pAndorov = m_pInstance->GetSingleCreatureFromStorage(NPC_GENERAL_ANDOROV))
        {
            pAndorov->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            DoScriptText(SAY_ANDOROV_INTRO, pAndorov);
            DoScriptText(SAY_ANDOROV_INTRO2, pAndorov);
            pAndorov->GetMotionMaster()->MovePoint(0, Andorov[0].x, Andorov[0].y, Andorov[0].z);
        }
    }

    void RestartEvent()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAJAXX, NOT_STARTED);

        m_uiWaveCount = 0;
        m_uiEventCheckTimer = 120000;
        m_uiDeadWaveMemberCount = 0;
        m_uiEventCheckTimer = 2000;

        // Despawn current army
        for (ObjectGuid currentGuid : m_uiSummonList)
        {
            TemporarySummon* pSummon = dynamic_cast<TemporarySummon*>(m_creature->GetMap()->GetCreature(currentGuid));
            if (pSummon)
                pSummon->UnSummon();
        } 

        m_uiSummonList.clear();

        SummonArmy();
    }

    void AttackWave(Unit* pTarget = nullptr)
    {
        if (!m_uiWaveCount)
            return;

        if (m_uiWaveCount < 8 && m_uiSummonList.size() >= 7 * 7)
        {
            for (short i = 0; i < 7; i++)
            {
                Creature* pCreature = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[i + 7 * (m_uiWaveCount - 1)]);

                if (pCreature)
                {
                    if (pTarget)
                    {
                        pCreature->SetInCombatWithZone();
                        pCreature->Attack(pTarget, false);
                    }
                    else
                    {
                        pCreature->SetInCombatWithZone();

                        pCreature->GetMotionMaster()->MovePoint(0, -8836.f, 1637.f, 22.f, true);
                    }
                }
            }
        }
        else if (m_uiWaveCount == 8)
        {
            m_creature->setFaction(73);
            m_creature->SetInCombatWithZone();
            m_creature->GetMotionMaster()->MovePoint(0, -8836.f, 1637.f, 22.f, true);
        }

    }

    Creature* GetSummon(uint32 uiEntry)
    {
        Creature* pCreature = nullptr;
        std::find_if(m_uiSummonList.begin(), m_uiSummonList.end(), [&pCreature, uiEntry, this](const ObjectGuid& obj)
        {
            pCreature = m_creature->GetMap()->GetCreature(obj);
            if (pCreature)
            {
                if (pCreature->GetEntry() == uiEntry)
                    return true;
            }

            return false;
        });

        return pCreature;
    }

    bool IsWaveDead()
    {
        if (m_uiDeadWaveMemberCount >= m_uiWaveCount * 7)
            return true;

        return false;
    }

    static uint32 WaveConversion(uint8 uiCount)
    {
        switch(uiCount)
        {
            case 1:
                return NPC_CAPTAIN_QEEZ;
            case 2:
                return NPC_CAPTAIN_TUUBID;
            case 3:
                return NPC_CAPTAIN_DRENN;
            case 4:
                return NPC_CAPTAIN_XURREM;
            case 5:
                return NPC_MAJOR_YEGGETH;
            case 6:
                return NPC_MAJOR_PAKKON;
            case 7:
                return NPC_COLONEL_ZERRAN;
            case 8:
                return NPC_RAJAXX;
            default:
                return 0;
        }
    }

    void SummonArmy()
    {
        Loc* waves[7] = { WaveOne, WaveTwo, WaveThree, WaveFour, WaveFive, WaveSix, WaveSeven };

        for (short j = 0; j < 7; j++)
        {
            // Waves
            for(uint8 i = 0; i < 7; ++i)
            {
                Creature* pSummon = m_creature->SummonCreature(waves[j][i].id, waves[j][i].x, waves[j][i].y, waves[j][i].z, waves[j][i].o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000, true);
                if (pSummon)
                    m_uiSummonList.push_back(pSummon->GetObjectGuid());
            }
        }
    }  


    void UpdateAI(const uint32 uiDiff)
    {
        // Call attack of next wave
        if (m_pInstance && m_pInstance->GetData(TYPE_RAJAXX) == IN_PROGRESS)
        {
            if (m_uiWaveTimer <= uiDiff || IsWaveDead())
            {
                ++m_uiWaveCount;
                AttackWave();
                if (m_uiWaveCount > 2)
                    DoScriptText(SAY_WAVE3 - m_uiWaveCount + 3, m_creature);

                m_uiEventCheckTimer = 20000;
                m_uiWaveTimer = 120000;
            }
            else
                m_uiWaveTimer -= uiDiff;


        }

        if (m_uiEventCheckTimer <= uiDiff)
        {
            if (m_uiWaveCount < 8)
            {
                bool inCombat = false;
                Unit* pTarget = nullptr;
                for (ObjectGuid guid : m_uiSummonList)
                {
                    Creature* pSummon = m_creature->GetMap()->GetCreature(guid);
                    if (pSummon && pSummon->isInCombat())
                    {
                        inCombat = true;
                        Unit* pVictim = pSummon->getVictim();
                        if (pVictim)
                            pTarget = pVictim;
                    }
                }

                if (!inCombat && m_pInstance && m_pInstance->GetData(TYPE_RAJAXX) == IN_PROGRESS)
                {
                    m_pInstance->SetData(TYPE_RAJAXX, FAIL);

                    RestartEvent();
                }
                else if (inCombat && m_pInstance && m_pInstance->GetData(TYPE_RAJAXX) != IN_PROGRESS)
                {
                    m_pInstance->SetData(TYPE_RAJAXX, IN_PROGRESS);

                    m_uiWaveCount = 1;
                    m_uiWaveTimer = 120000;

                    AttackWave(pTarget);
                }

                m_uiEventCheckTimer = 2000;
            }
        }
        else
            m_uiEventCheckTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Enrage at 30% hp
        if (HealthBelowPct(30))
            if (DoCastSpellIfCan(m_creature, SPELL_FRENZY, CAST_AURA_NOT_PRESENT) == CAST_OK)
                DoScriptText(EMOTE_FRENZY, m_creature);

        // Disarm
        if (m_uiDisarmTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DISARM);
            m_uiDisarmTimer = 15000;
        }
        else
            m_uiDisarmTimer -= uiDiff;

        // Thundecrash
        if (m_uiThunderCrashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_THUNDERCRASH);
            m_uiThunderCrashTimer = 30000;
        }
        else
            m_uiThunderCrashTimer -= uiDiff;

        if (Player* pPlayer = GetPlayerAtMinimumRange(DEFAULT_VISIBILITY_DISTANCE))
            DoCastSpellIfCan(pPlayer, SPELL_SUMMON_PLAYER);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rajaxx(Creature* pCreature)
{
    return new boss_rajaxxAI(pCreature);
}

struct MANGOS_DLL_DECL npc_general_andorovAI : public ScriptedAI
{
    npc_general_andorovAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->setFaction(1254);
        m_pInstance = (instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData();

        SummonKaldorei();

        Reset();
    }

    instance_ruins_of_ahnqiraj* m_pInstance;

    bool m_bCanMoveNext;
    bool m_bWaypointEnd;

    uint32 m_uiAuraOfCommand;
    uint32 m_uiBashTimer;
    uint32 m_uiStrikeTimer;
    uint32 m_uiWaypoint;
    uint32 m_uiWaitForOthersTimer;

    void Reset() 
    {
        m_bCanMoveNext = false;
        m_bWaypointEnd = false;

        m_uiAuraOfCommand = 10000;
        m_uiBashTimer = 5000;
        m_uiStrikeTimer = 15000;

        m_uiWaypoint = 0;
        m_uiWaitForOthersTimer = 500;
    }

    void SummonKaldorei()
    {
        // Kaldorei Elites
        for(uint8 i = 0; i < 4 ; ++i)
        {
            if (Creature* pKaldorei = m_creature->SummonCreature(NPC_KALDOREI_ELITE, NPCs[i].x, NPCs[i].y, NPCs[i].z, NPCs[i].o, TEMPSUMMON_CORPSE_DESPAWN, 0))
            {
                if (Creature* pAndorov = m_pInstance->GetSingleCreatureFromStorage(NPC_GENERAL_ANDOROV))
                {
                    pKaldorei->setFaction(1254);
                    pKaldorei->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    pKaldorei->GetMotionMaster()->MoveFollow(pAndorov, 2, i+1.5);
                }
            }
        }
    }

    void RelocateKaldorei(float x, float y, float z, float orientation)
    {
/*
        for (ObjectGuid guid : m_uiSummonList)
        {
            if (guid.GetEntry() != NPC_KALDOREI_ELITE)
                continue;

            Creature* pKaldorei = pAndorov->GetMap()->GetCreature(guid);
            if (pKaldorei)
                pKaldorei->RelocateCreature(x, y, z, orientation);
        }*/
    }

    void Aggro(Unit* pWho)
    {
        std::list<Creature*> m_lKaldorei;
        GetCreatureListWithEntryInGrid(m_lKaldorei, m_creature, NPC_KALDOREI_ELITE, DEFAULT_VISIBILITY_DISTANCE);
        if (!m_lKaldorei.empty())
            for(std::list<Creature*>::iterator itr = m_lKaldorei.begin(); itr != m_lKaldorei.end(); ++itr)
                if ((*itr) && (*itr)->isAlive())
                    (*itr)->AI()->AttackStart(pWho);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
        {
			Creature* pRajaxx = m_pInstance->GetSingleCreatureFromStorage(NPC_RAJAXX);
            if (pRajaxx && pRajaxx->isAlive())
                DoScriptText(SAY_KILLS_ANDOROV, pRajaxx);
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE || m_bWaypointEnd)
            return;

        m_uiWaypoint = uiPointId + 1;

        if (uiPointId == 2)
        {
            DoScriptText(SAY_ANDOROV_READY, m_creature);
            DoScriptText(SAY_ANDOROV_ATTACK, m_creature);
            m_creature->HandleEmote(EMOTE_STATE_READY1H);

            std::list<Creature*> m_lKaldorei;
            GetCreatureListWithEntryInGrid(m_lKaldorei, m_creature, NPC_KALDOREI_ELITE, 10.0f);
            if (!m_lKaldorei.empty())
                for(std::list<Creature*>::iterator itr = m_lKaldorei.begin(); itr != m_lKaldorei.end(); ++itr)
                    if ((*itr) && (*itr)->isAlive())
                        (*itr)->HandleEmote(EMOTE_STATE_READY1H);

            CreatureCreatePos pos(m_creature->GetMap(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation());
            m_creature->SetSummonPoint(pos);
            m_bWaypointEnd = true;

            m_creature->RelocateCreature(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation());
            RelocateKaldorei(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation());

            Creature* pRajaxx = m_pInstance->GetSingleCreatureFromStorage(NPC_RAJAXX);
            if (pRajaxx)
            {
                boss_rajaxxAI* pAI = dynamic_cast<boss_rajaxxAI*>(pRajaxx->AI());
                if (pAI)
                {
                    pAI->m_uiWaveCount = 1;
                    pAI->AttackWave();
                }
            }

            if (m_pInstance)
                m_pInstance->SetData(TYPE_RAJAXX, IN_PROGRESS);
        }

        m_bCanMoveNext = true;
        m_uiWaitForOthersTimer = 500;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bWaypointEnd)
	{
            if (m_bCanMoveNext && m_uiWaitForOthersTimer <= uiDiff)
            {
                m_bCanMoveNext = false;
                if (m_bWaypointEnd)
                    m_creature->GetMotionMaster()->Clear();
                else
                    m_creature->GetMotionMaster()->MovePoint(m_uiWaypoint, Andorov[m_uiWaypoint].x, Andorov[m_uiWaypoint].y, Andorov[m_uiWaypoint].z); 
            }
            else
                m_uiWaitForOthersTimer -= uiDiff;

	}

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Bash
        if (m_uiBashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BASH);
            m_uiBashTimer = 30000;
        }
        else
            m_uiBashTimer -= uiDiff;

        // Aura of Command
        if (m_uiAuraOfCommand <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_AURA_OF_COMMAND);
            m_uiAuraOfCommand = 20000;
        }
        else
            m_uiAuraOfCommand -= uiDiff;

        // Strike
        if (m_uiStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);
            m_uiStrikeTimer = 15000;
        }
        else
            m_uiStrikeTimer -= uiDiff; 

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_general_andorov(Creature* pCreature)
{
    return new npc_general_andorovAI(pCreature);
}

bool GossipHello_npc_general_andorov(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
	pPlayer->SEND_GOSSIP_MENU(14442, pCreature->GetObjectGuid());       //insert into `npc_text` (`ID`, `text0_0`, `text0_1`, `lang0`, `prob0`, `em0_0`, `em0_1`, `em0_2`, `em0_3`, `em0_4`, `em0_5`, `text1_0`, `text1_1`, `lang1`, `prob1`, `em1_0`, `em1_1`, `em1_2`, `em1_3`, `em1_4`, `em1_5`, `text2_0`, `text2_1`, `lang2`, `prob2`, `em2_0`, `em2_1`, `em2_2`, `em2_3`, `em2_4`, `em2_5`, `text3_0`, `text3_1`, `lang3`, `prob3`, `em3_0`, `em3_1`, `em3_2`, `em3_3`, `em3_4`, `em3_5`, `text4_0`, `text4_1`, `lang4`, `prob4`, `em4_0`, `em4_1`, `em4_2`, `em4_3`, `em4_4`, `em4_5`, `text5_0`, `text5_1`, `lang5`, `prob5`, `em5_0`, `em5_1`, `em5_2`, `em5_3`, `em5_4`, `em5_5`, `text6_0`, `text6_1`, `lang6`, `prob6`, `em6_0`, `em6_1`, `em6_2`, `em6_3`, `em6_4`, `em6_5`, `text7_0`, `text7_1`, `lang7`, `prob7`, `em7_0`, `em7_1`, `em7_2`, `em7_3`, `em7_4`, `em7_5`) values('14442','<Andorov looks at you grimly.>$B$BIf it bleeds, we can kill it.','','0','1','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0','','','0','0','0','0','0','0','0','0');

    return true;
}

bool GossipSelect_npc_general_andorov(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
            ((boss_rajaxxAI*)pCreature->AI())->StartEvent();
            break;
    }
    return true;
}

void AddSC_boss_rajaxx()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_rajaxx";
    pNewScript->GetAI = &GetAI_boss_rajaxx;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_general_andorov";
    pNewScript->GetAI = &GetAI_npc_general_andorov;
    pNewScript->pGossipHello = &GossipHello_npc_general_andorov;
    pNewScript->pGossipSelect = &GossipSelect_npc_general_andorov;
    pNewScript->RegisterSelf();
}
