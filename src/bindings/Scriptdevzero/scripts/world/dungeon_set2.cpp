#include "precompiled.h"
#include "TemporarySummon.h"
#include "CreatureEventAI.h"

enum
{
	NPC_SPECTRAL_STALKER = 16093,

	ALLIANCE_PROPOSITION_MIN = 8905,
	ALLIANCE_PROPOSITION_MAX = 8912,
	HORDE_PROPOSITION_MIN = 8913,
	HORDE_PROPOSITION_MAX = 8920
};

class PropositionQuestGiverAI : public CreatureEventAI
{
private:
	Player *m_questHolder;
	bool m_combatScriptActive;
	bool m_postCombatScriptActive;
	uint32 m_scriptTime;

	int mStalkerAttackMsg;
	int mAttackResponseMsg;
	int mExplainMsg1;
	int mExplainMsg2;
	int mExplainMsg3;
public:
	PropositionQuestGiverAI(Creature *pCreature, int stalkerAttackMsg, int attackResponseMsg, int explainMsg1, int explainMsg2, int explainMsg3)
		:CreatureEventAI(pCreature), m_questHolder(NULL), m_combatScriptActive(false), m_postCombatScriptActive(false), m_scriptTime(0),
		mStalkerAttackMsg(stalkerAttackMsg), mAttackResponseMsg(attackResponseMsg), mExplainMsg1(explainMsg1), mExplainMsg2(explainMsg2),
		mExplainMsg3(explainMsg3)
	{}

	void SummonedCreatureJustDied(Creature *pCreature)
	{
		CreatureEventAI::SummonedCreatureJustDied(pCreature);

		RunScriptIfNecessary(pCreature);
	}

	void CompletedQuest(Player *pPlayer)
	{
		this->m_questHolder = pPlayer;
		m_combatScriptActive = true;
		m_scriptTime = 0;
		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
	}

	void UpdateAI(const uint32 diff)
	{
		CreatureEventAI::UpdateAI(diff);

		if (m_combatScriptActive && m_creature->getVictim() != NULL)
		{
			uint32 newScriptTime = m_scriptTime + diff;

			//Your fate is sealed!
			if (newScriptTime >= 1500 && m_scriptTime < 1500)
				DoScriptText(mStalkerAttackMsg, m_creature->getVictim(), m_questHolder);

			//You're never going to stop...
			if (newScriptTime >= 5000 && m_scriptTime < 5000)
				DoScriptText(mAttackResponseMsg,m_creature,m_questHolder);

			if (newScriptTime >= 5000)
			{
				m_combatScriptActive = false;
				m_scriptTime = 0;
			} else
				m_scriptTime = newScriptTime;
		}

		if (m_postCombatScriptActive)
		{
			uint32 newScriptTime = m_scriptTime + diff;

			//I never meant to drag you into this
			if (newScriptTime >= 2000 && m_scriptTime < 2000)
				DoScriptText(mExplainMsg1, m_creature, m_questHolder);

			//Trying to find out about my predicament..
			if (newScriptTime >= 8000 && m_scriptTime < 8000)
				DoScriptText(mExplainMsg2, m_creature, m_questHolder);

			//Come to think about it...
			if (newScriptTime >= 16000 && m_scriptTime < 16000)
				DoScriptText(mExplainMsg3, m_creature, m_questHolder);

			if (newScriptTime >= 18000)
			{
				m_postCombatScriptActive = false;
				m_scriptTime = 0;
				m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
			} else
				m_scriptTime = newScriptTime;
		}
	}
	
protected:
	void RunScriptIfNecessary(Creature *pCreature)
	{
		if (pCreature->GetEntry() == NPC_SPECTRAL_STALKER && m_Phase == 3 && m_questHolder != NULL)
		{
			m_postCombatScriptActive = true;
		}
	}
};

bool Alliance_Earnest_Proposition_Complete(Player *pPlayer, Creature *pDeliana, const Quest *pQuest)
{
	if (pQuest->GetQuestId() < ALLIANCE_PROPOSITION_MIN || pQuest->GetQuestId() > ALLIANCE_PROPOSITION_MAX)
		return false;

	Creature *pAttacker = pDeliana->SummonCreature(NPC_SPECTRAL_STALKER, -4844.16f,-1058.47f,502.19f,3.114f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,60000,true);
	if (pAttacker->AI() != NULL)
		pAttacker->AI()->AttackStart(pDeliana);

	pAttacker = pDeliana->SummonCreature(NPC_SPECTRAL_STALKER, -4840.16f,-1055.47f,502.19f,3.114f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,60000,true);
	if (pAttacker->AI() != NULL)
		pAttacker->AI()->AttackStart(pDeliana);

	pAttacker = pDeliana->SummonCreature(NPC_SPECTRAL_STALKER, -4846.16f,-1054.47f,502.19f,3.114f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,60000,true);
	if (pAttacker->AI() != NULL)
		pAttacker->AI()->AttackStart(pDeliana);

	PropositionQuestGiverAI *deliana = dynamic_cast<PropositionQuestGiverAI*>(pDeliana->AI());

	if (deliana != NULL)
	{
		deliana->CompletedQuest(pPlayer);
	}

	return true;
}

CreatureAI *npc_Deliana(Creature *pCreature)
{
	return new PropositionQuestGiverAI(pCreature,-2001,-2002,-2004,-2005,-2006);
}

bool Horde_Earnest_Proposition_Complete(Player *pPlayer, Creature *pMokvar, const Quest *pQuest)
{
	if (pQuest->GetQuestId() < HORDE_PROPOSITION_MIN || pQuest->GetQuestId() > HORDE_PROPOSITION_MAX)
		return false;

	Creature *pAttacker = pMokvar->SummonCreature(NPC_SPECTRAL_STALKER, 1920.6f,-4172.34f,40.914f,3.029f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,60000,true);
	if (pAttacker->AI() != NULL)
		pAttacker->AI()->AttackStart(pMokvar);

	pAttacker = pMokvar->SummonCreature(NPC_SPECTRAL_STALKER, 1918.48f,-4167.32f,40.916f,4.1f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,60000,true);
	if (pAttacker->AI() != NULL)
		pAttacker->AI()->AttackStart(pMokvar);

	pAttacker = pMokvar->SummonCreature(NPC_SPECTRAL_STALKER, 1912.727f,-4168.2f,40.928f,5.546f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,60000,true);
	if (pAttacker->AI() != NULL)
		pAttacker->AI()->AttackStart(pMokvar);

	PropositionQuestGiverAI *mokvar = dynamic_cast<PropositionQuestGiverAI*>(pMokvar->AI());

	if (mokvar != NULL)
	{
		mokvar->CompletedQuest(pPlayer);
	}

	return true;
}

CreatureAI *npc_Mokvar(Creature *pCreature)
{
	return new PropositionQuestGiverAI(pCreature,-2007,-2008,-2010,-2011,-2012);
}

void AddSC_dungeon_set2()
{
	Script *pNewScript = new Script();
	pNewScript->Name = "npc_deliana";
	pNewScript->GetAI = &npc_Deliana;
	pNewScript->pQuestRewardedNPC = &Alliance_Earnest_Proposition_Complete;
	pNewScript->RegisterSelf();

	pNewScript = new Script();
	pNewScript->Name = "npc_mokvar";
	pNewScript->GetAI = &npc_Mokvar;
	pNewScript->pQuestRewardedNPC = &Horde_Earnest_Proposition_Complete;
	pNewScript->RegisterSelf();
}

