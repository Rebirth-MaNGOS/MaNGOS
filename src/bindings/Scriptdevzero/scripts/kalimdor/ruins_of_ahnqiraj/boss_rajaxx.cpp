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

struct Orientation
{
    float o;
};

static Move Andorov[]=
{
    {-8872.0f, 1628.0f, 21.38f},
    {-8919.6f, 1550.0f, 21.38f},
    {-8939.8f, 1550.3f, 21.58f}
};

static Move KaldoreiElite[]=
{
    {-8939.20f, 1544.97f, 21.47f},
    {-8933.68f, 1548.35f, 21.38f},
    {-8934.61f, 1553.60f, 51.38f},
    {-8940.60f, 1555.45f, 21.48f}
};

static Loc AndorovKaldoreiSpawn[] =
{
    {-8724.16f, 1576.59f, 21.38f, 2.37f, NPC_KALDOREI_ELITE},
    {-8727.11f, 1573.41f, 21.48f, 2.37f, NPC_KALDOREI_ELITE},
    {-8717.14f, 1584.30f, 21.41f, 2.37f, NPC_KALDOREI_ELITE},
    {-8714.63f, 1587.17f, 21.67f, 2.37f, NPC_KALDOREI_ELITE},
};

static Orientation BossTurn[]=
{5.81f, 5.266f, 6.275f, 5.325f, 0.25f, 4.95f, 5.65f};

static Orientation BossTurnBack[]=
{2.6586f, 2.16773f, 3.21623f, 2.19129f, 3.33012f, 1.9164f, 2.57f};

static Loc WaveOne[]=
{
    {-9007.93f, 1553.78f, 21.4471f, 2.6586f, NPC_CAPTAIN_QEEZ},    
    {-8991.87f, 1544.52f, 21.5402f, 2.93741f, NPC_QIRAJI_WARRIOR},
    {-9000.12f, 1548.71f, 22.1545f, 2.80781f, NPC_QIRAJI_WARRIOR},
    {-9005.65f, 1540.26f, 21.3876f, 2.87457f, NPC_QIRAJI_WARRIOR},
    {-8999.48f, 1559.31f, 22.0991f, 2.87457f, NPC_QIRAJI_WARRIOR},
    {-8996.66f, 1536.46f, 21.3973f, 2.78819f, NPC_SWARMGUARD_NEEDLER},
    {-8991.69f, 1557.3f, 21.5348f, 2.84317f, NPC_SWARMGUARD_NEEDLER},
};

static Loc WaveTwo[]=
{
    {-9039.84f, 1515.01f, 21.3867f, 2.16773f, NPC_CAPTAIN_TUUBID},
    {-9027.7f, 1511.36f, 21.3908f, 2.40726f, NPC_QIRAJI_WARRIOR},
    {-9036.07f, 1506.56f, 21.4651f, 2.1795f, NPC_QIRAJI_WARRIOR},
    {-9044.73f, 1501.99f, 21.6407f, 2.1795f, NPC_QIRAJI_WARRIOR},
    {-9041.21f, 1496.77f, 22.1256f, 2.17951f, NPC_SWARMGUARD_NEEDLER},
    {-9033.09f, 1501.58f, 21.7009f, 2.17951f, NPC_SWARMGUARD_NEEDLER},
    {-9024.8f, 1506.19f, 21.4779f, 2.17951f, NPC_SWARMGUARD_NEEDLER},
};

static Loc WaveThree[]=
{
    {-9035.08f, 1605.42f, 21.3869f, 3.21623f, NPC_CAPTAIN_DRENN},
    {-9027.48f, 1602.02f, 21.3867f, 3.14161f, NPC_QIRAJI_WARRIOR},
    {-9028.68f, 1594.08f, 21.3867f, 3.13769f, NPC_SWARMGUARD_NEEDLER},
    {-9027.33f, 1612.22f, 21.6694f, 3.19659f, NPC_SWARMGUARD_NEEDLER},
    {-9018.21f, 1611.46f, 23.8151f, 3.01988f, NPC_SWARMGUARD_NEEDLER},
    {-9019.53f, 1601.91f, 21.547f, 3.08663f, NPC_SWARMGUARD_NEEDLER},
    {-9019.97f, 1593.83f, 21.3868f, 3.08663f, NPC_SWARMGUARD_NEEDLER}, 
};

static Loc WaveFour[]=
{
    {-9082.3f, 1526.53f, 21.3868f, 2.19129f, NPC_CAPTAIN_XURREM},
    {-9087.04f, 1516.86f, 21.387f, 2.15987f, NPC_SWARMGUARD_NEEDLER},
    {-9080.49f, 1521.24f, 21.387f, 2.15987f, NPC_SWARMGUARD_NEEDLER},
    {-9074.12f, 1525.49f, 21.387f, 2.15987f, NPC_SWARMGUARD_NEEDLER},
    {-9069.55f, 1519.77f, 21.3862f, 2.32872f, NPC_QIRAJI_WARRIOR},
    {-9076.49f, 1515.23f, 21.3862f, 2.15201f, NPC_QIRAJI_WARRIOR},
    {-9083.1f, 1511.38f, 21.4098f, 2.07347f, NPC_QIRAJI_WARRIOR},
};

static Loc WaveFive[]=
{
    {-9088.17f, 1619.25f, 21.429f, 3.33012f, NPC_MAJOR_YEGGETH},
    {-9079.44f, 1611.76f, 21.3874f, 3.30261f, NPC_QIRAJI_WARRIOR},
    {-9080.38f, 1627.92f, 21.3874f, 3.47932f, NPC_QIRAJI_WARRIOR},
    {-9080.58f, 1620.74f, 21.3866f, 3.42043f, NPC_SWARMGUARD_NEEDLER},
    {-9071.55f, 1613.05f, 21.3888f, 3.34189f, NPC_SWARMGUARD_NEEDLER},
    {-9073.46f, 1622.49f, 21.3874f, 3.44399f, NPC_SWARMGUARD_NEEDLER},
    {-9074.59f, 1630.83f, 21.3874f, 3.58144f, NPC_SWARMGUARD_NEEDLER},
};

static Loc WaveSix[]=
{
    {-9126.08f, 1557.73f, 21.4741f, 1.9164f, NPC_MAJOR_PAKKON},
    {-9115.42f, 1544.86f, 21.3864f, 1.97923f, NPC_SWARMGUARD_NEEDLER},
    {-9131.38f, 1541.06f, 21.3864f, 1.72397f, NPC_SWARMGUARD_NEEDLER},
    {-9118.51f, 1552.4f, 21.3868f, 2.07347f, NPC_QIRAJI_WARRIOR},
    {-9125.47f, 1549.22f, 21.3868f, 1.89675f, NPC_QIRAJI_WARRIOR},
    {-9132.3f, 1547.67f, 21.3868f, 1.69647f, NPC_QIRAJI_WARRIOR},
    {-9124.8f, 1541.52f, 21.3868f, 1.75538f, NPC_QIRAJI_WARRIOR},
};

static Loc WaveSeven[]=
{
    {-9109.64f, 1586.35f, 21.38f, 2.57f, NPC_COLONEL_ZERRAN},
    {-9091.79f, 1583.9f, 21.3867f, 2.6743f, NPC_SWARMGUARD_NEEDLER},
    {-9096.74f, 1577.06f, 21.3867f, 2.48188f, NPC_SWARMGUARD_NEEDLER},
    {-9101.77f, 1569.9f, 21.3892f, 2.529f, NPC_SWARMGUARD_NEEDLER},
    {-9107.35f, 1573.77f, 21.4013f, 2.40726f, NPC_QIRAJI_WARRIOR},
    {-9101.93f, 1580.37f, 21.3888f, 2.45438f, NPC_QIRAJI_WARRIOR},
    {-9097.81f, 1586.82f, 21.3874f, 2.63502f, NPC_QIRAJI_WARRIOR},
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
        m_uiAndorovRespawn = 0;
        m_bInitSpawn = true;
        RestartEvent();
    }    

    instance_ruins_of_ahnqiraj* m_pInstance;

    bool m_bHasEnraged;
    bool m_bAndorovEngaged;
    bool m_bInitSpawn;

    uint32 m_uiDeadWaveMemberCount;
    uint32 m_uiEventCheckTimer;
    uint32 m_uiDisarmTimer;
    uint32 m_uiThunderCrashTimer;
    uint32 m_uiWaveCount;
    uint32 m_uiWaveTimer;
    uint32 m_uiAndorovRespawn;
    uint32 m_uiTurnCaptains;
    uint32 m_uiAttackTraining;
    uint32 m_uiAttackTrainingOdd;
    uint32 m_uiAttackTrainingCpt;
    uint32 m_uiAttackTrainingOddCpt;
    uint32 m_uiAttackTrainingBoss;
    
    std::vector<ObjectGuid> m_uiSummonList;

    void Reset()
    {
        RestartEvent();

        m_bHasEnraged = false;
        m_bAndorovEngaged = false;        
        m_uiDisarmTimer = 5000;
        m_uiThunderCrashTimer = 25000; 
        
        // beginning RP
        m_uiAttackTrainingBoss = 15000;
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

		if (Creature* pAndorov = m_pInstance->GetSingleCreatureFromStorage(NPC_GENERAL_ANDOROV))
        {
            if(pAndorov->isAlive())
            {
                pAndorov->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VENDOR);
                pAndorov->HandleEmoteState(EMOTE_STATE_NONE);
            }
        }
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
            m_bAndorovEngaged = true;

            std::list<Creature*> m_lKaldorei;
            GetCreatureListWithEntryInGrid(m_lKaldorei, pAndorov, NPC_KALDOREI_ELITE, DEFAULT_VISIBILITY_DISTANCE);

            if (!m_lKaldorei.empty())
            {
                for(std::list<Creature*>::iterator itr = m_lKaldorei.begin(); itr != m_lKaldorei.end(); ++itr)
                {
                    if ((*itr) && (*itr)->isAlive())
                    {
                        (*itr)->GetMotionMaster()->MovePoint(0, Andorov[0].x, Andorov[0].y, Andorov[0].z);
                    }
                }
            }
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
        m_bAndorovEngaged = false;

        // Despawn current army
        for (ObjectGuid currentGuid : m_uiSummonList)
        {
            TemporarySummon* pSummon = dynamic_cast<TemporarySummon*>(m_creature->GetMap()->GetCreature(currentGuid));
            if (pSummon)
                pSummon->UnSummon();
        } 

        if(!m_bInitSpawn)
        {
		    if (Creature* pAndorov = m_pInstance->GetSingleCreatureFromStorage(NPC_GENERAL_ANDOROV))
            {
                if(pAndorov->isAlive())
                {
                    pAndorov->SetDeathState(JUST_DIED);
                    pAndorov->SetHealthPercent(0.0f);
                    pAndorov->AI()->JustDied(nullptr);
                }
            }
        }

        m_bInitSpawn = false;

        m_uiSummonList.clear();

        SummonArmy();
    }

    void StartAndorovRespawnTimer()
    {
        m_uiAndorovRespawn = 60000;
    }
    
    void AttackTrainWave(int Groups = 0)
    {
        if(Groups == 0)     // turn the captains
        {
            int pos = 0;
            for (short i = 0; i <= 42; i ++) 
            {
                Creature* pCreature = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[i]);

                if (pCreature)
                    pCreature->SetFacingTo(BossTurn[pos].o);      
                i = i + 6;
                pos++;
            }           
        }
        if(Groups == 1)    // turn the captains back
        {       
            int pos = 0;
            for (short i = 0; i <= 42; i ++) 
            {
                Creature* pCreature = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[i]);

                if (pCreature)
                    pCreature->SetFacingTo(BossTurnBack[pos].o);
                i = i + 6;
                pos++;
            }            
        }
        
        if(Groups == 2)
        {
            for (short i = 0; i < 7; i++)   // first group
            {
                Creature* pCreature = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[i]);

                if (pCreature)
                    pCreature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            }            
           
            for (short i = 14; i < 21; i++)   // third group
            {
                Creature* pCreature = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[i]);

                if (pCreature)
                    pCreature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            }
            for (short i = 21; i < 28; i++)   // fourth group
            {
                Creature* pCreature = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[i]);

                if (pCreature)
                    pCreature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            }     
            for (short i = 42; i < 49; i++)   // seventh group
            {
                Creature* pCreature = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[i]);

                if (pCreature)
                    pCreature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            }
        }
        if(Groups == 3)
        {
            for (short i = 7; i < 14; i++)   // second group
            {
                Creature* pCreature = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[i]);

                if (pCreature)
                    pCreature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            }                   
           for (short i = 28; i < 35; i++)   // fifth group
            {
                Creature* pCreature = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[i]);

                if (pCreature)
                    pCreature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            }
            for (short i = 35; i < 42; i++)   // sixth group
            {
                Creature* pCreature = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[i]);

                if (pCreature)
                    pCreature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            }        
        }
        if(Groups == 4)     // attack with first groups of captains 1,3,4,7
        {            
            Creature* pCreature1 = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[0]);

            if (pCreature1)
                pCreature1->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
                
            Creature* pCreature3 = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[14]);

            if (pCreature3)
                pCreature3->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);

            Creature* pCreature4 = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[21]);

            if (pCreature4)
                pCreature4->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            
            Creature* pCreature7 = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[42]);

            if (pCreature7)
                pCreature7->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
        }
          if(Groups == 5)     // attack with rest of the captains 2,5,6
        {
            Creature* pCreature2 = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[7]);

            if (pCreature2)
                pCreature2->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            
            Creature* pCreature5 = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[28]);

            if (pCreature5)
                pCreature5->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
            
            Creature* pCreature6 = m_creature->GetMap()->GetCreature(
                        m_uiSummonList[35]);

            if (pCreature6)
                pCreature6->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
        }
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
        // RP
        if(m_pInstance && m_pInstance->GetData(TYPE_RAJAXX) == NOT_STARTED && m_creature->isAlive() && !m_creature->isInCombat())
        {  
            if (m_uiAttackTrainingBoss <= uiDiff)
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_ATTACKUNARMED);
                AttackTrainWave(0);     // turns captains                
                m_uiAttackTrainingBoss = 15000;     
                m_uiTurnCaptains = 6000;
                m_uiAttackTrainingCpt = 1000;
                m_uiAttackTraining = 3000;
                m_uiAttackTrainingOdd = 4000;
                m_uiAttackTrainingOddCpt = 2000;                
            }
            else
                m_uiAttackTrainingBoss -= uiDiff;
            
            if (m_uiTurnCaptains <= uiDiff)     // turn the captains back
            {
                AttackTrainWave(1);     // turns captains back
                m_uiTurnCaptains = 15000;
            }
            else
                m_uiTurnCaptains -= uiDiff;
            
             if (m_uiAttackTraining <= uiDiff)
            {
                AttackTrainWave(2);     // attack with full groups 1             
                m_uiAttackTraining = 15000;                    
            }
            else
                m_uiAttackTraining -= uiDiff;
            
            if (m_uiAttackTrainingOdd <= uiDiff)
            {
                AttackTrainWave(3);    // attack with full groups 2        
                m_uiAttackTrainingOdd = 15000;
            }
            else
                m_uiAttackTrainingOdd -= uiDiff;
            
            if (m_uiAttackTrainingOddCpt <= uiDiff)
            {
                AttackTrainWave(5);
                m_uiAttackTrainingOddCpt = 15000;
            }
            else
                m_uiAttackTrainingOddCpt -= uiDiff;
            
            if (m_uiAttackTrainingCpt <= uiDiff)
            {
                AttackTrainWave(4);
                m_uiAttackTrainingCpt = 15000;
            }
            else
                m_uiAttackTrainingCpt -= uiDiff;
        }
        
        if(m_uiAndorovRespawn)
        {
            if(m_uiAndorovRespawn <= uiDiff)
            {
                m_uiAndorovRespawn = 0;

                if(m_pInstance && m_pInstance->GetData(TYPE_RAJAXX) == NOT_STARTED && m_creature->isAlive() && !m_creature->isInCombat())
                {
                    if (Creature* pAndorov = m_creature->SummonCreature(NPC_GENERAL_ANDOROV, -8719.97f, 1579.10f, 21.43f, 2.47f, TEMPSUMMON_CORPSE_DESPAWN, 0))
                    {
                        pAndorov->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VENDOR);
                        pAndorov->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                        pAndorov->GetMotionMaster()->MovePoint(5, -8873.42f, 1647.67f, 21.38f, false);
                        //pAndorov->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    }
                }
            }
            else
                m_uiAndorovRespawn -= uiDiff;
        }

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

                    if(!m_bAndorovEngaged)
                        StartEvent();

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



     /*   if (Player* pPlayer = GetPlayerAtMinimumRange(DEFAULT_VISIBILITY_DISTANCE))
            DoCastSpellIfCan(pPlayer, SPELL_SUMMON_PLAYER); */

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rajaxx(Creature* pCreature)
{
    return new boss_rajaxxAI(pCreature);
}

struct MANGOS_DLL_DECL npc_kaldorei_elite : public ScriptedAI
{
    npc_kaldorei_elite(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRunning = false;
        m_bFirstRun = true;
        m_pInstance = (instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ruins_of_ahnqiraj* m_pInstance;

    bool m_bCanMoveNext;
    bool m_bFirstRun;
    uint32 m_uiWaitForOthersTimer;
    uint32 m_uiWaypoint;
    uint32 m_uiEndMovePoint;
    float m_EndMovePointCoords[3];
    bool m_uiSetFacing;
    bool m_bIsRunning;

    float m_uiFacing;

    void Reset() 
    {
        if(!m_bIsRunning)
        {
            m_bCanMoveNext = false;
            m_uiWaypoint = 0;
            m_uiEndMovePoint = 0;
            m_uiFacing = 0.0;
            m_uiSetFacing = false;
        }
        else
            m_bCanMoveNext = true;

        if(m_bFirstRun)
        {
            m_creature->GetMotionMaster()->MovePoint(5, m_EndMovePointCoords[0], m_EndMovePointCoords[1], m_EndMovePointCoords[2], false);
        }

        m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        m_uiWaitForOthersTimer = 500;

        if(m_pInstance)
        {
            if(m_pInstance->GetData(TYPE_RAJAXX) == DONE)
                m_creature->HandleEmoteState(EMOTE_STATE_NONE);
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        m_uiWaypoint = uiPointId + 1;

        switch(uiPointId)
        {
            case 0:
                m_bIsRunning = true;
                break;
            case 5:
                {
                    m_uiSetFacing = true;
                    m_bFirstRun = false;
                    m_uiFacing = 5.69f;
                    break;
                }
            case 2:
                {
                    m_uiSetFacing = true;
                    m_bIsRunning = false;
                    m_uiFacing = 3.2f;
                    m_creature->HandleEmote(EMOTE_STATE_READY2H);
                    break;
                }
        }

        CreatureCreatePos pos(m_creature->GetMap(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation());
        m_creature->SetSummonPoint(pos);
        m_creature->RelocateCreature(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation());

        if(uiPointId != 5 && uiPointId != 2)
        {
            m_bCanMoveNext = true;
            m_uiWaitForOthersTimer = 500;
        }
    }

    void SetEndMovePoint(uint32 movePoint, float x, float y, float z)
    {
        m_uiEndMovePoint = movePoint;
        m_EndMovePointCoords[0] = x;
        m_EndMovePointCoords[1] = y;
        m_EndMovePointCoords[2] = z;
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if(m_uiSetFacing && m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
        {
            m_uiSetFacing = false;
            m_creature->SetFacingTo(m_uiFacing);
        }

        if (m_bCanMoveNext && m_uiWaitForOthersTimer <= uiDiff)
        {
            m_bCanMoveNext = false;
            if(m_uiWaypoint == 2)
                m_creature->GetMotionMaster()->MovePoint(m_uiWaypoint, KaldoreiElite[m_uiEndMovePoint].x, KaldoreiElite[m_uiEndMovePoint].y, KaldoreiElite[m_uiEndMovePoint].z);
            else
                m_creature->GetMotionMaster()->MovePoint(m_uiWaypoint, Andorov[m_uiWaypoint].x, Andorov[m_uiWaypoint].y, Andorov[m_uiWaypoint].z); 
        }
        else
            m_uiWaitForOthersTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_kaldorei_elite(Creature* pCreature)
{
    return new npc_kaldorei_elite(pCreature);
}

struct MANGOS_DLL_DECL npc_general_andorovAI : public ScriptedAI
{
    npc_general_andorovAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->setFaction(1254);
        m_pInstance = (instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData();
        m_bIsRunning = false;
        m_bFirstRun = true;
        SummonKaldorei();

        Reset();
    }

    instance_ruins_of_ahnqiraj* m_pInstance;

    bool m_bCanMoveNext;
    bool m_bIsRunning;
    bool m_uiSetFacing;
    bool m_bFirstRun;

    uint32 m_uiAuraOfCommand;
    uint32 m_uiBashTimer;
    uint32 m_uiStrikeTimer;
    uint32 m_uiWaypoint;
    uint32 m_uiWaitForOthersTimer;
    uint32 m_uiStartTimer;


    float m_uiFacing;

    void Reset() 
    {
        if(!m_bIsRunning)
        {
            m_bCanMoveNext = false;
            m_uiWaypoint = 0;
            m_uiFacing = 0.0;
            m_uiStartTimer = 0;
            m_uiSetFacing = false;
        }
        else
        {
            m_bCanMoveNext = true;
        }

        if(m_bFirstRun)
        {
            m_creature->GetMotionMaster()->MovePoint(5, -8873.42f, 1647.67f, 21.38f, false);
        }

        m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        m_uiAuraOfCommand = 10000;
        m_uiBashTimer = 5000;
        m_uiStrikeTimer = 15000;
        m_uiWaitForOthersTimer = 500;

    }

    void SummonKaldorei()
    {
        // Kaldorei Elites
        for(uint8 i = 0; i < 4 ; ++i)
        {
            if (Creature* pKaldorei = m_creature->SummonCreature(NPC_KALDOREI_ELITE, AndorovKaldoreiSpawn[i].x, AndorovKaldoreiSpawn[i].y, AndorovKaldoreiSpawn[i].z, AndorovKaldoreiSpawn[i].o, TEMPSUMMON_CORPSE_DESPAWN, 0))
            {
              //  if (Creature* pAndorov = m_pInstance->GetSingleCreatureFromStorage(NPC_GENERAL_ANDOROV))
               // {
                    pKaldorei->setFaction(1254);
                    pKaldorei->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    pKaldorei->GetMotionMaster()->MovePoint(5, NPCs[i].x, NPCs[i].y, NPCs[i].z, false);

                    npc_kaldorei_elite *pKaldoreiAi = dynamic_cast<npc_kaldorei_elite*>(pKaldorei->AI());

                    if(pKaldoreiAi)
                        pKaldoreiAi->SetEndMovePoint(i, NPCs[i].x, NPCs[i].y, NPCs[i].z);
              //  }
            }
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
            {
                boss_rajaxxAI *pRajaxxAi = dynamic_cast<boss_rajaxxAI*>(pRajaxx->AI());

                if(pRajaxxAi)
                {
                    pRajaxxAi->StartAndorovRespawnTimer();
                }

                if(m_pInstance->GetData(TYPE_RAJAXX) == IN_PROGRESS)
                    DoScriptText(SAY_KILLS_ANDOROV, pRajaxx);
            }

            if(m_pInstance->GetData(TYPE_RAJAXX) == NOT_STARTED)
            {
                std::list<Creature*> m_lKaldorei;
                GetCreatureListWithEntryInGrid(m_lKaldorei, m_creature, NPC_KALDOREI_ELITE, DEFAULT_VISIBILITY_DISTANCE);

                if (!m_lKaldorei.empty())
                    for(std::list<Creature*>::iterator itr = m_lKaldorei.begin(); itr != m_lKaldorei.end(); ++itr)
                        if ((*itr) && (*itr)->isAlive())
                        {
                            (*itr)->SetDeathState(JUST_DIED);
                            (*itr)->SetHealthPercent(0.0f);
                        }
            }
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        m_uiWaypoint = uiPointId + 1;

        switch(uiPointId)
        {
        case 0:
            m_bIsRunning = true;
            break;
        case 5:
            {
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_bFirstRun = false;
                m_uiSetFacing = true;
                m_uiFacing = 5.69f;
                break;
            }
        case 2:
            {
                DoScriptText(SAY_ANDOROV_READY, m_creature);
                DoScriptText(SAY_ANDOROV_ATTACK, m_creature);
                m_creature->HandleEmote(EMOTE_STATE_READY1H);
                m_bIsRunning = false;
                m_creature->GetMotionMaster()->Clear();
                m_bCanMoveNext = false;
                m_uiWaitForOthersTimer = 0;

                if(m_pInstance)
                    if(m_pInstance->GetData(TYPE_RAJAXX) != IN_PROGRESS)
                        m_uiStartTimer = 3000;

                break;
            }
        }

        CreatureCreatePos pos(m_creature->GetMap(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation());
        m_creature->SetSummonPoint(pos);
        m_creature->RelocateCreature(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation());

        if(uiPointId != 5 && uiPointId != 2)
        {
            m_bCanMoveNext = true;
            m_uiWaitForOthersTimer = 500;
        }
    }

    void StartWaves()
    {
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

    void UpdateAI(const uint32 uiDiff)
    {

        if(m_uiSetFacing && m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
        {
            m_uiSetFacing = false;
            m_creature->SetFacingTo(m_uiFacing);
        }

        if(m_uiStartTimer)
        {
            if(m_uiStartTimer <= uiDiff)
            {
                m_uiStartTimer = 0;

                StartWaves();
            }
            else
                m_uiStartTimer -= uiDiff;
        }

        if (m_bCanMoveNext && m_uiWaitForOthersTimer <= uiDiff)
        {
            m_bCanMoveNext = false;
            m_creature->GetMotionMaster()->MovePoint(m_uiWaypoint, Andorov[m_uiWaypoint].x, Andorov[m_uiWaypoint].y, Andorov[m_uiWaypoint].z); 
        }
        else
            m_uiWaitForOthersTimer -= uiDiff;

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

    pNewScript = new Script;
    pNewScript->Name = "npc_kaldorei_elite";
    pNewScript->GetAI = &GetAI_npc_kaldorei_elite;
    pNewScript->RegisterSelf();
}
