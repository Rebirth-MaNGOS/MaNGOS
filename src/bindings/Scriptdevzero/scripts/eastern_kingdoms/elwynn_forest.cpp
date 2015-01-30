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
SDName: Elwynn_Forest
SD%Complete: 50
SDComment: Quest support: 1786
SDCategory: Elwynn Forest
EndScriptData */

/* ContentData
npc_henze_faulk
EndContentData */

#include "precompiled.h"
#include "patrol_ai.h"

#include <vector>

/*######
## npc_henze_faulk
######*/

#define SAY_HEAL    -1000187

struct MANGOS_DLL_DECL npc_henze_faulkAI : public ScriptedAI
{
    uint32 lifeTimer;
    bool spellHit;

    npc_henze_faulkAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        lifeTimer = 120000;
        m_creature->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);   // lay down
        spellHit = false;
    }

    void MoveInLineOfSight(Unit* /*who*/) { }

    void UpdateAI(const uint32 diff)
    {
        if (m_creature->IsStandState())
        {
            if (lifeTimer < diff)
                m_creature->AI()->ResetToHome();
            else
                lifeTimer -= diff;
        }
    }

    void SpellHit(Unit *Hitter, const SpellEntry *Spellkind)
    {
        if (Spellkind->Id == 8593 && !spellHit)
        {
            DoCastSpellIfCan(m_creature,32343);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
            //m_creature->RemoveAllAuras();
            DoScriptText(SAY_HEAL, m_creature, Hitter);
            spellHit = true;
        }
    }

};
CreatureAI* GetAI_npc_henze_faulk(Creature* pCreature)
{
    return new npc_henze_faulkAI(pCreature);
}

/*####################
 * npc_creepy_child ##
 * #################*/


enum Children
{
    DANA        = 804,
    CAMERON     = 805,
    JOSE        = 811,
    ARON        = 810,
    LISA        = 807,
    JOHN        = 806
};

enum CreepySounds
{
    CTHUN_DEATH_IS_CLOSE = 8580,
    CTHUN_YOU_WILL_DIE = 8585,
};

static const uint32 m_ChildEntries[5] = { JOHN, JOSE, ARON, LISA, DANA };
// The AI for the middle child.
struct MANGOS_DLL_DECL npc_creepy_child : public npc_patrolAI
{
    npc_creepy_child(Creature* pCreature) : npc_patrolAI(pCreature, 0.f, false)
    {
        Reset();
    }
    
    float m_ChildOffsets[5][2];
    float direction;
    std::vector<Unit*> m_ChildList;
    
    uint32 m_uiChildCatchingTimer;
    
    void Reset()
    {
        npc_patrolAI::Reset();
        
        m_uiChildCatchingTimer = 20000;
        direction = 0;
        m_ChildList.clear();
        
        InitialiseChildOffsets();
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        // Only do this once after reset.
        if (m_uiChildCatchingTimer)
        {
            if (m_uiChildCatchingTimer <= uiDiff)
            {
                GetChildren();
                m_creature->SetSplineFlags(SPLINEFLAG_WALKMODE);
                // Position the children around this creature.
                float origo[2];
                origo[0] = m_creature->GetPositionX();
                origo[1] = m_creature->GetPositionY();
                PositionChildren(origo);

                StartPatrol(0, true);
                
                m_uiChildCatchingTimer = 0;
            }
            else
                m_uiChildCatchingTimer -= uiDiff;
        }
    }
    
    void MovementInform(uint32 movementType, uint32 pointId)
    {
        npc_patrolAI::MovementInform(movementType, pointId);

        //if(pointId == 15)
        //    InitialiseChildOffsets(-0.5 * PI);
        
        float origo[2] = { GetTargetWaypoint().fX, GetTargetWaypoint().fY };
        
        m_creature->MonsterMove(GetTargetWaypoint().fX, GetTargetWaypoint().fY, GetTargetWaypoint().fZ, (sqrtf(m_creature->GetDistanceSqr(GetTargetWaypoint().fX, GetTargetWaypoint().fY, GetTargetWaypoint().fZ))/m_creature->GetSpeed(MOVE_RUN)) * 1000);
    }
    
    void PositionChildren(float origo[2])
    {
        if (m_ChildList.size() < 5)
            return;

        for (short i = 0; i < 5; ++i)
        {
            float pos[2];
            
            CalculateChildPosFromOffset(origo, m_ChildOffsets[i], pos);
            m_ChildList[i]->MonsterMove(pos[0], pos[1], GetTargetWaypoint().fZ, (sqrtf(m_ChildList[i]->GetDistanceSqr(GetTargetWaypoint().fX, GetTargetWaypoint().fY, GetTargetWaypoint().fZ))/m_ChildList[i]->GetSpeed(MOVE_RUN)) * 1000);
           // m_ChildList[i]->GetMotionMaster()->MovePoint(0, pos[0], pos[1], GetTargetWaypoint().fZ, false);//>MonsterMove(pos[0], pos[1], GetTargetWaypoint().fZ, m_ChildList[i]->GetDistanceSqr(pos[0], pos[1], GetTargetWaypoint().fZ));//
        }
    }
    
    void GetChildren()
    {
        for (short i = 0; i < 5; ++i)
        {
            Unit* child = GetClosestCreatureWithEntry(m_creature, m_ChildEntries[i], 10.f);
            if (child)
            {
                m_ChildList.push_back(child);
                ((Creature*)child)->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
            }
        }
    }
    
    // Give the children offsets that match a pentagram.
    void InitialiseChildOffsets(float rotation = 0.f)
    {
        for (short i = 0; i < 5; ++i)
        {
            if (i != 4)
                CalculateNewChildOffset(i < 2 ? 3.5f : 2.f, i * (PI / 2.f) + PI / 4.f + rotation, m_ChildOffsets[i]);
            else
                CalculateNewChildOffset(4.f, PI / 2 + rotation, m_ChildOffsets[i]);
        }
    }
    
    // Takes an offset and a center point and returns the childs position in the same
    // coordinates as the center was given as.
    inline void CalculateChildPosFromOffset(float origo[2], float offset[2], float* result)
    {
        result[0] = origo[0] + offset[0];
        result[1] = origo[1] + offset[1];
    }
    
    // Calculates a child's offset from origo for the given radius and angle.
    inline void CalculateNewChildOffset(float radius, float angle, float* result)
    {
        result[0] = radius * (float) cos(angle);
        result[1] = radius * (float) sin(angle);
    }
};

CreatureAI* GetAI_npc_creepy_child(Creature* pCreature)
{
    return new npc_creepy_child(pCreature);
}

void AddSC_elwynn_forest()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_henze_faulk";
    pNewscript->GetAI = &GetAI_npc_henze_faulk;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "npc_creepy_child";
    pNewscript->GetAI = &GetAI_npc_creepy_child;
    pNewscript->RegisterSelf();
}
