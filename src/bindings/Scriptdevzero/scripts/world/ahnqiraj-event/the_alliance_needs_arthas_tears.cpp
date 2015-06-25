#include "precompiled.h"
#include "sc_creature.h"
#include "event_defines.h"
#include "EventResourceMgr.h"

bool QuestRewarded_npc_sgt_maj_germaine(Player* /*pPlayer*/, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest)
    {
        if (pQuest->GetQuestId() == QUEST_ALLIANCE_ARTHAS_TEARS)
        {
            // Add to the resource counter and remove the quest giver flag if the 
            // maximum amount of resources has been reached.
            if (!AddResourceCount(EVENT_ID, ARTHAS_TEARS, 20))
                pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        }
    }
    return false;
}

void AddSC_gate_turnin_scripts()
{
    Script* pScript = new Script;
    
    pScript->Name = "npc_sgt_maj_germaine";
    pScript->pQuestRewardedNPC = &QuestRewarded_npc_sgt_maj_germaine;
    pScript->RegisterSelf();

}
