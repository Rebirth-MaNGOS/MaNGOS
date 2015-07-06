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

// Wrappers for the Horde quests.
QuestReward_wrapper(sharpclaw, BAKED_SALMON, 20, Q_H_BAKED_SALMON)

QuestReward_wrapper(cromwell, COPPER_BAR, 20, Q_H_COPPER_BARS)

QuestReward_wrapper(pelekeiki, FIREBLOOM, 20, Q_H_FIREBLOOM)

QuestReward_wrapper(jamani, HEAVY_LEATHER, 10, Q_H_HEAVY_LEATHER)

QuestReward_wrapper(rawtar, LEAN_WOLF_STEAK, 20, Q_H_LEAN_WOLF_STEAKS)

QuestReward_wrapper(callow, MAGEWEAVE_BANDAGE, 20, Q_H_MAGEWEAVE_BANDAGES)

QuestReward_wrapper(tkelah, MITHRIL_BAR, 20, Q_H_MITHRIL_BARS)

QuestReward_wrapper(proudfeather, PEACEBLOOM, 20, Q_H_PEACEBLOOM)

QuestReward_wrapper(jezel, PURPLE_LOTUS, 20, Q_H_PURPLE_LOTUS)

QuestReward_wrapper(serratus, RUGGED_LEATHER, 10, Q_H_RUGGED_LEATHER)

QuestReward_wrapper(clayhoof, RUNECLOTH_BANDAGE, 20, Q_H_RUNECLOTH_BANDAGES)

QuestReward_wrapper(lindo, SPOTTED_YELLOWTAIL, 20, Q_H_SPOTTED_YELLOWTAIL)

QuestReward_wrapper(umala, THICK_LEATHER, 10, Q_H_THICK_LEATHER)

QuestReward_wrapper(maugh, TIN_BAR, 20, Q_H_TIN_BARS)

QuestReward_wrapper(longrunner, WOOL_BANDAGE, 20, Q_H_WOOL_BANDAGES)

template<AhnQirajResources RESOURCE>
struct MANGOS_DLL_DECL npc_ahnqiraj_turninAI : public ScriptedAI
{
    npc_ahnqiraj_turninAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        if (IsEventCompleted(EVENT_ID) || GetResourceCount(EVENT_ID, RESOURCE) >= GetFullResourceCount(EVENT_ID, RESOURCE))
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void Reset() {}

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

template<AhnQirajResources RESOURCE>
CreatureAI* GetAI_npc_ahnqiraj_turnin(Creature* pCreature)
{
    return new npc_ahnqiraj_turninAI<RESOURCE>(pCreature);
}

void AddSC_gate_turnin_scripts()
{
    Script* pScript = new Script;
    
    pScript->Name = "npc_sgt_maj_germaine";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<ARTHAS_TEARS>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(germaine);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_sgt_stonebrow";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<COPPER_BAR>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(stonebrow);
    pScript->RegisterSelf();
    
    pScript = new Script;
    pScript->Name = "npc_bonnie_stoneflayer";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<LIGHT_LEATHER>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(bonnie);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_sent_silversky";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<LINEN_BANDAGE>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(silversky);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_pvt_porter";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<MEDIUM_LEATHER>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(porter);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_master_nightsong";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<PURPLE_LOTUS>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(nightsong);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_slicky";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<RAINBOW_FIN_ALBACORE>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(slicky);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_sarah_sadwhistle";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<ROAST_RAPTOR>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(sarah);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_kpr_moonshade";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<RUNECLOTH_BANDAGE>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(moonshade);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_nrs_stonefield";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<SILK_BANDAGE>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(stonefield);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_hntr_swiftriver";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<SPOTTED_YELLOWTAIL>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(swiftriver);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_pvt_draxlegauge";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<STRANGLEKELP>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(draxlegauge);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_marta_finespindle";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<THICK_LEATHER>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(marta);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_dame_twinbraid";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<THORIUM_BAR>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(dame);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_cpr_carnes";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<IRON_BAR>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(carnes);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_chief_sharpclaw";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<BAKED_SALMON>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(sharpclaw);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_miner_cromwell";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<COPPER_BAR>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(cromwell);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_batrdr_pelikeiki";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<FIREBLOOM>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(pelekeiki);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_sknr_jamani";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<HEAVY_LEATHER>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(jamani);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_grd_rawtar";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<LEAN_WOLF_STEAK>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(rawtar);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_ldy_callow";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<MAGEWEAVE_BANDAGE>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(callow);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_snr_srg_tkelah";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<MITHRIL_BAR>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(tkelah);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_hrb_proudfeather";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<PEACEBLOOM>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(proudfeather);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_apo_jezel";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<PURPLE_LOTUS>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(jezel);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_dr_serratus";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<RUGGED_LEATHER>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(serratus);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_grd_clayhoof";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<RUNECLOTH_BANDAGE>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(clayhoof);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_fish_lindo";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<SPOTTED_YELLOWTAIL>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(lindo);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_sgt_umala";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<THICK_LEATHER>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(umala);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_grunt_maug";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<TIN_BAR>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(maugh);
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->Name = "npc_hlr_longrunner";
    pScript->GetAI = &GetAI_npc_ahnqiraj_turnin<WOOL_BANDAGE>;
    pScript->pQuestRewardedNPC = GetQuestReward_wrapper(longrunner);
    pScript->RegisterSelf();
}
