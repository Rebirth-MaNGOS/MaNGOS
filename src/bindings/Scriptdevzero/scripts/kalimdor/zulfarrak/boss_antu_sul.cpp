//Created by Baum

/*
### PROJEKTMAPPE ###
CPP Datei hinzufügen
### SCRIPTLOADER ###
extern void AddSC_boss_antu_sul();
AddSC_boss_antu_sul();
### SQL ###
update creature_template set ScriptName = "boss_antu_sul"
where entry = 8127;
*/

//SAY EMOTES are missing

#include "precompiled.h"
#include "zulfarrak.h"

enum DATA
{
    SPELL_KETTENBLITZSCHLAG     = 16006,
    SPELL_ERDSCHOCK				= 15501,
	SPELL_HEILUNGSZAUBERSCHUTZ	= 11899,
	SPELL_TOTEM_DES_ERDENGRIFFS	= 8376,
	SPELL_WELLE_DER_HEILUNG		= 12491,
	SPELL_HEILUNG_VON_ANTU_SUL	= 11895,

	NPC_NON_ELITE_BASILISK		= 8138,
	NPC_ELITE_BASILISK			= 8156
};

struct MANGOS_DLL_DECL boss_antu_sulAI : public ScriptedAI
{
    boss_antu_sulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_zulfarrak*)pCreature->GetInstanceData();
        Reset();
    }

	 instance_zulfarrak* m_pInstance;

	 uint32 heal_timer;
	 uint32 totem_timer;
	 uint32 shock_timer;
	 uint32 blitz_timer;
	 float boss_hp;

    void Reset()
    {
		killAdds();

		heal_timer = urand(20000,35000);
		totem_timer = 3000;
		shock_timer = 8000;
		blitz_timer = 15000;
		boss_hp = 100;
    }

	void killAdds()
	{
		for (int i = 0; i < 2; i++)
		{
			int Add_delete;
			if (i < 1)
				Add_delete = NPC_ELITE_BASILISK;
			else
				Add_delete = NPC_NON_ELITE_BASILISK;

			std::list<Creature*> ADDList;          
			GetCreatureListWithEntryInGrid(ADDList,m_creature,Add_delete,60.0f);
			for(std::list<Creature*>::iterator i = ADDList.begin(); i != ADDList.end(); ++i)
			{
				Creature* pTemp = *i;
				if (pTemp)
				{
					pTemp->SetDeathState(JUST_DIED);
					pTemp->SetHealth(0);
				}
			}
		}
	}

    void Aggro(Unit* /*pVictim*/)
    {
		for(int i = 0; i < 6; i++)
		{
			int random = urand(-5,5);
			Creature* pTemp = m_creature->SummonCreature(NPC_NON_ELITE_BASILISK,m_creature->GetPositionX()+random,m_creature->GetPositionY()+random,m_creature->GetPositionZ(),0,TEMPSUMMON_CORPSE_DESPAWN,5000);
			pTemp->SetInCombatWithZone();
		}
    }

	void JustDied(Unit* /*pKiller*/)
    {
		killAdds();
    }

	 void UpdateAI(const uint32 uiDiff)
    {
        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_creature->GetHealthPercent() < boss_hp - 20)
		{
			boss_hp = boss_hp - 20;

			Creature* pTemp = m_creature->SummonCreature(NPC_ELITE_BASILISK,m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),0,TEMPSUMMON_CORPSE_DESPAWN,5000);
			pTemp->SetInCombatWithZone();
		}

		if (shock_timer < uiDiff)
		{
			Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
			if(pTarget)
			{
				if (DoCastSpellIfCan(pTarget,SPELL_ERDSCHOCK) == CAST_OK)
					shock_timer = urand(18000,30000);
			}
		}
		else
			shock_timer -= uiDiff;

		if (blitz_timer < uiDiff)
		{
			if (DoCastSpellIfCan(m_creature->getVictim(),SPELL_KETTENBLITZSCHLAG) == CAST_OK)
				blitz_timer = urand(12000, 25000);
		}
		else
			blitz_timer -= uiDiff;

		if (heal_timer < uiDiff)
		{
			Unit* pFriendlyUnit = DoSelectLowestHpFriendly(40.0f,1); 
			if (pFriendlyUnit && pFriendlyUnit != m_creature && m_creature->IsWithinDistInMap(pFriendlyUnit,40.0f))
			{
				DoCastSpellIfCan(pFriendlyUnit,SPELL_WELLE_DER_HEILUNG);
				heal_timer = urand(20000,35000);
				if(m_creature->GetHealthPercent() < 30)
					DoCastSpellIfCan(m_creature,SPELL_HEILUNG_VON_ANTU_SUL);
			}
			else
			{
				DoCastSpellIfCan(m_creature,SPELL_HEILUNG_VON_ANTU_SUL);
				heal_timer = urand(20000,35000);
			}
		}
		else
			heal_timer -= uiDiff;

		if (totem_timer < uiDiff)
		{
			int random = urand(0,1);
			m_creature->SetPower(POWER_MANA, 50000);
			if (random == 0)
			{
				if (DoCastSpellIfCan(m_creature,SPELL_HEILUNGSZAUBERSCHUTZ) == CAST_OK)
					totem_timer = 15000;
			}
			else
			{
				if (DoCastSpellIfCan(m_creature,SPELL_TOTEM_DES_ERDENGRIFFS) == CAST_OK)
					totem_timer = 15000;
			}
		}
		else
			totem_timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_antu_sul(Creature* pCreature)
{
    return new boss_antu_sulAI(pCreature);
}

void AddSC_boss_antu_sul()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_antu_sul";
    pNewscript->GetAI = &GetAI_boss_antu_sul;
    pNewscript->RegisterSelf();
}
