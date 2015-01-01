/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Boss_Ossirian
SD%Complete: 90
SDComment: Correct tornado, rework crystals without summoning them
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
//#include "World.h"
//#include "Weather.h"
#include "ruins_of_ahnqiraj.h"

enum Yells
{
    SAY_SURPREME1                 = -1509018,
    SAY_SURPREME2                 = -1509019,
    SAY_SURPREME3                 = -1509020,
    SAY_RAND_INTRO1               = -1509021,
    SAY_RAND_INTRO2               = -1509022,
    SAY_RAND_INTRO3               = -1509023,
    SAY_RAND_INTRO4               = -1509024,
    SAY_AGGRO                     = -1509025,
    SAY_SLAY                      = -1509026,
    SAY_DEATH                     = -1509027
};

enum Spells
{
    SPELL_WAR_STOMP               = 25188,
    SPELL_CURSE_OF_TONGUES        = 25195,
    SPELL_ENVELOPING_WINGS        = 25189,
    SPELL_STRENGTH_OF_OSSIRIAN    = 25176,
    SPELL_SUMMON_PLAYER           = 20477,
    SPELL_FIRE_WEAKNESS           = 25177,
    SPELL_FROST_WEAKNESS          = 25178,
    SPELL_NATURE_WEAKNESS         = 25180,
    SPELL_ARCANE_WEAKNESS         = 25181,
    SPELL_SHADOW_WEAKNESS         = 25183
};

static Loc Crystal[]=
{
    {-9449.53f, 1988.67f, 85.91f, 0, 0}, // first
    {-9375.54f, 2061.64f, 85.91f, 0, 0},
    {-9254.70f, 1952.90f, 85.55f, 0, 0},
    {-9197.60f, 1849.50f, 85.55f, 0, 0},
    {-9301.75f, 1748.75f, 85.55f, 0, 0},
    {-9393.15f, 1835.80f, 85.55f, 0, 0},
    {-9509.55f, 1864.75f, 85.55f, 0, 0}
};

static Loc Tornado[]=
{
    {-9444.0f,1857.0f,85.55f, 0, 0},
    {-9352.0f,2012.0f,85.55f, 0, 0}
};

struct MANGOS_DLL_DECL boss_ossirianAI : public ScriptedAI
{
    boss_ossirianAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((instance_ruins_of_ahnqiraj*)pCreature->GetInstanceData());
        Reset();
    }

    instance_ruins_of_ahnqiraj* m_pInstance;

    uint32 m_uiWarStompTimer;
    uint32 m_uiCurseOfTonguesTimer;
    uint32 m_uiEnvelopingWingsTimer;

    void Reset()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_OSSIRIAN, NOT_STARTED);
            DespawnLastSummonedCrystal();
        }

        m_uiWarStompTimer = urand(8000,12000);
        m_uiCurseOfTonguesTimer = urand(15000,20000);
        m_uiEnvelopingWingsTimer = urand(15000,20000);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OSSIRIAN, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_STRENGTH_OF_OSSIRIAN);

        /*uint32 zoneid = m_creature->GetZoneId();
        if (Weather* pWth = sWorld.FindWeather(zoneid))
            pWth->SetWeather(WeatherType(3), 2);*/

        for (uint8 i = 0; i < 2; ++i)
            m_creature->SummonCreature(NPC_TORNADO, Tornado[i].x, Tornado[i].y, Tornado[i].z, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);

        // This spawn first Ossirian Crystal
        //who->SummonGameObject(GO_OSSIRIAN_CRYSTAL, Crystal[i].x, Crystal[i].x, Crystal[i].x, 0, 0, 0, 0, 0, 0);
    }
    
    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_OSSIRIAN, DONE);
            DespawnLastSummonedCrystal();
        }

        std::list<Creature*> m_lTornados;
        GetCreatureListWithEntryInGrid(m_lTornados, m_creature, NPC_TORNADO, MAX_VISIBILITY_DISTANCE);
        if (!m_lTornados.empty())
            for(std::list<Creature*>::iterator itr = m_lTornados.begin(); itr != m_lTornados.end(); ++itr)
                if ((*itr) && (*itr)->isAlive())
                    (*itr)->RemoveFromWorld();
    }

    void DespawnLastSummonedCrystal()
    {
		if (GameObject* pLastCrystal = m_pInstance->GetSingleGameObjectFromStorage(GO_OSSIRIAN_CRYSTAL))
            pLastCrystal->AddObjectToRemoveList();
    }

    void TeleportFarAwayPlayerBack(float /*range = 0.0f*/, bool alive = true)
    {
        Map* map = m_creature->GetMap();
        if(!map->IsDungeon()) return;

        Map::PlayerList const &PlayerList = map->GetPlayers();
        if(PlayerList.isEmpty())
            return;
        for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            //if(!i->getSource()->IsWithinDistInMap(m_creature, range))
            if(i->getSource()->GetAreaId() != AREAID_WATCHERS_TERRACE)
            {
                //if(m_creature->GetAreaId() != AREAID_WATCHERS_TERRACE)
                //    DoTeleportTo(Spawns[0][0], Spawns[0][1], Spawns[0][2], 0);
                float mX, mY, mZ, mO;
                m_creature->GetPosition(mX, mY, mZ);
                mO = m_creature->GetOrientation();
                
                if(alive)
                if(i->getSource()->isAlive())
                    DoTeleportPlayer(i->getSource(), mX, mY, mZ+1, mO);
                if(!alive)
                    DoTeleportPlayer(i->getSource(), mX, mY, mZ+1, mO);
            }
            if(i == PlayerList.end()) i = PlayerList.begin(); // make impossible reach end
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If has not boost nor weakness  
        if (!m_creature->HasAura(SPELL_STRENGTH_OF_OSSIRIAN, EFFECT_INDEX_0) && !m_creature->HasAura(SPELL_FIRE_WEAKNESS, EFFECT_INDEX_0) && !m_creature->HasAura(SPELL_FROST_WEAKNESS, EFFECT_INDEX_0) && !m_creature->HasAura(SPELL_NATURE_WEAKNESS, EFFECT_INDEX_0) && !m_creature->HasAura(SPELL_ARCANE_WEAKNESS, EFFECT_INDEX_0) && !m_creature->HasAura(SPELL_SHADOW_WEAKNESS, EFFECT_INDEX_0))
        {
            DoCastSpellIfCan(m_creature, SPELL_STRENGTH_OF_OSSIRIAN);
            switch(urand(0,2))
            {
                case 0:
                    DoScriptText(SAY_SURPREME1, m_creature);
                    break;
                case 1:
                    DoScriptText(SAY_SURPREME2, m_creature);
                    break;
                case 2:
                    DoScriptText(SAY_SURPREME3, m_creature);
                    break;
            }
        }

        // War Stom
        if (m_uiWarStompTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WAR_STOMP);
            m_uiWarStompTimer = urand(8000,10000);
        }
        else
            m_uiWarStompTimer -= uiDiff;

        // Curse of Tongues
        if (m_uiCurseOfTonguesTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_TONGUES);
            m_uiCurseOfTonguesTimer = urand(15000,20000);
        }
        else
            m_uiCurseOfTonguesTimer -= uiDiff;

        // Enveloping Wings
        if (m_uiEnvelopingWingsTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ENVELOPING_WINGS);
            m_uiEnvelopingWingsTimer = urand(15000,20000);
        }
        else
            m_uiEnvelopingWingsTimer -= uiDiff;

        TeleportFarAwayPlayerBack(40);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ossirian(Creature* pCreature)
{
    return new boss_ossirianAI(pCreature);
}

bool GOUse_go_ossirian_crystal(Player* pPlayer, GameObject* pGo)
{
    if (!pPlayer)
        return false;

    if (Creature* pOssirian = GetClosestCreatureWithEntry(pGo, NPC_OSSIRIAN, 20.0f))
    {
        pOssirian->RemoveAurasDueToSpell(SPELL_STRENGTH_OF_OSSIRIAN);
        pOssirian->RemoveAurasDueToSpell(SPELL_FIRE_WEAKNESS);
        pOssirian->RemoveAurasDueToSpell(SPELL_FROST_WEAKNESS);
        pOssirian->RemoveAurasDueToSpell(SPELL_NATURE_WEAKNESS);
        pOssirian->RemoveAurasDueToSpell(SPELL_ARCANE_WEAKNESS);
        pOssirian->RemoveAurasDueToSpell(SPELL_SHADOW_WEAKNESS);

        switch(urand(0,4))
        {
            case 0:
                pPlayer->CastSpell(pOssirian, SPELL_FIRE_WEAKNESS, false);
                break;
            case 1:
                pPlayer->CastSpell(pOssirian, SPELL_FROST_WEAKNESS, false);
                break;
            case 2:
                pPlayer->CastSpell(pOssirian, SPELL_NATURE_WEAKNESS, false);
                break;
            case 3:
                pPlayer->CastSpell(pOssirian, SPELL_ARCANE_WEAKNESS, false);
                break;
            case 4:
                pPlayer->CastSpell(pOssirian, SPELL_SHADOW_WEAKNESS, false);
                break;
        }
    }
    
    // Spawn next Ossirian Crystal at random position
    bool RollAgain = true;
    while(RollAgain)
    {
        uint8 random = rand()%8;
        // If positionX of new crystal != positionX current crystal, spawn new crystal and stop roll
        if(pGo->GetPositionX() != Crystal[random].x)
        {
            //GameObject* Crystal = player->SummonGameObject(GO_OSSIRIAN_CRYSTAL, Crystal[random].x, Crystal[random].y, Crystal[random].z, 0, 0, 0, 0, 0, 0);
            RollAgain = false;
        }
    }
    
    pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE + GO_FLAG_INTERACT_COND); // clicked crystal become unclickable
    pGo->SetGoState(GO_STATE_ACTIVE);
    pGo->SetRespawnTime(10); // despawn after 10 secs
    
    return true;
}

void AddSC_boss_ossirian()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_ossirian";
    pNewscript->GetAI = &GetAI_boss_ossirian;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_ossirian_crystal";
    pNewscript->pGOUse = &GOUse_go_ossirian_crystal;
    pNewscript->RegisterSelf();
}
