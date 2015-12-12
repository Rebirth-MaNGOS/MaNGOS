#include "precompiled.h"
#include "sc_creature.h"
#include "../ScriptMgr.h"
#include "Player.h"
#include "ahnqiraj_turnin_defines.h"
#include "SharedDefines.h"
#include "EventResourceMgr.h"
#include <sstream>

/*
 Gossip ID saying "The horde", in order 0, 25, 50, 75, 100% complete
 102,103,104,105,106
 */

/*
 Gossip ID saying "The alliance", in order 0, 25, 50, 75, 100% complete
 107,108,109,110,111
 */

static AhnQirajResources allianceResources[] = { ARTHAS_TEARS, COPPER_BAR, LIGHT_LEATHER,
                                                 LINEN_BANDAGE, MEDIUM_LEATHER, PURPLE_LOTUS,
                                                 RAINBOW_FIN_ALBACORE, ROAST_RAPTOR, RUNECLOTH_BANDAGE,
                                                 SILK_BANDAGE, SPOTTED_YELLOWTAIL, STRANGLEKELP,
                                                 THICK_LEATHER, THORIUM_BAR, IRON_BAR };

static AhnQirajResources hordeResources[] = { BAKED_SALMON, COPPER_BAR, FIREBLOOM, HEAVY_LEATHER,
                                              LEAN_WOLF_STEAK, MAGEWEAVE_BANDAGE, MITHRIL_BAR,
                                              PEACEBLOOM, PURPLE_LOTUS, RUGGED_LEATHER, RUNECLOTH_BANDAGE,
                                              SPOTTED_YELLOWTAIL, THICK_LEATHER, TIN_BAR, WOOL_BANDAGE };

struct MANGOS_DLL_DECL general_zogAI : public ScriptedAI
{
    general_zogAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void Reset()
    {
    }

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        if (emote == TEXTEMOTE_KNEEL)
        {
            std::stringstream ss;
            ss << "General Zog acknowledges " << pPlayer->GetName() << "'s obedience.";
            m_creature->GenericTextEmote(ss.str().c_str(), pPlayer);
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_general_zog(Creature* pCreature)
{
    return new general_zogAI(pCreature);
}

struct MANGOS_DLL_DECL commander_stronghammerAI : public ScriptedAI
{
    commander_stronghammerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_commander_stronghammer(Creature* pCreature)
{
    return new commander_stronghammerAI(pCreature);
}

template<Team FACTION>
bool GossipHello_ambassador(Player* pPlayer, Creature* pCreature)
{
    AhnQirajResources* chosen;
    if (FACTION == HORDE)
    {
        chosen = hordeResources; 
    }
    else
    {
        chosen = allianceResources;
    }
    
    float current = 0;
    float max = 0;
    for (short i = 0; i < 15; i++)
    {
        current += GetResourceCount(EVENT_ID, chosen[i]);
        max += GetFullResourceCount(EVENT_ID, chosen[i]);
    }
    float ratio = current/max * 100;
    uint32 uRatio = floor(ratio);

    uint32 chosenText = FACTION == HORDE ? 101 : 106;
    if (uRatio == 100)
    {
        chosenText += 5;
    }
    else if (uRatio > 75)
    {
        chosenText += 4;
    }
    else if (uRatio > 50)
    {
        chosenText += 3;
    }
    else if (uRatio > 25)
    {
        chosenText += 2;
    }
    else 
    {
        chosenText += 1;
    }

    pPlayer->SEND_GOSSIP_MENU(chosenText, pCreature->GetObjectGuid());
    return true;
}

void AddSC_ambassadors()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_general_zog";
    pNewScript->GetAI = &GetAI_general_zog;
    pNewScript->pGossipHello = &GossipHello_ambassador<HORDE>;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_commander_stronghammer";
    pNewScript->GetAI = &GetAI_commander_stronghammer;
    pNewScript->pGossipHello = &GossipHello_ambassador<ALLIANCE>;
    pNewScript->RegisterSelf();
}
