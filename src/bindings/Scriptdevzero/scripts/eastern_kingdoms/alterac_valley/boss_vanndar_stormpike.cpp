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

#include "precompiled.h"

enum Spells
{
    SPELL_AVATAR        = 19135,
    SPELL_STORMBOLT     = 20685,
    SPELL_THUNDERCLAP   = 15588,
};

enum Yells
{
    YELL_AGGRO          = -1030008,
    YELL_EVADE          = -1030009,
    YELL_RESPAWN1       = -1030010,
    YELL_RESPAWN2       = -1030011,
    YELL_RANDOM1        = -1030012,
    YELL_RANDOM2        = -1030013,
    YELL_RANDOM3        = -1030014,
    YELL_RANDOM4        = -1030015,
    YELL_RANDOM5        = -1030016,
    YELL_RANDOM6        = -1030017,
    YELL_RANDOM7        = -1030018,
};

struct MANGOS_DLL_DECL boss_vanndar_stormpikeAI : public ScriptedAI
{
    boss_vanndar_stormpikeAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiAvatarTimer;
    uint32 m_uiStormboltTimer;
    uint32 m_uiThunderclapTimer;
    uint32 m_uiYellTimer;
    uint32 m_uiEvadeTimer;

    void Reset()
    {
        m_uiAvatarTimer = 3*IN_MILLISECONDS;
        m_uiStormboltTimer = 6*IN_MILLISECONDS;
        m_uiThunderclapTimer = 4*IN_MILLISECONDS;
        m_uiYellTimer = urand(20*IN_MILLISECONDS,30*IN_MILLISECONDS);
        m_uiEvadeTimer = 5*IN_MILLISECONDS;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(YELL_AGGRO, m_creature);
    }

    void JustReachedHome()
    {
        switch(urand(0,1))
        {
            case 0:
                DoScriptText(YELL_RESPAWN1, m_creature);
                break;
            case 1:
                DoScriptText(YELL_RESPAWN2, m_creature);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Avatar
        if (m_uiAvatarTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_AVATAR);
            m_uiAvatarTimer = urand(15*IN_MILLISECONDS,20*IN_MILLISECONDS);
        }
        else
            m_uiAvatarTimer -= uiDiff;

        // Thundeclap
        if (m_uiThunderclapTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_THUNDERCLAP);
            m_uiThunderclapTimer = urand(5*IN_MILLISECONDS,15*IN_MILLISECONDS);
        }
        else
            m_uiThunderclapTimer -= uiDiff;

        // Stormbolt
        if (m_uiStormboltTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STORMBOLT);
            m_uiStormboltTimer = urand(10*IN_MILLISECONDS,25*IN_MILLISECONDS);
        }
        else
            m_uiStormboltTimer -= uiDiff;

        // Yell
        if (m_uiYellTimer <= uiDiff)
        {
            switch(urand(0,6))
            {
                case 0:
                    DoScriptText(YELL_RANDOM1, m_creature);
                    break;
                case 1:
                    DoScriptText(YELL_RANDOM2, m_creature);
                    break;
                case 2:
                    DoScriptText(YELL_RANDOM3, m_creature);
                    break;
                case 3:
                    DoScriptText(YELL_RANDOM4, m_creature);
                    break;
                case 4:
                    DoScriptText(YELL_RANDOM5, m_creature);
                    break;
                case 5:
                    DoScriptText(YELL_RANDOM6, m_creature);
                    break;
                case 6:
                    DoScriptText(YELL_RANDOM7, m_creature);
                    break;
            }
            m_uiYellTimer = urand(20*IN_MILLISECONDS,30*IN_MILLISECONDS);
        }
        else
            m_uiYellTimer -= uiDiff;

        // Check if creature is not outside of building
        if (m_uiEvadeTimer <= uiDiff)
        {
            float fX, fY, fZ;
            m_creature->GetRespawnCoord(fX,fY,fZ);
            if (m_creature->GetDistance2d(fX, fY) > 23.0f)
            {
                ResetToHome();
                DoScriptText(YELL_EVADE, m_creature);
            }
            m_uiEvadeTimer = 5*IN_MILLISECONDS;
        }
        else
            m_uiEvadeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_vanndar_stormpike(Creature* pCreature)
{
    return new boss_vanndar_stormpikeAI(pCreature);
}

void AddSC_boss_vanndar_stormpike()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_vanndar_stormpike";
    pNewscript->GetAI = &GetAI_boss_vanndar_stormpike;
    pNewscript->RegisterSelf();
}
