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
SDName: Bosses Ebonroc, Firemaw, Flamegor
SD%Complete: 100
SDComment:
SDCategory: Blackwing Lair
EndScriptData */

#include "precompiled.h"
#include "blackwing_lair.h"

enum eBlackwingDragon
{
    EMOTE_GENERIC_FRENZY        = -1000002,

    SPELL_SHADOW_FLAME          = 22539,
    SPELL_THRASH                = 3391,
    SPELL_WING_BUFFET           = 23339,
    SPELL_SUMMON_PLAYER         = 20279,

    SPELL_SHADOW_OF_EBONROC     = 23340,        // Ebonroc
    SPELL_FLAME_BUFFET          = 23341,        // Firemaw
    SPELL_FRENZY                = 23342,        // Flamegor
};

#define SUMMON_PLAYER_TRIGGER_DISTANCE 65.0f

struct MANGOS_DLL_DECL boss_blackwing_dragonAI : public ScriptedAI
{
    boss_blackwing_dragonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackwing_lair*)pCreature->GetInstanceData();
		m_creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_FIRE, true);
        Reset();

        switch(m_creature->GetEntry())
        {
            case NPC_EBONROC:
                m_uiSpecialSpellId = SPELL_SHADOW_OF_EBONROC;
                break;
            case NPC_FIREMAW:
                m_uiSpecialSpellId = SPELL_FLAME_BUFFET;
                break;
            case NPC_FLAMEGOR:
                m_uiSpecialSpellId = SPELL_FRENZY;
                break;
            default:        // shouldn't happen
                m_uiSpecialSpellId = SPELL_FRENZY;
                break;
        }
    }

    instance_blackwing_lair* m_pInstance;

    uint32 m_uiShadowFlameTimer;
    uint32 m_uiTrashTimer;
    uint32 m_uiWingBuffetTimer;
    uint32 m_uiSummonPlayerTimer;

    uint32 m_uiSpecialSpellId;
    uint32 m_uiSpecialSpellTimer;

    void Reset()
    {
        m_uiShadowFlameTimer = 21000;
        m_uiTrashTimer = 15000;
        m_uiWingBuffetTimer = 35000;
        m_uiSummonPlayerTimer = 30000;
        m_uiSpecialSpellTimer = 10000;
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_WING_BUFFET && pTarget)
        {
            if (m_creature->getThreatManager().getThreat(pTarget))
                m_creature->getThreatManager().modifyThreatPercent(pTarget,-50);
        }                 
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Special Spell
        if (m_uiSpecialSpellTimer <= uiDiff)
        {
			m_creature->CastSpell(m_uiSpecialSpellId == SPELL_FRENZY ? m_creature : m_creature->getVictim(), m_uiSpecialSpellId, false);

			if (m_uiSpecialSpellId == SPELL_FRENZY)
			{
				DoScriptText(EMOTE_GENERIC_FRENZY, m_creature);
				m_uiSpecialSpellTimer = urand(10000, 15000);
			}
			else if (m_uiSpecialSpellId == SPELL_FLAME_BUFFET)
				m_uiSpecialSpellTimer = urand(3000, 5000);
			else
				m_uiSpecialSpellTimer = urand(15000, 20000);

        }
        else
            m_uiSpecialSpellTimer -= uiDiff;

        // Shadow Flame
        if (m_uiShadowFlameTimer <= uiDiff)
        {
			m_creature->CastSpell(m_creature->getVictim(), SPELL_SHADOW_FLAME, false);
			m_uiShadowFlameTimer = urand(15000, 22000);
        }
        else
            m_uiShadowFlameTimer -= uiDiff;

        // Thrash
        if (m_uiTrashTimer <= uiDiff)
        {
			m_creature->CastSpell(m_creature->getVictim(), SPELL_THRASH, false);
			m_uiTrashTimer = urand(15000,25000);
        }
        else
            m_uiTrashTimer -= uiDiff;

        // Wing Buffet
        if (m_uiWingBuffetTimer <= uiDiff)
        {
			m_creature->CastSpell(m_creature->getVictim(), SPELL_WING_BUFFET, false);	
			m_uiWingBuffetTimer = 25000;
        }
        else
            m_uiWingBuffetTimer -= uiDiff;

        // Summon Player
        /*if (m_uiSummonPlayerTimer <= uiDiff)
        {
            if (m_creature->getVictim()->GetTypeId() == TYPEID_PLAYER && !m_creature->getVictim()->IsWithinDist(m_creature, SUMMON_PLAYER_TRIGGER_DISTANCE))
            {
                Position pos;
                m_creature->GetPosition(pos.x, pos.y, pos.z);
                pos.o = m_creature->GetOrientation();
                ((Player*)m_creature->getVictim())->TeleportTo(m_creature->GetMapId(), pos.x, pos.y, pos.z, -pos.o);
            }
            m_uiSummonPlayerTimer = 2000;
        }
        else
            m_uiSummonPlayerTimer -= uiDiff;*/

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_blackwing_dragon(Creature* pCreature)
{
    return new boss_blackwing_dragonAI(pCreature);
}

void AddSC_boss_blackwing_dragon()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_blackwing_dragon";
    pNewScript->GetAI = &GetAI_boss_blackwing_dragon;
    pNewScript->RegisterSelf();
}
