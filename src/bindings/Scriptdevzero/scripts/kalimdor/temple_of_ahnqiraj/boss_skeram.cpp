/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss_Skeram
SD%Complete: 75
SDComment: Mind Control buggy.
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "temple_of_ahnqiraj.h"
#include "Group.h"
#include <vector>

#define SAY_AGGRO1                  -1531000
#define SAY_RANDOM4                  -1531001
#define SAY_RANDOM5                  -1531002
#define SAY_SLAY1                   -1531003
#define SAY_RANDOM1                  -1531004
#define SAY_RANDOM2                   -1531005
#define SAY_RANDOM3                   -1531006
#define SAY_DEATH                   -1531007

#define SPELL_ARCANE_EXPLOSION      25679
#define SPELL_EARTH_SHOCK           26194
#define SPELL_TRUE_FULFILLMENT4     785
#define SPELL_BLINK                 28391

class ov_mycoordinates
{
    public:
        float x,y,z,r;
        ov_mycoordinates(float cx, float cy, float cz, float cr)
        {
            x = cx; y = cy; z = cz; r = cr;
        }
};

struct MANGOS_DLL_DECL boss_skeramAI : public ScriptedAI
{
    boss_skeramAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        IsImage = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 ArcaneExplosion_Timer;
    uint32 EarthShock_Timer;
    uint32 FullFillment_Timer;
    uint32 Blink_Timer;
    uint32 Invisible_Timer;
    uint32 m_uiRandomYellTimer;

    Creature *Image1, *Image2;

    bool Images75;
    bool Images50;
    bool Images25;
    bool IsImage;
    bool Invisible;

    std::vector<ObjectGuid> m_MCVictimList;

    void Reset()
    {
        ArcaneExplosion_Timer = urand(6000, 12000);
        EarthShock_Timer = 2000;
        FullFillment_Timer = 15000;
        Blink_Timer = urand(8000, 20000);
        Invisible_Timer = 500;
        m_uiRandomYellTimer = urand(600000, 1800000);

        Images75 = false;
        Images50 = false;
        Images25 = false;
        Invisible = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetVisibility(VISIBILITY_ON);

        if (IsImage)
            m_creature->SetDeathState(JUST_DIED);

        // Kill MCed people on reset.
        for (ObjectGuid guid : m_MCVictimList)
        {
            Player* pVictim = m_creature->GetMap()->GetPlayer(guid);
            if (pVictim && pVictim->HasAura(SPELL_TRUE_FULFILLMENT4, EFFECT_INDEX_0))
                m_creature->DealDamage(pVictim, pVictim->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, true);
        }
        m_MCVictimList.clear();
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(SAY_SLAY1, m_creature);
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (!IsImage)
        {
            DoScriptText(SAY_DEATH, m_creature);

            if (m_pInstance)
                m_pInstance->SetData(TYPE_SKERAM, DONE);
        }
        
        // Remove the MC from MCed people. 
        for (ObjectGuid guid : m_MCVictimList)
        {
            if (Player* pVictim = m_creature->GetMap()->GetPlayer(guid))
            {
                if (Aura* pAura = pVictim->GetAura(SPELL_TRUE_FULFILLMENT4, EFFECT_INDEX_0))
                {
                    pVictim->RemoveAura(pAura, AURA_REMOVE_BY_CANCEL);
                }
            }

        }
        m_MCVictimList.clear();
    }

    void Aggro(Unit* /*who*/)
    {
        if (IsImage || Images75)
            return;

        DoScriptText(SAY_AGGRO1, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKERAM, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKERAM, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->isInCombat())
        {
            if(m_uiRandomYellTimer <= diff)
            {
                m_uiRandomYellTimer = urand(600000, 1800000);

                switch(urand(0,4))
                {
                case 0:
                    DoScriptText(SAY_RANDOM1, m_creature);
                    break;
                case 1:
                    DoScriptText(SAY_RANDOM2, m_creature);
                    break;
                case 2:
                    DoScriptText(SAY_RANDOM3, m_creature);
                    break;
                case 3:
                    DoScriptText(SAY_RANDOM4, m_creature);
                    break;
                case 4:
                    DoScriptText(SAY_RANDOM5, m_creature);
                    break;
                }
            }
            else
                m_uiRandomYellTimer -= diff;
        }

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //ArcaneExplosion_Timer
        if (ArcaneExplosion_Timer < diff)
        {
                uint8 uiTargetInRangeCount = 0;

                GUIDVector vGuids;
                m_creature->FillGuidsListFromThreatList(vGuids);

                for (GUIDVector::const_iterator itr = vGuids.begin(); itr != vGuids.end(); ++itr)
                {
                    Unit* pTarget = m_creature->GetMap()->GetUnit(*itr);

                    if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER && m_creature->CanReachWithMeleeAttack(pTarget))
                        ++uiTargetInRangeCount;
                }

                if (uiTargetInRangeCount > 3)
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANE_EXPLOSION);            
            
            ArcaneExplosion_Timer = urand(8000, 18000);

        }else ArcaneExplosion_Timer -= diff;

        //If we are within range melee the target
        if (m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
        {
            //Make sure our attack is ready and we arn't currently casting
            if (m_creature->isAttackReady() && !m_creature->IsNonMeleeSpellCasted(false))
            {
                m_creature->AttackerStateUpdate(m_creature->getVictim());
                m_creature->resetAttackTimer();
            }
        }else
        {
            //EarthShock_Timer
            if (EarthShock_Timer < diff)
            {
                DoCastSpellIfCan(m_creature->getVictim(),SPELL_EARTH_SHOCK);
                EarthShock_Timer = 1000;
            }else EarthShock_Timer -= diff;
        }

        // MC timer
        if(FullFillment_Timer)
        {
            if(FullFillment_Timer <= diff)
            {
                FullFillment_Timer = 15000;

                Unit *pTarget = GetRandomTargetFromThreatList();

                if(pTarget && pTarget->isAlive())
                {
                    DoCast(pTarget, SPELL_TRUE_FULFILLMENT4, true);
                    m_MCVictimList.push_back(pTarget->GetObjectGuid());
                }
            }
            else
                FullFillment_Timer -= diff;
        }

        //Blink_Timer
        if (Blink_Timer < diff)
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetVisibility(VISIBILITY_OFF);
            DoStopAttack();
            //DoCastSpellIfCan(m_creature, SPELL_BLINK);
            switch(urand(0, 2))
            {
                case 0:
                    m_creature->GetMap()->CreatureRelocation(m_creature, -8340.782227f, 2083.814453f, 125.648788f, 0.0f);
                    DoResetThreat();
                    break;
                case 1:
                    m_creature->GetMap()->CreatureRelocation(m_creature, -8341.546875f, 2118.504639f, 133.058151f, 0.0f);
                    DoResetThreat();
                    break;
                case 2:
                    m_creature->GetMap()->CreatureRelocation(m_creature, -8318.822266f, 2058.231201f, 133.058151f, 0.0f);
                    DoResetThreat();
                    break;
            }
            Invisible = true;

            Blink_Timer = urand(20000, 40000);
        }else Blink_Timer -= diff;

        float procent = m_creature->GetHealthPercent();

        //Summoning 2 Images and teleporting to a random position on 75% health
        if (!Images75 && !IsImage && procent <= 75.0f && procent > 70.0f)
            DoSplit(75);

        //Summoning 2 Images and teleporting to a random position on 50% health
        if (!Images50 && !IsImage && procent <= 50.0f && procent > 45.0f)
            DoSplit(50);

        //Summoning 2 Images and teleporting to a random position on 25% health
        if (!Images25 && !IsImage && procent <= 25.0f && procent > 20.0f)
            DoSplit(25);

        //Invisible_Timer
        if (Invisible)
        {
            if (Invisible_Timer < diff)
            {
                //Making Skeram visible after telporting
                m_creature->SetVisibility(VISIBILITY_ON);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                Invisible_Timer = 500;
                Invisible = false;
            }else Invisible_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }

    void DoSplit(int atPercent /* 75 50 25 */)
    {
        ov_mycoordinates *place1 = new ov_mycoordinates(-8340.782227f, 2083.814453f, 125.648788f, 0.0f);
        ov_mycoordinates *place2 = new ov_mycoordinates(-8341.546875f, 2118.504639f, 133.058151f, 0.0f);
        ov_mycoordinates *place3 = new ov_mycoordinates(-8318.822266f, 2058.231201f, 133.058151f, 0.0f);

        ov_mycoordinates *bossc=place1, *i1=place2, *i2=place3;

        switch(urand(0, 2))
        {
            case 0:
                bossc = place1;
                i1 = place2;
                i2 = place3;
                break;
            case 1:
                bossc = place2;
                i1 = place1;
                i2 = place3;
                break;
            case 2:
                bossc = place3;
                i1 = place1;
                i2 = place2;
                break;
        }

        m_creature->RemoveAllAuras();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetVisibility(VISIBILITY_OFF);

        m_creature->GetMap()->CreatureRelocation(m_creature, bossc->x, bossc->y, bossc->z, bossc->r);

        Invisible = true;
        DoResetThreat();
        DoStopAttack();

        switch (atPercent)
        {
            case 75: Images75 = true; break;
            case 50: Images50 = true; break;
            case 25: Images25 = true; break;
        }

        Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);

        Image1 = m_creature->SummonCreature(15263, i1->x, i1->y, i1->z, i1->r, TEMPSUMMON_CORPSE_DESPAWN, 30000);
        if (Image1)
        {
            Image1->SetMaxHealth(m_creature->GetMaxHealth() / 5);
            Image1->SetHealth(m_creature->GetHealth() / 5);

            if (boss_skeramAI* pImageAI = dynamic_cast<boss_skeramAI*>(Image1->AI()))
                pImageAI->IsImage = true;

            if (target)
                Image1->AI()->AttackStart(target);
        }

        Image2 = m_creature->SummonCreature(15263,i2->x, i2->y, i2->z, i2->r, TEMPSUMMON_CORPSE_DESPAWN, 30000);
        if (Image2)
        {
            Image2->SetMaxHealth(m_creature->GetMaxHealth() / 5);
            Image2->SetHealth(m_creature->GetHealth() / 5);

            if (boss_skeramAI* pImageAI = dynamic_cast<boss_skeramAI*>(Image2->AI()))
                pImageAI->IsImage = true;

            if (target)
                Image2->AI()->AttackStart(target);
        }


        Invisible = true;
        delete place1;
        delete place2;
        delete place3;
    }
};

CreatureAI* GetAI_boss_skeram(Creature* pCreature)
{
    return new boss_skeramAI(pCreature);
}

void AddSC_boss_skeram()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_skeram";
    newscript->GetAI = &GetAI_boss_skeram;
    newscript->RegisterSelf();
}
