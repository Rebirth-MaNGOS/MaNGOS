//Created by Baum

/*
### PROJEKTMAPPE ###
CPP Datei hinzufügen
### SCRIPTLOADER ###
extern void AddSC_boss_zum_rah();
AddSC_boss_zum_rah();
### SQL ###
update creature_template set ScriptName = "boss_zum_rah_AND_hydromance_velratha"
where entry = 7271;
update creature_template set ScriptName = "boss_zum_rah_AND_hydromance_velratha"
where entry = 7795;
*/

//SAY EMOTES are missing

#include "precompiled.h"
#include "zulfarrak.h"

enum DATA
{
	SPELL_SCHATTENBLITZ				= 12739,
	SPELL_SCHATTENBLITZSALVE		= 15245,
	SPELL_WELLE_DER_HEILUNG			= 12491,
	SPELL_ZAUBERSCHUTZ_VON_ZUM_RAH	= 11086,

	NPC_ZOMBIE						= 7286,
	NPC_SKELETT						= 7786,
	NPC_WITCH_DOCTOR_ZUM_RAH		= 7271,

	OBJECT_GRAVE					= 128403
};

struct MANGOS_DLL_DECL boss_zum_rahAI : public ScriptedAI
{
    boss_zum_rahAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulfarrak*)pCreature->GetInstanceData();
		m_creature->SetPower(POWER_MANA, 50000);
        Reset();
    }

	 instance_zulfarrak* m_pInstance;

	 uint32 sblitz_timer;
	 uint32 sbsalve_timer;
	 uint32 heilung_timer;
	 uint32 zauberschutz_timer;
	 uint32 zombie_timer;


    void Reset()
    {
		resetFight();

		sblitz_timer = 1000;
		sbsalve_timer = 15000;
		heilung_timer = urand(15000,30000);
		zauberschutz_timer = urand(10000,25000);
		zombie_timer = urand(15000,40000);
    }

	void JustDied(Unit* /*pKiller*/)
    {
		resetFight();
    }

	void resetFight()
	{
		std::list<Creature*> ADDList;          
		GetCreatureListWithEntryInGrid(ADDList,m_creature,NPC_ZOMBIE,100.0f);
		for(std::list<Creature*>::iterator i = ADDList.begin(); i != ADDList.end(); ++i)
		{
			Creature* pTemp = *i;
			if (pTemp)
			{
				pTemp->SetDeathState(JUST_DIED);
				pTemp->SetHealth(0);
			}
		}

		std::list<GameObject*> pList;
		GetGameObjectListWithEntryInGrid(pList,m_creature,OBJECT_GRAVE,500.0f);
		for(std::list<GameObject*>::iterator i = pList.begin(); i != pList.end(); ++i)
		{
			GameObject* pTemp = *i;
			if(pTemp && pTemp->GetGoState() == GO_STATE_ACTIVE)
				pTemp->SetGoState(GO_STATE_READY);
		}
	}

	 void UpdateAI(const uint32 uiDiff)
    {
        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (sblitz_timer < uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(),SPELL_SCHATTENBLITZ);
			sblitz_timer = 3000;
		}
		else
			sblitz_timer -= uiDiff;

		if (sbsalve_timer < uiDiff)
		{
			m_creature->CastSpell(m_creature,SPELL_SCHATTENBLITZSALVE,true);
			m_creature->SetPower(POWER_MANA, 50000);
			sbsalve_timer = urand(10000,30000);
		}
		else
			sbsalve_timer -= uiDiff;

		if (heilung_timer < uiDiff)
		{
			m_creature->CastSpell(m_creature,SPELL_WELLE_DER_HEILUNG,false);
			heilung_timer = urand(15000,30000);
		}
		else
			heilung_timer -= uiDiff;

		if (zauberschutz_timer < uiDiff)
		{
			m_creature->CastSpell(m_creature,SPELL_ZAUBERSCHUTZ_VON_ZUM_RAH,true);
			zauberschutz_timer = urand(10000,25000);
		}
		else
			zauberschutz_timer -= uiDiff;

		if (m_creature->GetEntry() == NPC_WITCH_DOCTOR_ZUM_RAH)
		{
			if (zombie_timer < uiDiff)
			{
				GameObject* pGrave;
				std::list<GameObject*> pList;
				GetGameObjectListWithEntryInGrid(pList,m_creature,OBJECT_GRAVE,30.0f);
				for(std::list<GameObject*>::iterator i = pList.begin(); i != pList.end(); ++i)
				{
					pGrave = *i;
					if(pGrave && pGrave->GetGoState() == GO_STATE_READY && pGrave->isSpawned())
						break;
				}

				if (pGrave && pGrave->GetGoState() == GO_STATE_READY && pGrave->isSpawned())
				{
					pGrave->SetGoState(GO_STATE_ACTIVE);
					pGrave->SetLootState(GO_JUST_DEACTIVATED);
					if(Creature* pTemp = pGrave->SummonCreature(NPC_ZOMBIE,pGrave->GetPositionX(),pGrave->GetPositionY(),pGrave->GetPositionZ(),0,TEMPSUMMON_CORPSE_DESPAWN,5000))
						pTemp->SetInCombatWithZone();
					zombie_timer = urand(15000,30000);
				}
			}
			else
				zombie_timer -= uiDiff;
		}

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_zum_rah(Creature* pCreature)
{
    return new boss_zum_rahAI(pCreature);
}

void AddSC_boss_zum_rah()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_zum_rah_AND_hydromance_velratha";
    pNewscript->GetAI = &GetAI_boss_zum_rah;
    pNewscript->RegisterSelf();
}