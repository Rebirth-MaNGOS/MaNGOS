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
SDName: Molten_Core
SD%Complete: 100
SDComment:
SDCategory: Molten Core
EndScriptData */

/* ContentData
mob_ancient_core_hound
go_molten_core_rune
mob_flameguard
mob_lava_elemental
mob_lava_surger
EndContentData */

#include "precompiled.h"
#include "molten_core.h"

/*######
## mob_ancient_core_hound
######*/

enum eAncientCoreHound
{
    //SPELL_CONE_OF_FIRE        = 19630,
    //SPELL_BITE                = 19771,
    SPELL_LAVA_BREATH           = 19272,
    SPELL_VICIOUS_BITE          = 19319,

    // Random Debuff (each hound has only one of these)
    SPELL_GROUND_STOMP          = 19364,
    SPELL_ANCIENT_DREAD         = 19365,
    SPELL_CAUTERIZING_FLAMES    = 19366,
    SPELL_WITHERING_HEAT        = 19367,
    SPELL_ANCIENT_DESPAIR       = 19369,
    SPELL_ANCIENT_HYSTERIA      = 19372
};

struct MANGOS_DLL_DECL mob_ancient_core_houndAI : public ScriptedAI
{
    mob_ancient_core_houndAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_pInstance = (instance_molten_core*)m_creature->GetInstanceData();
        Reset();
    }

    uint32 m_uiLavaBreathTimer;
    uint32 m_uiViciousBiteTimer;
    uint32 m_uiRandomDebuffTimer;
    uint32 m_uiRandomDebuffId;

	instance_molten_core* m_pInstance;

    void Reset()
    {        
        m_uiLavaBreathTimer = 10000;
        m_uiViciousBiteTimer = 4000;
        m_uiRandomDebuffTimer = 15000;

        m_uiRandomDebuffId = 0;
        switch(urand(0,5))
        {
            case 0:
                m_uiRandomDebuffId = SPELL_GROUND_STOMP;
                break;
            case 1:
                m_uiRandomDebuffId = SPELL_ANCIENT_DREAD;
                break;
            case 2:
                m_uiRandomDebuffId = SPELL_CAUTERIZING_FLAMES;
                break;
            case 3:
                m_uiRandomDebuffId = SPELL_WITHERING_HEAT;
                break;
            case 4:
                m_uiRandomDebuffId = SPELL_ANCIENT_HYSTERIA;
                break;
            case 5:
               m_uiRandomDebuffId = SPELL_ANCIENT_DESPAIR;
               break;
        }
    }

	//void JustDied(Unit* pKiller)
	//{
	//	//Set high respawn time if Magmadar is dead
	//	if (m_pInstance && m_pInstance->GetData(TYPE_MAGMADAR) == DONE)
	//	{
	//		m_creature->SetRespawnDelay(43200);
	//		m_creature->SetDeathState(JUST_DIED);
	//	}
	//}

	void JustRespawned()
	{
		//Make invisible if Magmadar is dead
		if (m_pInstance && m_pInstance->GetData(TYPE_MAGMADAR) == DONE)
		{
			m_creature->setFaction(35);
			m_creature->SetVisibility(VISIBILITY_OFF);
			//m_creature->SetCorpseDelay(1);
			//m_creature->SetDeathState(JUST_DIED);
			//m_creature->SetHealth(0);
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Lava Breath
        if (m_uiLavaBreathTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_LAVA_BREATH);
            m_uiLavaBreathTimer = urand(8000,10000);
        }
        else
            m_uiLavaBreathTimer -= uiDiff;

        // Vicious Bite
        if (m_uiViciousBiteTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_VICIOUS_BITE);
            m_uiViciousBiteTimer = urand(4000,7000);
        }
        else
            m_uiViciousBiteTimer -= uiDiff;

        // Random Debuff
        if (m_uiRandomDebuffTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), m_uiRandomDebuffId, CAST_TRIGGERED);
            m_uiRandomDebuffTimer = urand(20000,25000);
        }
        else
            m_uiRandomDebuffTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_ancient_core_hound(Creature* pCreature)
{
    return new mob_ancient_core_houndAI(pCreature);
}

/*######
## CoreHoundTriggerAI
######*/
//Corehound triggers are still spawned, but deactivated

struct MANGOS_DLL_DECL CoreHoundTriggerAI : public ScriptedAI
{
    CoreHoundTriggerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		SetCombatMovement(false);
		m_creature->SetVisibility(VISIBILITY_OFF);
		m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();

        Reset();
    }

	instance_molten_core* m_pInstance;

	Creature* pAdds[5];

	bool GetCoreHounds;

	uint32 CoreHound_Timer[5];
	uint32 Bite_Timer[5];
	uint8 AddCounter;
	bool inCombat;

	void Reset()
	{
		for (uint8 i = 0; i < 5; ++i)
			CoreHound_Timer[i] = 12000;

		for (uint8 i = 0; i < 5; ++i)
			Bite_Timer[i] = 10000;

		AddCounter = 0;

		GetCoreHounds = false;
	}


	void UpdateAI(const uint32 diff)
    {
		//m_creature->CallForHelp(15.0f);


		if (GetCoreHounds == false)
		{
			//Create list of corehounds, 5 per pack
			std::list<Creature*> pList;          
			GetCreatureListWithEntryInGrid(pList,m_creature, NPC_CORE_HOUND, 20.0f);
			for(std::list<Creature*>::iterator i = pList.begin(); i != pList.end(); ++i)
			{
				if (AddCounter < 5)
				{
					pAdds[AddCounter] = *i;

					++AddCounter;
				}
			}

			GetCoreHounds = true;
		}

		//for (uint8 i = 0; i < 5; ++i)
		//{
		//	if (pAdds[i] && pAdds[i]->isInCombat())
		//		pAdds[i]->CallForHelp(15.0f);				
		//}

		//No need to make them use serrated bite here
		//for (uint8 i = 0; i < 5; ++i)
		//{
		//	if (Bite_Timer[i] < diff)
		//	{
		//		if (pAdds[i] && pAdds[i]->isAlive())
		//		{
		//			Unit* pTarget = pAdds[i]->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
		//			Use serrated bite ability
		//			if (pTarget)
		//				pAdds[i]->CastSpell(pTarget,SPELL_SERRATED_BITE,true);

		//			Bite_Timer[i] = 10000 + rand()%5000;
		//		}
		//	}
		//	else
		//		Bite_Timer[i] -= diff;
		//}

		bool inCombat = false;

		for (uint8 i = 0; i < 5; ++i)
		{
			if (pAdds[i] && pAdds[i]->isInCombat())
			{
				inCombat = true;
				break;
			}
		}

		//Set timer if a corehound dies
		for (uint8 i = 0; i < 5; ++i)
		{
			if (pAdds[i] && pAdds[i]->isDead() && CoreHound_Timer[i] > 10000)
				CoreHound_Timer[i] = 10000;
		}

		for (uint8 i = 0; i < 5; ++i)
		{
			if (CoreHound_Timer[i] <= 10000)
			{
				if (CoreHound_Timer[i] < diff)
				{
					if (pAdds[0]->isAlive() || pAdds[1]->isAlive() || pAdds[2]->isAlive() ||pAdds[3]->isAlive() ||pAdds[4]->isAlive())
					{
						pAdds[i]->SetDeathState(JUST_ALIVED);
						//pAdds[i]->SetHealthPercent(100);

						if (inCombat)
						{
							pAdds[i]->SetInCombatWithZone();
						}

						//Unit* pTarget = pAdds[i]->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
						//pAdds[i]->AI()->AttackStart(pTarget);
					}
					else
					{
						m_creature->SetDeathState(JUST_DIED);
						m_creature->SetHealthPercent(0);
					}

					CoreHound_Timer[i] = 12000;
				}
				else
					CoreHound_Timer[i] -= diff;
			}
		}
	}			
};

CreatureAI* GetAI_CoreHoundTrigger(Creature* pCreature)
{
    return new CoreHoundTriggerAI(pCreature);
}

/*######
## go_molten_core_rune
######*/

bool GOUse_go_molten_core_rune(Player* /*pPlayer*/, GameObject* pGo)
{
    if (instance_molten_core* m_pInstance = (instance_molten_core*)pGo->GetInstanceData())
    {
        switch(pGo->GetEntry())
        {
            case GO_RUNE_OF_KRESS:
                if (m_pInstance->GetData(TYPE_LUCIFRON) == DONE && m_pInstance->GetData(TYPE_MAGMADAR) == DONE)
                {
                    m_pInstance->SetData(TYPE_LUCIFRON, SPECIAL);
                    m_pInstance->SetData(TYPE_MAGMADAR, SPECIAL);
                }
                break;
            case GO_RUNE_OF_MOHN:
                if (m_pInstance->GetData(TYPE_GEHENNAS) == DONE)
                    m_pInstance->SetData(TYPE_GEHENNAS, SPECIAL);
                break;
            case GO_RUNE_OF_BLAZ:
                if (m_pInstance->GetData(TYPE_GARR) == DONE)
                    m_pInstance->SetData(TYPE_GARR, SPECIAL);
                break;
            case GO_RUNE_OF_MAZJ:
                if (m_pInstance->GetData(TYPE_SHAZZRAH) == DONE)
                    m_pInstance->SetData(TYPE_SHAZZRAH, SPECIAL);
                break;
            case GO_RUNE_OF_ZETH:
                if (m_pInstance->GetData(TYPE_GEDDON) == DONE)
                    m_pInstance->SetData(TYPE_GEDDON, SPECIAL);
                break;
            case GO_RUNE_OF_KORO:
                if (m_pInstance->GetData(TYPE_SULFURON) == DONE)
                    m_pInstance->SetData(TYPE_SULFURON, SPECIAL);
                break;
            case GO_RUNE_OF_THERI:
                if (m_pInstance->GetData(TYPE_GOLEMAGG) == DONE)
                    m_pInstance->SetData(TYPE_GOLEMAGG, SPECIAL);
                break;
        }
    }

    return true;
}

/*######
## mob_flameguard
######*/

enum eFlameguard
{
    SPELL_MELT_ARMOR    = 19631,
    SPELL_CONE_OF_FIRE  = 19630,
};

struct MANGOS_DLL_DECL mob_flameguardAI : public ScriptedAI
{
    mob_flameguardAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiMeltArmorTimer;
    uint32 m_uiConeOfFireTimer;

    void Reset()
    {
        m_uiMeltArmorTimer = urand(1000,5000);
        m_uiConeOfFireTimer = urand(4000,9000);

        m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_FIRE, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Melt Armor
        if (m_uiMeltArmorTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MELT_ARMOR);
            m_uiMeltArmorTimer = urand(7000,11000);
        }
        else
            m_uiMeltArmorTimer -= uiDiff;

        // Cone of Fire
        if (m_uiConeOfFireTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CONE_OF_FIRE);
            m_uiConeOfFireTimer = urand(8000,10000);
        }
        else
            m_uiConeOfFireTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_flameguard(Creature* pCreature)
{
    return new mob_flameguardAI(pCreature);
}

/*######
## mob_lava_elemental
######*/

enum eLavaElemental
{
    SPELL_PYROCLAST_BARRAGE = 19641,
};

struct MANGOS_DLL_DECL mob_lava_elementalAI : public ScriptedAI
{
    mob_lava_elementalAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiPyroclastBarrageTimer;

    void Reset()
    {
        m_uiPyroclastBarrageTimer = urand(3000,9000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Pyroclast Barrage
        if (m_uiPyroclastBarrageTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_PYROCLAST_BARRAGE);
            m_uiPyroclastBarrageTimer = urand(7000,10000);
        }
        else
            m_uiPyroclastBarrageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_lava_elemental(Creature* pCreature)
{
    return new mob_lava_elementalAI(pCreature);
}

/*######
## mob_lava_surger
######*/

#define SPELL_SURGE 19196   // before: 25787

struct MANGOS_DLL_DECL mob_lava_surgerAI : public ScriptedAI
{
    mob_lava_surgerAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        instance_molten_core* pInstance = dynamic_cast<instance_molten_core*>(m_creature->GetInstanceData());
        if (pInstance && pInstance->GetData(TYPE_GARR) >= DONE)
            m_creature->SetRespawnDelay(DAY * 7);
        
        Reset();
    }

    uint32 m_uiSurgeTimer;
    uint32 m_uiSuspendTimer; // Timer used to allow the charge animation to occur correctly.

    void Reset()
    {
        m_uiSurgeTimer = 5000;
        m_uiSuspendTimer = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiSuspendTimer > uiDiff)
        {
            m_uiSuspendTimer -= uiDiff;
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Surge
        if (m_uiSurgeTimer <= uiDiff)
        {
            Player* pPlayer = GetPlayerFarthestAwayFromThreatlistWithinRange(40.f);

            if(pPlayer && (!m_creature->IsWithinDistInMap(pPlayer, 5.f) && m_creature->IsWithinLOSInMap(pPlayer)))
            {
                DoCastSpellIfCan(pPlayer, SPELL_SURGE);
                m_uiSuspendTimer = 2000;
            }
            m_uiSurgeTimer = 8000;
        }
        else
            m_uiSurgeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_lava_surger(Creature* pCreature)
{
    return new mob_lava_surgerAI(pCreature);
}

/*######
## mob_lava_spawn
######*/

enum eLavaSpawn
{
    SPELL_FIREBALL = 19391,
    SPELL_SPLIT    = 19569,
};

struct MANGOS_DLL_DECL mob_lava_spawnAI : public ScriptedAI
{
    mob_lava_spawnAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_FIRE, true);
		Reset();
	}

    uint32 m_uiFireballTimer;
    uint32 m_uiSplitTimer;

    void Reset()
    {
        m_uiFireballTimer = 1000;
        m_uiSplitTimer = 8000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Fireball
        if (m_uiFireballTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_FIREBALL);
            m_uiFireballTimer = 3000;
        }
        else
            m_uiFireballTimer -= uiDiff;

        // Split
        if (m_uiSplitTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SPLIT);
            m_uiSplitTimer = 8000;
        }
        else
            m_uiSplitTimer -= uiDiff;

        // No meele
    }
};

CreatureAI* GetAI_mob_lava_spawn(Creature* pCreature)
{
    return new mob_lava_spawnAI(pCreature);
}

/*######
## mob_flame_imp
######*/

struct MANGOS_DLL_DECL mob_flame_impAI : public ScriptedAI
{
    mob_flame_impAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
		Reset();
	}

	instance_molten_core* m_pInstance;

    uint32 m_uiFireNovaTimer;

    void Reset()
    {
        m_uiFireNovaTimer = urand(1000,5000);
    }

	void JustRespawned()
	{
		//Make invisible if Lucifron is dead
		if (m_pInstance && m_pInstance->GetData(TYPE_LUCIFRON) == DONE)
		{
			m_creature->setFaction(35);
			m_creature->SetVisibility(VISIBILITY_OFF);
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Fire Nova
        if (m_uiFireNovaTimer <= uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), 20602, false);
            m_uiFireNovaTimer = urand(4000,7000);
        }
        else
            m_uiFireNovaTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_flame_imp(Creature* pCreature)
{
    return new mob_flame_impAI(pCreature);
}

/*######
## AddSC
######*/

void AddSC_molten_core()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "mob_ancient_core_hound";
    pNewScript->GetAI = &GetAI_mob_ancient_core_hound;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "CoreHoundTrigger";
    pNewScript->GetAI = &GetAI_CoreHoundTrigger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_molten_core_rune";
    pNewScript->pGOUse = &GOUse_go_molten_core_rune;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_flameguard";
    pNewScript->GetAI = &GetAI_mob_flameguard;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_lava_elemental";
    pNewScript->GetAI = &GetAI_mob_lava_elemental;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_lava_surger";
    pNewScript->GetAI = &GetAI_mob_lava_surger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_lava_spawn";
    pNewScript->GetAI = &GetAI_mob_lava_spawn;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_flame_imp";
    pNewScript->GetAI = &GetAI_mob_flame_imp;
    pNewScript->RegisterSelf();
}
