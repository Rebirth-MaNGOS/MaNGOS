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

enum Creatures
{
    NPC_NORTH_MARSHAL                             = 14762,
    NPC_SOUTH_MARSHAL                             = 14763,
    NPC_ICEWING_MARSHAL                           = 14764,
    NPC_STONEHEARTH_MARSHAL                       = 14765,
    NPC_EAST_FROSTWOLF_WARMASTER                  = 14772,
    NPC_ICEBLOOD_WARMASTER                        = 14773,
    NPC_TOWER_POINT_WARMASTER                     = 14776,
    NPC_WEST_FROSTWOLF_WARMASTER                  = 14777,
};

enum Spells
{
    SPELL_CHARGE                                  = 22911,
    SPELL_CLEAVE                                  = 15584,
    SPELL_DEMORALIZING_SHOUT                      = 23511,
    SPELL_ENRAGE                                  = 8599,
    SPELL_WHIRLWIND1                              = 15589,
    SPELL_WHIRLWIND2                              = 13736,
    
    // Are there some preTBC equivalents ?
    SPELL_NORTH_MARSHAL                           = 45828,
    SPELL_SOUTH_MARSHAL                           = 45829,
    SPELL_STONEHEARTH_MARSHAL                     = 45830,
    SPELL_ICEWING_MARSHAL                         = 45831,
    SPELL_ICEBLOOD_WARMASTER                      = 45822,
    SPELL_TOWER_POINT_WARMASTER                   = 45823,
    SPELL_WEST_FROSTWOLF_WARMASTER                = 45824,
    SPELL_EAST_FROSTWOLF_WARMASTER                = 45826,
};

struct MANGOS_DLL_DECL mob_av_marshal_or_warmasterAI : public ScriptedAI
{
    mob_av_marshal_or_warmasterAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool m_bHasAura;

    uint32 m_uiChargeTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiDemoralizingShoutTimer;
    uint32 m_uiWhirlwind1Timer;
    uint32 m_uiWhirlwind2Timer;
    uint32 m_uiEvadeTimer;

    void Reset()
    {
        m_bHasAura = false;

        m_uiChargeTimer = urand(2*IN_MILLISECONDS,12*IN_MILLISECONDS);
        m_uiCleaveTimer = urand(1*IN_MILLISECONDS,11*IN_MILLISECONDS);
        m_uiDemoralizingShoutTimer = urand(2*IN_MILLISECONDS,2*IN_MILLISECONDS);
        m_uiWhirlwind1Timer = urand(1*IN_MILLISECONDS,12*IN_MILLISECONDS);
        m_uiWhirlwind2Timer = urand(5*IN_MILLISECONDS,20*IN_MILLISECONDS);
        m_uiEvadeTimer = 5*IN_MILLISECONDS;
    }

	void JustRespawned()
	{
		Reset();
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bHasAura)
        {
            switch(m_creature->GetEntry())
            {
                case NPC_NORTH_MARSHAL:
                    DoCastSpellIfCan(m_creature, SPELL_NORTH_MARSHAL);
                    break;
                case NPC_SOUTH_MARSHAL:
                    DoCastSpellIfCan(m_creature, SPELL_SOUTH_MARSHAL);
                    break;
                case NPC_STONEHEARTH_MARSHAL:
                    DoCastSpellIfCan(m_creature, SPELL_STONEHEARTH_MARSHAL);
                    break;
                case NPC_ICEWING_MARSHAL:
                    DoCastSpellIfCan(m_creature, SPELL_ICEWING_MARSHAL);
                    break;
                case NPC_EAST_FROSTWOLF_WARMASTER:
                    DoCastSpellIfCan(m_creature, SPELL_EAST_FROSTWOLF_WARMASTER);
                    break;
                case NPC_WEST_FROSTWOLF_WARMASTER:
                    DoCastSpellIfCan(m_creature, SPELL_WEST_FROSTWOLF_WARMASTER);
                    break;
                case NPC_ICEBLOOD_WARMASTER:
                    DoCastSpellIfCan(m_creature, SPELL_ICEBLOOD_WARMASTER);
                    break;
                case NPC_TOWER_POINT_WARMASTER:
                    DoCastSpellIfCan(m_creature, SPELL_TOWER_POINT_WARMASTER);
                    break;
            }

            m_bHasAura = true;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Enrage
        if (HealthBelowPct(40))
            DoCastSpellIfCan(m_creature, SPELL_ENRAGE, CAST_AURA_NOT_PRESENT);

        // Charge
        if (m_uiChargeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHARGE);
            m_uiChargeTimer = urand(10*IN_MILLISECONDS,25*IN_MILLISECONDS);
        }
        else
            m_uiChargeTimer -= uiDiff;

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer =  urand(10*IN_MILLISECONDS,16*IN_MILLISECONDS);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Demoralizing Shout
        if (m_uiDemoralizingShoutTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEMORALIZING_SHOUT);
            m_uiDemoralizingShoutTimer = urand(10*IN_MILLISECONDS,15*IN_MILLISECONDS);
        }
        else
            m_uiDemoralizingShoutTimer -= uiDiff;

        // Whirlwind 1
        if (m_uiWhirlwind1Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND1);
            m_uiWhirlwind1Timer = urand(6*IN_MILLISECONDS,20*IN_MILLISECONDS);
        }
        else
            m_uiWhirlwind1Timer -= uiDiff;

        // Whirlwind 1
        if (m_uiWhirlwind2Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND2);
            m_uiWhirlwind2Timer = urand(10*IN_MILLISECONDS,25*IN_MILLISECONDS);
        }
        else
            m_uiWhirlwind2Timer -= uiDiff;

        // Check if creature is not outside of building
        if (m_uiEvadeTimer <= uiDiff)
        {
            float fX, fY, fZ;
            m_creature->GetRespawnCoord(fX,fY,fZ);
            if (m_creature->GetDistance2d(fX, fY) > 50.0f)
                ResetToHome();
            m_uiEvadeTimer = 5*IN_MILLISECONDS;
        }
        else
            m_uiEvadeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_av_marshal_or_warmaster(Creature* pCreature)
{
    return new mob_av_marshal_or_warmasterAI(pCreature);
}

/*######
## mob_av_mount_lieutenant
######*/

struct MANGOS_DLL_DECL mob_av_mount_lieutenantAI : public ScriptedAI
{
    mob_av_mount_lieutenantAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset() 
    {
        // reload their mount
        m_creature->LoadCreatureAddon();
    }

    void Aggro(Unit* /*pWho*/)
    {
        // don't wanna be mounted while combat
         if (m_creature->IsMounted())
            m_creature->Unmount();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_av_mount_lieutenant(Creature* pCreature)
{
    return new mob_av_mount_lieutenantAI(pCreature);
}

void AddSC_alterac_valley()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "mob_av_marshal_or_warmaster";
    pNewscript->GetAI = &GetAI_mob_av_marshal_or_warmaster;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "mob_av_mount_lieutenant";
    pNewscript->GetAI = &GetAI_mob_av_mount_lieutenant;
    pNewscript->RegisterSelf();
}
