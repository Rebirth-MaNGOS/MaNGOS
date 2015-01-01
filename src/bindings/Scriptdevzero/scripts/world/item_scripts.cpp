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
SDName: Item_Scripts
SD%Complete: 100
SDComment: Items for a range of different items. See content below (in script)
SDCategory: Items
EndScriptData */

/* ContentData
item_essence_of_hakkar
EndContentData */

#include "precompiled.h"
#include "Spell.h"

/*######
## item_essence_of_hakkar       temp script to create filled egg
######*/

enum eEssenceOfHakkar
{
    ITEM_EGG_OF_HAKKAR            = 10465,
    ITEM_FILLED_EGG_OF_HAKKAR     = 10662,
    ITEM_ESSENCE_OF_HAKKAR        = 10663,
    QUEST_THE_GOD_HAKKAR          = 3528
};

bool ItemUse_item_essence_of_hakkar(Player* pPlayer, Item* /*pItem*/, SpellCastTargets const& /*targets*/)
{
    if (pPlayer->GetQuestStatus(QUEST_THE_GOD_HAKKAR) == QUEST_STATUS_INCOMPLETE)
        if (pPlayer->HasItemCount(ITEM_EGG_OF_HAKKAR, 1) && pPlayer->HasItemCount(ITEM_ESSENCE_OF_HAKKAR, 1))
        {
            pPlayer->DestroyItemCount(ITEM_ESSENCE_OF_HAKKAR, 1, true);
            pPlayer->DestroyItemCount(ITEM_EGG_OF_HAKKAR, 1, true);
            if (Item* pFilledEgg = pPlayer->StoreNewItemInInventorySlot(ITEM_FILLED_EGG_OF_HAKKAR, 1))
                pPlayer->SendNewItem(pFilledEgg, 1, true, false);
            return true;
        }
    return false;
}

/*######
## Muisek Vessels
######*/

bool ItemUse_Muisek_Vessel(Player* /*pPlayer*/, Item* /*I*/, SpellCastTargets const& targets)
{
	Creature* pVictim = 0;
	if (Unit* U = targets.getUnitTarget())
		pVictim = (Creature*)U;

	if (pVictim)
	{
		pVictim->SaveRespawnTime();
		pVictim->SetDeathState(JUST_DIED);
		pVictim->RemoveCorpse();
	}

	return false;
}

/*###########################
## Defender of the Timbermaw
###########################*/

#define SPELL_HEALING_TOUCH 5189
#define SPELL_LIGHTNING 20295

struct MANGOS_DLL_DECL defender_of_the_timbermawAI : public ScriptedAI
{
	defender_of_the_timbermawAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool isFollowing;

	void Reset()
	{
        m_creature->SetName("Timbermaw Ancestor");
        m_creature->SetLevel(60);
        m_creature->SetHealth(300);
        m_creature->SetMaxHealth(300);
        m_creature->SetMaxPower(POWER_MANA, 5000);
        m_creature->SetPower(POWER_MANA, 5000);

		// Make the it follow the player.
		m_creature->GetMotionMaster()->Clear();
		m_creature->GetMotionMaster()->MoveFollow(m_creature->GetOwner(), 0.5f, 3.14f / 6.0f);
        isFollowing = true;
	}

    void EnterCombat(Unit* /*victim*/)
    {
        isFollowing = false;
    }

	void UpdateAI(const uint32 /*uiDiff*/)
	{
        Player* pOwner = dynamic_cast<Player*>(m_creature->GetCharmerOrOwner());

        if (!pOwner)
            return;

        Group* pGroup = pOwner->GetGroup();
        
        if (pGroup)
        {
            for (auto itr = pGroup->GetMemberSlots().begin(); itr != pGroup->GetMemberSlots().end(); ++itr) // Heal partymembers if their health is below 50 %.
            {
                Player* currentMember = pOwner->GetMap()->GetPlayer((*itr).guid); 

                if (currentMember && currentMember->GetHealthPercent() < 50)
                {
                   DoCastSpellIfCan(currentMember,  SPELL_HEALING_TOUCH);
                   return;
                }
            }
        }
        else if (pOwner->GetHealthPercent() < 50)
            DoCastSpellIfCan(pOwner, SPELL_HEALING_TOUCH);

        Unit* ownerTarget = pOwner->getVictim();
        
        if (!ownerTarget || !pOwner->isInCombat())
        {
            if (!isFollowing)
            {
                m_creature->GetMotionMaster()->Clear();
		        m_creature->GetMotionMaster()->MoveFollow(m_creature->GetOwner(), 0.5f, 3.14f / 6.0f);
                isFollowing = true;
            }
            return;
        }

        if (!m_creature->isInCombat())
            m_creature->SetInCombatWith(ownerTarget);

        DoCastSpellIfCan(ownerTarget, SPELL_LIGHTNING);


	}
};

CreatureAI* GetAI_defender_of_the_timbermaw(Creature* pCreature)
{
	return new defender_of_the_timbermawAI(pCreature);
}

/*######
## AddSC
######*/

void AddSC_item_scripts()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name="item_essence_of_hakkar";
    pNewscript->pItemUse = &ItemUse_item_essence_of_hakkar;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
	pNewscript->Name = "item_muiseks_vessel";
    pNewscript->pItemUse = &ItemUse_Muisek_Vessel;
    pNewscript->RegisterSelf();

    pNewscript = new Script();
    pNewscript->Name = "defender_of_the_timbermaw";
    pNewscript->GetAI = &GetAI_defender_of_the_timbermaw;
    pNewscript->RegisterSelf();
}
