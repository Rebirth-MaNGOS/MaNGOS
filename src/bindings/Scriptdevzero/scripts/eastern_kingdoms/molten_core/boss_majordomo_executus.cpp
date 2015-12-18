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
SDName: Boss_Majordomo_Executus
SD%Complete: 70
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "escort_ai.h"
#include "molten_core.h"
#include "MapManager.h"

enum eMajordomoExecutus
{
    SAY_AGGRO                   = -1409003,
    SAY_SPAWN                   = -1409004,
    SAY_SLAY                    = -1409005,
    SAY_SPECIAL                 = -1409006,
    SAY_DEFEAT                  = -1409007,
	SAY_DEFEAT2					= -1409019,
	SAY_DEFEAT3					= -1409020,

    SAY_PRESUMMON1              = -1409021,
    SAY_PRESUMMON2              = -1409022,
    SAY_SUMMON_MAJ              = -1409008,
    SAY_ARRIVAL1_RAG            = -1409009,
    SAY_ARRIVAL2_MAJ            = -1409010,
    SAY_ARRIVAL3_RAG            = -1409011,
    SAY_ARRIVAL5_RAG            = -1409012,

    SPELL_MAGIC_REFLECTION      = 20619,
    SPELL_DAMAGE_SHIELD         = 21075,
    SPELL_BLAST_WAVE            = 20229,
    SPELL_AEGIS                 = 20620,                // This is self casted whenever we are below 50%
    SPELL_TELEPORT              = 20618,
    SPELL_TELEPORT_VISUAL       = 19484,
    SPELL_SUMMON_RAGNAROS       = 19774,

    // Flamewaker Elite
    SPELL_FIRE_BLAST            = 20623,
	SPELL_FIREBALL				= 20420,

    // Flamewaker Healer
    SPELL_SHADOW_BOLT           = 21077,
	SPELL_SHADOW_SHOCK			= 20603,
    SPELL_GREATER_HEAL          = 22883,                // need correct pretbc spell id

    // Ragnaros's "underwater" spells
    SPELL_RAGNAROS_EMERGE       = 20568,
    SPELL_RAGNAROS_SUBMERGE     = 21107,
    SPELL_RAGNAROS_SUBMERGE_    = 20567,
};

#define SPAWN_RAG_X             838.51f
#define SPAWN_RAG_Y             -829.84f
#define SPAWN_RAG_Z             -232.00f
#define SPAWN_RAG_O             1.70f

#define GOSSIP_LET_ME_SPEAK     "Let me speak to your master, servant."

struct MANGOS_DLL_DECL boss_majordomoAI : public npc_escortAI
{
    boss_majordomoAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
        m_uiEventPhase = 0;
        m_uiEventTimer = 0;
        m_uiRagnarosSummonPlayerGUID = ObjectGuid();
        m_bCanWalk = false;
        Reset();
    }

    instance_molten_core* m_pInstance;

    uint32 m_addShieldTimer;
    uint32 m_uiBlastwaveTimer;
    uint32 m_uiTeleportTimer;

    uint8 m_uiEventPhase;
    uint32 m_uiEventTimer;

    ObjectGuid m_uiRagnarosSummonPlayerGUID;

    bool m_bCanWalk;

    void Reset()
    {
        m_addShieldTimer = 30000;
        m_uiBlastwaveTimer = urand(10000,15000);
        m_uiTeleportTimer = 30000;  //60000

        if ((m_pInstance && m_pInstance->GetData(TYPE_MAJORDOMO) == DONE) || HasEscortState(STATE_ESCORT_ESCORTING) || HasEscortState(STATE_ESCORT_PAUSED))
            return;

        m_uiEventPhase = 0;
        m_uiEventTimer = 0;

		m_uiRagnarosSummonPlayerGUID.Clear();

        m_bCanWalk = false;

		m_pInstance->SetData(TYPE_MAJORDOMO, FAIL);		
		m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        const CreatureInfo* cinfo = m_creature->GetCreatureInfo();
        m_creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, cinfo->mindmg);
        m_creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, cinfo->maxdmg);
        m_creature->UpdateDamagePhysical(BASE_ATTACK);
    }

    void JustReachedHome()
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_MAJORDOMO) != DONE)
            m_pInstance->SetData(TYPE_MAJORDOMO, FAIL);
    }

    void MovementInform(uint32 movementType, uint32 data)
    {
        if (movementType == POINT_MOTION_TYPE && data == 999)
        {
            float x,y,z,o;
            m_creature->GetRespawnCoord(x,y,z,&o);
            m_creature->Relocate(x,y,z,o);
            m_creature->SendHeartBeat(false);
        }
    }

    void JustStartedEscort()
    {
        //m_bCanWalk = true;
        m_bCanWalk = false;
        m_uiEventPhase = 20;
        m_uiEventTimer = 2000;
    }

    void WaypointReached(uint32 /*uiPointId*/)
    {
        //HACK - removing short walk to the edge of the island until we can fix 
        //the movemaps in rag's chamber
        //switch(uiPointId)
        //{
        //    case 1:
        //        m_bCanWalk = false;
        //        m_uiEventPhase = 20;
        //        m_uiEventTimer = 2000;
        //        break;
        //}
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        if (urand(0, 4))
            return;

        DoScriptText(SAY_SLAY, m_creature);
    }

	void DamageTaken(Unit* pDoneBy, uint32& /*uiDamage*/)
	{
		Player* attacking_player = dynamic_cast<Player*>(pDoneBy);
		if (m_pInstance && attacking_player)
		{
			// If a GM damages Executus we force him to turn visible.
			if (attacking_player->isGameMaster())
				m_pInstance->DoSpawnMajordomoIfCan(false, true);
		}

	}


    void Aggro(Unit* /*pWho*/)
    {
        m_creature->CallForHelp(40.0f);
		this->m_pInstance->DoOrSimulateScriptTextForThisInstance(SAY_AGGRO,m_creature->GetEntry());

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAJORDOMO, IN_PROGRESS);
    }

    Creature* GetRagnaros()
    {
        if (!m_pInstance)
            return NULL;

		Creature* pRagnaros = m_pInstance->GetSingleCreatureFromStorage(NPC_RAGNAROS);

        return pRagnaros ? pRagnaros : NULL;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiEventTimer)
        {
            if (m_uiEventPhase >= 20)
            {
                Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiRagnarosSummonPlayerGUID);
                if (!pPlayer || (pPlayer && pPlayer->isDead()))
                {
                    // cancel summoning event
                    m_creature->SetHealth(0);
                    m_creature->SetDeathState(JUST_DIED);
                    return;
                }
            }

            if (m_uiEventTimer <= uiDiff)
            {
                m_uiEventTimer = 0;

                switch(m_uiEventPhase)
                {
                    // Event #1: Teleport
					case 5:
						m_uiEventTimer = 7000;
						break;
					case 6:
						DoScriptText(SAY_DEFEAT2,m_creature);
						m_uiEventTimer=8500;
						m_uiEventPhase=0;
						break;
                    case 1:
						DoScriptText(SAY_DEFEAT3, m_creature);
                        m_uiEventTimer = 16000; // ~35000
                        break;
                    case 2:
						{
                        DoCastSpellIfCan(m_creature, SPELL_TELEPORT_VISUAL);
                        m_uiEventTimer = 1300;
                        break;
						}
                    case 3:
						{
                            m_creature->SetVisibility(VISIBILITY_OFF);
							//TODO: SetHomePosition
							m_creature->NearTeleportTo(852.95f, -819.74f, -229.0f, 3.70f);
							CellPair p = MaNGOS::ComputeCellPair(m_creature->GetPositionX(), m_creature->GetPositionY());
							Cell cell(p);
							m_creature->GetMap()->LoadGrid(cell);
							m_uiEventTimer=1;
							break;
						}
                    case 4:
                        m_creature->SetVisibility(VISIBILITY_ON);
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        m_uiEventPhase = 0;
                        return;
                        //break;

                    // Event #2: To talk with Ragnaros
                    case 20:
                        if (Creature* pRagnaros = GetRagnaros())
                        {
                            //m_creature->SetFacingToObject(pRagnaros);
                            pRagnaros->setFaction(FACTION_FRIENDLY);
                            pRagnaros->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            m_creature->SetFacingToObject(m_creature);
                            pRagnaros->CastSpell(pRagnaros, SPELL_RAGNAROS_SUBMERGE_, true);
                            pRagnaros->CastSpell(pRagnaros, SPELL_RAGNAROS_SUBMERGE,  true);
                        }
                        m_creature->CastSpell(m_creature,19774,false);
                        m_uiEventTimer = 3000;
                        break;
                    case 21:
                        DoScriptText(SAY_PRESUMMON2,m_creature);
                        m_uiEventTimer = 9000;
                        break;
                    case 22:
                        if (Creature* pRagnaros = GetRagnaros())
                            pRagnaros->SetVisibility(VISIBILITY_ON);
                        DoScriptText(SAY_SUMMON_MAJ, m_creature);
                        m_uiEventTimer = 12000;
                        break;
                    case 23:
                        if (Creature* pRagnaros = GetRagnaros())
                        {
                            pRagnaros->RemoveAurasDueToSpell(SPELL_RAGNAROS_SUBMERGE);
                            pRagnaros->CastSpell(pRagnaros, SPELL_RAGNAROS_EMERGE, false);
                        }
                        m_uiEventTimer = 3000;  // maybe 7000 will be better
                        break;
                    case 24:
                        if (Creature* pRagnaros = GetRagnaros())
                            pRagnaros->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                        m_uiEventTimer = 4000;
                        break;
                    case 25:
                        if (Creature* pRagnaros = GetRagnaros())
                            DoScriptText(SAY_ARRIVAL1_RAG, pRagnaros);
                        m_uiEventTimer = 13000;
                        break;
                    case 26:
                        DoScriptText(SAY_ARRIVAL2_MAJ, m_creature);
                        m_uiEventTimer = 10000;
                        break;
                    case 27:
                        if (Creature* pRagnaros = GetRagnaros())
                            DoScriptText(SAY_ARRIVAL3_RAG, pRagnaros);
                        m_uiEventTimer = 10000;
                        break;
                    case 28:
                        if (Creature* pRagnaros = GetRagnaros())
                            pRagnaros->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                        m_uiEventTimer = 7500;
                        break;
                    case 29:
                        if (Creature *pRagnaros = GetRagnaros())
                        {
                            pRagnaros->CastSpell(m_creature,19773,false);
                        }
                        m_uiEventTimer = 1500;
                        break;
                    case 30:
                        // Majordomo died (fake corpse)
                        m_creature->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                        m_uiEventTimer = 4000;
                        break;
                    case 31:
                        if (Creature* pRagnaros = GetRagnaros())
                        {
                            DoScriptText(SAY_ARRIVAL5_RAG, pRagnaros);

                            // Ragnaros look at his awakener (maybe not to be)
                            if (m_uiRagnarosSummonPlayerGUID)
                                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiRagnarosSummonPlayerGUID))
                                    pRagnaros->SetFacingToObject(pPlayer);
                        }
                        m_uiEventTimer = 12000; // ~15000
                        break;
                    case 32:
                        if (Creature* pRagnaros = GetRagnaros())
                        {
                            pRagnaros->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            pRagnaros->setFaction(pRagnaros->GetCreatureInfo()->faction_A);
                            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiRagnarosSummonPlayerGUID))
                                pRagnaros->AI()->AttackStart(pPlayer);
                        }
                        m_uiEventTimer = 5000;
                        break;
                    case 33:
                        // To complete escort.
                        m_uiEventPhase = 0;
						m_uiRagnarosSummonPlayerGUID.Clear();
                        m_bCanWalk = true;
                        return;
                        //break;
                }
                ++m_uiEventPhase;
            }
            else
                m_uiEventTimer -= uiDiff;
        }

        if (m_bCanWalk)
            npc_escortAI::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
            
        // When all adds of Majordomo are dead, evade and teleport to the Ragnaros's cave
        if (m_pInstance)
        {
            if (m_pInstance->GetData(TYPE_MAJORDOMO) == SPECIAL)
            {
				m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				m_creature->setFaction(FACTION_FRIENDLY);

                m_pInstance->SetData(TYPE_MAJORDOMO, DONE);
                m_pInstance->SetData(TYPE_MAJORDOMO_FIRST_SPAWN, DONE); // Chest will be spawned only one time
				m_pInstance->DoOrSimulateScriptTextForThisInstance(SAY_DEFEAT,m_creature->GetEntry());
    
                m_creature->RemoveAllAuras();
                m_creature->DeleteThreatList();
                m_creature->CombatStop(true);
                m_creature->SetLootRecipient(NULL);

                if (m_creature->isAlive())
                {
                    float homeX,homeY,homeZ;
                    m_creature->GetRespawnCoord(homeX,homeY,homeZ);
                    m_creature->GetMotionMaster()->MovePoint(999,homeX,homeY,homeZ);
                }

                m_creature->SetLootRecipient(NULL);
    
                m_uiEventPhase = 5;
                m_uiEventTimer = 1;
                return;
            }
        }
        //else
        //    error_log("SD0: Molten Core: m_pinstance is null, majordomo event will never end.");

        // Cast Aegis if less than 50% hp
        if (HealthBelowPct(50))
            DoCastSpellIfCan(m_creature, SPELL_AEGIS);

        // Magic Reflection
        if (m_addShieldTimer <= uiDiff)
        {
			m_addShieldTimer = 0;

			uint32 spell = SPELL_MAGIC_REFLECTION;

			if (urand(0,1) == 0)
				spell = SPELL_DAMAGE_SHIELD;

            if (DoCastSpellIfCan(m_creature, spell) == CAST_OK)
				m_addShieldTimer = 30000;
        }
        else
            m_addShieldTimer -= uiDiff;

        // Blast Wave
        if (m_uiBlastwaveTimer <= uiDiff)
        {
			m_uiBlastwaveTimer = 0;

            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLAST_WAVE) == CAST_OK)
				m_uiBlastwaveTimer = urand(25000,30000);
        }
        else
            m_uiBlastwaveTimer -= uiDiff;

        // Teleport (player at burning coals)
        if (m_uiTeleportTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_TELEPORT);
            }
            m_uiTeleportTimer = urand(5000, 30000);
        }
        else
            m_uiTeleportTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_majordomo(Creature* pCreature)
{
    return new boss_majordomoAI(pCreature);
}

bool GossipHello_boss_majordomo(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tell me more.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
	pPlayer->SEND_GOSSIP_MENU(4995, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_boss_majordomo(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What else do you have to say?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
	    pPlayer->SEND_GOSSIP_MENU(5011, pCreature->GetObjectGuid());
    } else if (uiAction == GOSSIP_ACTION_INFO_DEF + 2)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "You challenged us and we have come.  Where is this master you speak of?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        pPlayer->SEND_GOSSIP_MENU(5012, pCreature->GetObjectGuid());
   } else if (uiAction == GOSSIP_ACTION_INFO_DEF + 3)
    {
        if (boss_majordomoAI* pDomoAI = dynamic_cast<boss_majordomoAI*>(pCreature->AI()))
        {
            pPlayer->CLOSE_GOSSIP_MENU();
            pDomoAI->m_uiRagnarosSummonPlayerGUID = pPlayer->GetObjectGuid();
            pDomoAI->Start(false, 0, 0);
            //DoScriptText(SAY_PRESUMMON1,pCreature,pPlayer);
        }
    }
    return true;
}

/*######
## mob_flamewaker_healer
######*/

struct MANGOS_DLL_DECL mob_flamewaker_healerAI : public ScriptedAI
{
    mob_flamewaker_healerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiShadowBoltTimer;
    uint32 m_uiGreaterHealTimer;
	uint32 m_shadowShockTimer;

    void Aggro(Unit* /*pWho*/)
    {
        m_creature->CallForHelp(40.0f);
    }

    void Reset()
    {
        m_uiShadowBoltTimer = urand(3000, 7000);
        m_uiGreaterHealTimer = 10000;
		m_shadowShockTimer = urand(10000,15000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

         // Greater Heal
        if (m_uiGreaterHealTimer <= uiDiff)
        {
			m_uiGreaterHealTimer = 0;
			Unit* pTarget = DoSelectLowestHpFriendly(40.0f, 1000);
            if ( pTarget && DoCastSpellIfCan(pTarget, SPELL_GREATER_HEAL) == CAST_OK)
                m_uiGreaterHealTimer = urand(8000,12000);
        }
        else
            m_uiGreaterHealTimer -= uiDiff;

		// Shadow Shock
		if (m_shadowShockTimer <= uiDiff)
		{
			m_shadowShockTimer = 0;

			if (DoCastSpellIfCan(m_creature, SPELL_SHADOW_SHOCK) == CAST_OK)
				m_shadowShockTimer = urand(25000,30000);
		} else
			m_shadowShockTimer -= uiDiff;
		
		// Shadow Bolt
        if (m_uiShadowBoltTimer <= uiDiff)
        {
			m_uiShadowBoltTimer = 0;

			Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
			if (pTarget && DoCastSpellIfCan(pTarget, SPELL_SHADOW_BOLT) == CAST_OK)
				m_uiShadowBoltTimer = urand(3500,6000);
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_flamewaker_healer(Creature* pCreature)
{
    return new mob_flamewaker_healerAI(pCreature);
}

/*######
## mob_flamewaker_elite
######*/

struct MANGOS_DLL_DECL mob_flamewaker_eliteAI : public ScriptedAI
{
    mob_flamewaker_eliteAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiBlastWaveTimer;
    uint32 m_uiFireBlastTimer;
	uint32 m_uiFireballTimer;

    void Aggro(Unit* /*pWho*/)
    {
        m_creature->CallForHelp(40.0f);
    }

    void Reset()
    {
        m_uiBlastWaveTimer = urand(10000,15000);
        m_uiFireBlastTimer = urand(10000,15000);
		m_uiFireballTimer = urand(10000,15000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Fire Blast
        if (m_uiBlastWaveTimer <= uiDiff)
        {
			m_uiBlastWaveTimer = 0;
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIRE_BLAST) == CAST_OK)
				m_uiBlastWaveTimer = urand(25000,30000);
        }
        else
            m_uiBlastWaveTimer -= uiDiff;

        // Blast Wave
        if (m_uiFireBlastTimer <= uiDiff)
        {
			m_uiFireBlastTimer = 0;
            if (DoCastSpellIfCan(m_creature, SPELL_BLAST_WAVE) == CAST_OK)
				m_uiFireBlastTimer = 10000;
        }
        else
            m_uiFireBlastTimer -= uiDiff;

		//Fireball
		if (m_uiFireballTimer <= uiDiff)
		{
			m_uiFireballTimer = 0;
			Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
			if (pTarget && DoCastSpellIfCan(pTarget, SPELL_FIREBALL) == CAST_OK)
				m_uiFireballTimer = urand(10000,15000);
		} else 
			m_uiFireballTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_flamewaker_elite(Creature* pCreature)
{
    return new mob_flamewaker_eliteAI(pCreature);
}

void AddSC_boss_majordomo()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_majordomo";
    pNewScript->GetAI = &GetAI_boss_majordomo;
    pNewScript->pGossipHello = &GossipHello_boss_majordomo;
    pNewScript->pGossipSelect = &GossipSelect_boss_majordomo;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_flamewaker_healer";
    pNewScript->GetAI = &GetAI_mob_flamewaker_healer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_flamewaker_elite";
    pNewScript->GetAI = &GetAI_mob_flamewaker_elite;
    pNewScript->RegisterSelf();
}
