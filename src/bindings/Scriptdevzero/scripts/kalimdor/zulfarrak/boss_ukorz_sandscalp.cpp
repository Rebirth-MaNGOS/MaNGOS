//Created by Baum

/*
### PROJEKTMAPPE ###
CPP Datei hinzufügen
### SCRIPTLOADER ###
extern void AddSC_boss_ukorz_sandscalp();
AddSC_boss_ukorz_sandscalp();
### SQL ###
update creature_template set ScriptName = "boss_ukorz_sandscalp_AND_ruuzlu"
where entry = 7267;
update creature_template set ScriptName = "boss_ukorz_sandscalp_AND_ruuzlu"
where entry = 7797;
*/

//SAY EMOTES are missing

#include "precompiled.h"
#include "zulfarrak.h"

enum DATA
{
	SPELL_WEITER_STREICH			= 11837,
	SPELL_SPALTEN					= 15496,
	SPELL_RASEREI					= 8269,
	SPELL_HINRICHTEN				= 7160,
	SPELL_BERSERKERHALTUNG			= 7366,

	NPC_UKORZ_SANDSCALP				= 7267,
	NPC_RUUZLU						= 7797
};

struct MANGOS_DLL_DECL boss_ukorz_sandscalpAI : public ScriptedAI
{
    boss_ukorz_sandscalpAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DAZE, true);
        m_pInstance = (instance_zulfarrak*)pCreature->GetInstanceData();

        Reset();
    }

	instance_zulfarrak* m_pInstance;

	uint32 streich_timer;
	uint32 spalten_timer;
	uint32 hinrichten_timer;
	bool getBossOrAdd;
	bool BossScript;
	bool enrage;

	Creature* mFriend;

    void Reset()
	{
		m_creature->SetVisibility(VISIBILITY_ON);
		streich_timer = urand(12000,20000);
		spalten_timer = urand(5000,15000);
		hinrichten_timer = 1000;
		getBossOrAdd = false;
		BossScript = false;
		enrage = false;
    }

	void Aggro(Unit* /*pVictim*/)
    {
		if (!getBossOrAdd)
		{
			if (m_creature->GetEntry() == NPC_UKORZ_SANDSCALP)
				BossScript = true;
			getBossOrAdd = true;
		}
		m_creature->CastSpell(m_creature,SPELL_BERSERKERHALTUNG,true);
		m_creature->CallForHelp(15.0f);
		if (BossScript)
			mFriend = GetFriend(NPC_RUUZLU);
		else
			mFriend = GetFriend(NPC_UKORZ_SANDSCALP);

		GUIDList L = m_pInstance->m_uiSnakes;
		Map* pMap = m_creature->GetMap();
		if (pMap && !L.empty())
		{
			for (GUIDList::iterator itr = L.begin(); itr != L.end(); itr++)
			{
				Creature* snake = pMap->GetCreature(*itr);
				if (snake && snake->IsInWorld() && snake->isAlive())
					snake->SetInCombatWithZone();
			}
		}
    }

	void JustReachedHome()
    {
        if (BossScript)
			ReviveFriendIfDead(NPC_RUUZLU);
		else
			ReviveFriendIfDead(NPC_UKORZ_SANDSCALP);
    }

	void JustDied(Unit* /*pKiller*/)
    {
		if (BossScript)
			GoLoot(NPC_RUUZLU);
		else
			GoLoot(NPC_UKORZ_SANDSCALP);			
    }

	void ReviveFriendIfDead(int /*pFriend*/)
	{ 
		if(mFriend && mFriend->isDead())
			mFriend->Respawn();
	}

	void GoLoot(int /*pFriend*/)
	{
		if(mFriend && mFriend->isDead()) 
			mFriend->SetVisibility(VISIBILITY_ON);
		else 
			m_creature->SetVisibility(VISIBILITY_OFF);
	}

	Creature* GetFriend(int pFriend)
	{
		Creature* pTemp = 0;
		std::list<Creature*> ADList;
		GetCreatureListWithEntryInGrid(ADList,m_creature,pFriend,500.0f);
		for(std::list<Creature*>::iterator i = ADList.begin(); i != ADList.end(); ++i)
			pTemp = *i;
		return pTemp;
	}

	 void UpdateAI(const uint32 uiDiff)
    {
        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (spalten_timer < uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(),SPELL_SPALTEN);
			spalten_timer = urand(5000,15000);
		}
		else
			spalten_timer -= uiDiff;

		if (BossScript)
		{
			if (streich_timer < uiDiff)
			{
				DoCastSpellIfCan(m_creature->getVictim(),SPELL_WEITER_STREICH);
				streich_timer = urand(12000,20000);
			}
			else
				streich_timer -= uiDiff;

			if (!enrage)
			{
				if (m_creature->GetHealthPercent() < 30)
				{
					m_creature->CastSpell(m_creature, SPELL_RASEREI, true);
					enrage = true;
				}
			}
		}
		else
		{
			Unit* pTarget = m_creature->getVictim();
			if (pTarget->GetHealthPercent() < 20)
			{
				if (hinrichten_timer < uiDiff)
				{
					if(pTarget)
					{
						DoCastSpellIfCan(pTarget,SPELL_HINRICHTEN);
						hinrichten_timer = 8000;
					}
				}
				else
					hinrichten_timer -= uiDiff;
			}
		}

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ukorz_sandscalp(Creature* pCreature)
{
    return new boss_ukorz_sandscalpAI(pCreature);
}

void AddSC_boss_ukorz_sandscalp()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_ukorz_sandscalp_AND_ruuzlu";
    pNewscript->GetAI = &GetAI_boss_ukorz_sandscalp;
    pNewscript->RegisterSelf();
}
