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
SDName: Maraudon
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "precompiled.h"
#include "escort_ai.h"
#include "maraudon.h"

enum Enums
{
    SAY_ESCORT_READY                  = -1349000,
    SAY_ESCORT_PROGRESS               = -1349001,
    SAY_SCEPTER_START                 = -1349002,
    SAY_SCEPTER_PROGRESS_1            = -1349003,
    SAY_SCEPTER_PROGRESS_2            = -1349005,
    SAY_SCEPTER_PROGRESS_3            = -1349006,
    SAY_SCEPTER_FINISH                = -1349007,
    
    //EMOTE_SCEPTER_START               = -1349004,

	GO_MARAUDON_STAFF_CREATOR		  = 178560,
	GO_CELEBRAS_BLUEAURA			  = 178964,
	GO_INCANTATION_OF_CELEBRAS		  = 178965,
    
    QUEST_THE_SCEPTER_OF_CELEBRAS     = 7046,
    SPELL_CREATE_SCEPTER              = 21939,
	SPELL_CHANNELING_VISUAL           = 13236,
	SPELL_REJUV						  = 774,
};

struct MANGOS_DLL_DECL npc_celebras_the_redeemedAI : public npc_escortAI
{
    npc_celebras_the_redeemedAI(Creature *pCreature) : npc_escortAI(pCreature) {Reset();}
    
	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;
	bool m_bOutro;

    bool Escort;

    void Reset()
    {
        Escort = true;
		m_bOutro = false;
		m_uiSpeechStep = 1;
		m_uiSpeechTimer = 0;
    }

    void WaypointReached(uint32 uiWaypointId)
    {
        Player* pPlayer = GetPlayerForEscort();
        if (!pPlayer)
            return;
		GameObject* GoStaff = GetClosestGameObjectWithEntry(m_creature, GO_MARAUDON_STAFF_CREATOR, 20.0f);
        switch (uiWaypointId)
        {
            case 0:
                m_creature->SetSplineFlags(SPLINEFLAG_WALKMODE);
                break;
			case 1:
				DoScriptText(SAY_SCEPTER_START, m_creature, pPlayer);
				break;       
            case 4:
                DoScriptText(SAY_SCEPTER_FINISH, m_creature);
				if (GoStaff->GetGoState() != GO_STATE_ACTIVE_ALTERNATIVE)
                    GoStaff->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                break;	
            case 9:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
                if (pPlayer && pPlayer->GetTypeId() == TYPEID_PLAYER)
                    pPlayer->GroupEventHappens(QUEST_THE_SCEPTER_OF_CELEBRAS, m_creature);
                break;
        }
    }

    void Aggro(Unit* /*who*/)
    {
    }

    void JustDied(Unit* /*killer*/)
    {
        if (Player* pPlayer = GetPlayerForEscort())
            pPlayer->FailQuest(QUEST_THE_SCEPTER_OF_CELEBRAS);
    }
	void StartOutro()
	{
		m_bOutro = true; 
		m_uiSpeechTimer = 12500;
		m_uiSpeechStep = 1;
		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
		m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
	}

	void UpdateAI(const uint32 uiDiff)
    {
        if (Escort)
            npc_escortAI::UpdateAI(uiDiff);
        
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }

	void UpdateEscortAI(const uint32 uiDiff)
    {
		if (m_uiSpeechTimer && m_bOutro)
		{
			if (!m_uiSpeechStep)
				return;
		
			if (m_uiSpeechTimer <= uiDiff)
            {
				GameObject* GoBook = GetClosestGameObjectWithEntry(m_creature, GO_INCANTATION_OF_CELEBRAS, 10.0f);
                switch(m_uiSpeechStep)
                {
					case 1:
						m_creature->SetFacingTo(4.607f);
						m_uiSpeechTimer = 2500;
						break;
					case 2:
						DoScriptText(SAY_ESCORT_PROGRESS, m_creature);
                        m_uiSpeechTimer = 11500;
						break;
					case 3:						
						m_creature->SetFacingTo(2.92f);
						m_creature->SummonGameObject(GO_INCANTATION_OF_CELEBRAS,36000,652.42f,74.18f,-85.33f,0);
						if (GoBook)
						{
							GoBook->SetGoState(GO_STATE_READY);
							GoBook->SetLootState(GO_READY);	
						}
						m_uiSpeechTimer = 1000;
						break;
					case 4:
						DoScriptText(SAY_SCEPTER_PROGRESS_1, m_creature);
						m_uiSpeechTimer = 3000;
						break;
					case 5:
						m_creature->CastSpell(m_creature, SPELL_REJUV, true);
						m_creature->CastSpell(m_creature,SPELL_CHANNELING_VISUAL, true);			// makes him glide instead of walk even after removed				
						m_creature->GenericTextEmote("Celebras the Redeemed begins to channel his energy, focusing on the stone.", NULL, false);
						m_uiSpeechTimer = 3000;
						break;
					case 6:
						m_creature->CastSpell(m_creature, SPELL_REJUV, true);
						m_uiSpeechTimer = 3000;
						break;
					case 7:
						DoScriptText(SAY_SCEPTER_PROGRESS_2, m_creature);
						m_uiSpeechTimer = 3000;
						break;
					case 8:
						m_creature->CastSpell(m_creature, SPELL_REJUV, true);
						m_uiSpeechTimer = 3000;
						break;
					case 9:
						m_creature->CastSpell(m_creature, SPELL_REJUV, true);
						m_uiSpeechTimer = 3000;
						break;
					case 10:
						m_creature->RemoveAllAuras();
						m_creature->SetStandState(UNIT_STAND_STATE_STAND);			// made him glide slightly less
						DoScriptText(SAY_SCEPTER_PROGRESS_3, m_creature);
						m_creature->SummonGameObject(GO_CELEBRAS_BLUEAURA,15000,650.99f,74.458f,-86.8651f,0);
						m_uiSpeechTimer = 15000;
						break;
					case 11:
						m_creature->SetFacingTo(0.46f);
						m_uiSpeechTimer = 15000;
						break;
					case 12:
						m_creature->SetFacingTo(5.78f);
						m_uiSpeechTimer = 1000;
						break;
					case 13:
						m_bOutro = false;
						break;
                    /*default:
                        m_uiSpeechStep = 0;
                        return;*/
                }
                ++m_uiSpeechStep;
            }
            else
                m_uiSpeechTimer -= uiDiff;
		}
	}   
};

CreatureAI* GetAI_npc_celebras_the_redeemed(Creature *pCreature)
{
    return new npc_celebras_the_redeemedAI (pCreature);
}

bool QuestAccept_npc_celebras_the_redeemed(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_THE_SCEPTER_OF_CELEBRAS)
	{
		if (npc_celebras_the_redeemedAI* pEscortAI = dynamic_cast<npc_celebras_the_redeemedAI*>(pCreature->AI()))
		{
			pEscortAI->Start(false, pPlayer, pQuest, true);
			pEscortAI->StartOutro();
			DoScriptText(SAY_ESCORT_READY, pCreature, pPlayer);
		}
	}
	return true;
}

enum GIZLOCK_SPELLS
{
	SPELL_BLITZSTRAHLBOMBE			= 29419,
	SPELL_BOMBE						= 9143,
	SPELL_GOBLINDRACHENWERFER		= 21833,
	SPELL_SCHIESSEN					= 16100
};

struct MANGOS_DLL_DECL boss_tueftler_gizlockAI : public ScriptedAI
{
    boss_tueftler_gizlockAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	uint32 blitzstrahl_timer;
	uint32 bombe_timer;
	uint32 werfer_timer;

    void Reset()
    {
		blitzstrahl_timer = urand(30000,45000);
		bombe_timer = urand(12000,20000);
		werfer_timer = urand(8000,25000);
    }

	void shoot()
	{
		if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
			m_creature->CastSpell(pTarget,SPELL_SCHIESSEN,true);
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (blitzstrahl_timer < uiDiff)
		{
			m_creature->CastSpell(m_creature->getVictim(),SPELL_BLITZSTRAHLBOMBE,true);
			blitzstrahl_timer = urand(30000,45000);
			shoot();
		}
		else
			blitzstrahl_timer -= uiDiff;

		if (bombe_timer < uiDiff)
		{
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
			{
				m_creature->CastSpell(pTarget,SPELL_BOMBE,true);
				bombe_timer = urand(12000,20000);
				shoot();
			}
		}
		else
			bombe_timer -= uiDiff;

		if (werfer_timer < uiDiff)
		{
			if(DoCastSpellIfCan(m_creature->getVictim(),SPELL_GOBLINDRACHENWERFER) == CAST_OK)
			{
				werfer_timer = urand(8000,25000);
				shoot();
			}
		}
		else
			werfer_timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

enum SCHLANGENZUNGE_SPELLS
{
	SPELL_BLINZELN				= 21655,
	SPELL_MEHRFACHSCHUSS		= 21390,
	SPELL_RAUCHBOMBE			= 7964,

	NPC_SHADOWSTALKER			= 11792,
	ADD1						= 54675,
	ADD2						= 54676
};	

struct MANGOS_DLL_DECL boss_lord_schlangenzungeAI : public ScriptedAI
{
    boss_lord_schlangenzungeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_pInstance = (instance_maraudon*)pCreature->GetInstanceData();
        Reset();
    }

	instance_maraudon* m_pInstance;

	uint32 blinzeln_timer;
	uint32 schuss_timer;
	uint32 bombe_timer;

    void Reset()
    {
		blinzeln_timer = urand(15000,30000);
		schuss_timer = urand(8000,18000);
		bombe_timer = urand(12000,25000);

		ReviveAdds();
    }

	void Aggro(Unit* /*pwho*/)
    {
		m_creature->CallForHelp(40.0f);
    }

	void ReviveAdds()
	{
		std::list<Creature*> ADList;
		GetCreatureListWithEntryInGrid(ADList,m_creature,NPC_SHADOWSTALKER,40.0f);
		for(std::list<Creature*>::iterator i = ADList.begin(); i != ADList.end(); ++i)
		{
			if(Creature* pTemp = *i)
			{
				if(pTemp->GetGUID() == ADD1 || pTemp->GetGUID() == ADD2)
				{
					if(pTemp->isDead())
						pTemp->Respawn();
				}
			}
		}
	}

	void shoot()
	{
		if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
			m_creature->CastSpell(pTarget,SPELL_SCHIESSEN,true);
	}

	void UpdateAI(const uint32 uiDiff)
    {
        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (blinzeln_timer < uiDiff)
		{
			if(DoCastSpellIfCan(m_creature,SPELL_BLINZELN) == CAST_OK)
			{
				blinzeln_timer = urand(15000,30000);
				shoot();
			}
		}
		else
			blinzeln_timer -= uiDiff;

		if (schuss_timer < uiDiff)
		{
			if(DoCastSpellIfCan(m_creature->getVictim(),SPELL_MEHRFACHSCHUSS) == CAST_OK)
			{
				schuss_timer = urand(8000,18000);
				shoot();
			}
		}
		else
			schuss_timer -= uiDiff;

		if (bombe_timer < uiDiff)
		{
			shoot();
			if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
			{
				if(DoCastSpellIfCan(pTarget,SPELL_RAUCHBOMBE) == CAST_OK)
				{
					bombe_timer = urand(12000,25000);
					shoot();
				}
			}
		}
		else
			bombe_timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lord_schlangenzunge(Creature* pCreature)
{
    return new boss_lord_schlangenzungeAI(pCreature);
}

CreatureAI* GetAI_boss_tueftler_gizlock(Creature* pCreature)
{
    return new boss_tueftler_gizlockAI(pCreature);
}

/*######
## mob_theradrim_guardian
######*/

enum aTheradrimGuardian
{
	NPC_THERADRIM_SHARDLING		= 11783,
	SPELL_KNOCKDOWN				= 16790,
};

struct MANGOS_DLL_DECL mob_theradrim_guardianAI : public ScriptedAI
{
    mob_theradrim_guardianAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

	uint32 m_uiKnockdownTimer;
	uint32 m_uiSummonCount;

    void Reset()
    {
		m_uiKnockdownTimer = 5000;
		m_uiSummonCount = 0;
    }

	void JustSummoned(Creature* pSummoned)	
    {
        ++m_uiSummonCount;
	}

	void JustDied(Unit* /*pKiller*/)			// Spawn adds on death
    {
		if (m_uiSummonCount < 3)	// max 3 Shardlings
        {
			float fX, fY, fZ;
            m_creature->GetPosition(fX, fY, fZ);
			for(uint8 i = 0; i < 3; ++i)
				if (Creature* pShardling = m_creature->SummonCreature(NPC_THERADRIM_SHARDLING, fX+irand(-3,3), fY+irand(-3,3), fZ, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000))
				{
					pShardling->AI()->AttackStart(m_creature->getVictim());
					pShardling->SetRespawnDelay(-10);				// to stop them from randomly respawning
				}
		}
	}

	void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		// Knockdown timer
        if (m_uiKnockdownTimer <= uiDiff)
        {
			Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0);
			if (pTarget)
				DoCastSpellIfCan(pTarget, SPELL_KNOCKDOWN);
			m_uiKnockdownTimer = urand(7000, 12000);
		}
		else 
			m_uiKnockdownTimer -= uiDiff;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_theradrim_guardian(Creature* pCreature)
{
    return new mob_theradrim_guardianAI(pCreature);
}

void AddSC_maraudon()
{
    Script *newscript;
        
    newscript = new Script;
    newscript->Name="npc_celebras_the_redeemed";
    newscript->pQuestAcceptNPC = &QuestAccept_npc_celebras_the_redeemed; 
    newscript->GetAI = &GetAI_npc_celebras_the_redeemed;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_tueftler_gizlock";
    newscript->GetAI = &GetAI_boss_tueftler_gizlock;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_lord_schlangenzunge";
    newscript->GetAI = &GetAI_boss_lord_schlangenzunge;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "mob_theradrim_guardian";
	newscript->GetAI = &GetAI_mob_theradrim_guardian;
	newscript->RegisterSelf();
}
