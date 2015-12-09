#include "precompiled.h"
#include "sc_creature.h"
#include "../ScriptMgr.h"
#include "Player.h"
#include <sstream>

/*
 Gossip ID saying "The horde", in order 0, 25, 50, 75, 100% complete
 102,103,104,105,106
 */

/*
 Gossip ID saying "The alliance", in order 0, 25, 50, 75, 100% complete
 107,108,109,110,111
 */

struct MANGOS_DLL_DECL general_zogAI : public ScriptedAI
{
    general_zogAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
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

void AddSC_ambassadors()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_general_zog";
    pNewScript->GetAI = &GetAI_general_zog;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_commander_stronghammer";
    pNewScript->GetAI = &GetAI_commander_stronghammer;
    pNewScript->RegisterSelf();
}
