#include "precompiled.h"
#include "sc_creature.h"
#include "ahnqiraj_turnin_defines.h"
#include "EventResourceMgr.h"

// This macro creates a wrapper that calls the QuestRewarded_ahnqiraj_turnin function
// with the specified quests and resources. This to avoid having to rewrite the handling
// function each time.
// Both the initial quest and the repeat quest need to exists. They are given as enums
// with the repeat quest having the suffix "_REP" added after the name.

// Ex: Q_A_ARTHAS_TEARS, Q_A_ARTHAS_TEARS_REP

// Only the initial quest needs to be given to the macro since it appends the "_REP" suffix
// to that name.
#define QuestReward_wrapper(Name, Resource, ResourceCount, QuestID) \
bool QuestRewarded_##Name(Player* pPlayer, Creature* pCreature, const Quest* pQuest) \
{ \
    return QuestRewarded_ahnqiraj_turnin(pPlayer, pCreature, pQuest, Resource, \
                                         ResourceCount, QuestID, QuestID##_REP); \
}

// Gets a pointer to the wrapper created by the define above.
#define GetQuestReward_wrapper(Name) &QuestRewarded_##Name

bool QuestRewarded_ahnqiraj_turnin(Player* /*pPlayer*/, Creature* pCreature, const Quest* pQuest, 
        AhnQirajResources resource, int resourceCount, AhnQirajQuest quest, AhnQirajQuest repeatable)
{
    if (pQuest)
    {
        if (pQuest->GetQuestId() == quest || pQuest->GetQuestId() == repeatable)
        {
            // Add to the resource counter and remove the quest giver flag if the 
            // maximum amount of resources has been reached.
            if (!AddResourceCount(EVENT_ID, resource, resourceCount))
                pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        }
    }
    return false;
}

// Wrappers for the Alliance quests.
QuestReward_wrapper(germaine, ARTHAS_TEARS, 20, Q_A_ARTHAS_TEARS)

QuestReward_wrapper(stonebrow, COPPER_BAR, 20, Q_A_COPPER_BARS)

QuestReward_wrapper(bonnie, LIGHT_LEATHER, 10, Q_A_LIGHT_LEATHER)

QuestReward_wrapper(silversky, LINEN_BANDAGE, 20, Q_A_LINEN_BANDAGES)

QuestReward_wrapper(porter, MEDIUM_LEATHER, 10, Q_A_MEDIUM_LEATHER)

QuestReward_wrapper(nightsong, PURPLE_LOTUS, 20, Q_A_PURPLE_LOTUS)

QuestReward_wrapper(slicky, RAINBOW_FIN_ALBACORE, 20, Q_A_ALBACORE)

QuestReward_wrapper(sarah, ROAST_RAPTOR, 20, Q_A_ROAST_RAPTOR)

QuestReward_wrapper(moonshade, RUNECLOTH_BANDAGE, 20, Q_A_RUNECLOTH_BANDAGES)

QuestReward_wrapper(stonefield, SILK_BANDAGE, 20, Q_A_SILK_BANDAGES)

QuestReward_wrapper(swiftriver, SPOTTED_YELLOWTAIL, 20, Q_A_SPOTTED_YELLOWTAIL)

QuestReward_wrapper(draxlegauge, STRANGLEKELP, 20, Q_A_STRANGLEKELP)

QuestReward_wrapper(marta, THICK_LEATHER, 10, Q_A_THICK_LEATHER)

QuestReward_wrapper(dame, THORIUM_BAR, 20, Q_A_THORIUM_BARS)

QuestReward_wrapper(carnes, IRON_BAR, 20, Q_A_IRON_BAR)

void AddSC_gate_turnin_scripts()
{
    Script* pScript = new Script;
    
    pScript->Name = "npc_sgt_maj_germaine";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(germaine);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_sgt_stonebrow";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(stonebrow);
    pScript->RegisterSelf();
    
    pScript = new Script;
    pScript->Name = "npc_bonnie_stoneflayer";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(bonnie);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_sent_silversky";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(silversky);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_prvt_porter";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(porter);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_master_nightsong";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(nightsong);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_slicky";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(slicky);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_sarah_sadwhistle";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(sarah);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_kpr_moonshade";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(moonshade);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_nrs_stonefield";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(stonefield);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_hntr_swiftriver";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(swiftriver);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_pvt_draxlegauge";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(draxlegauge);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_marta_finespindle";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(marta);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_dame_twinbraid";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(dame);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_cpr_carnes";
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(carnes);
    pScript->RegisterSelf();
}
