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
#include "blackfathom_deeps.h"

enum Spells
{
    SPELL_POISON_CLOUD                = 3815,
    SPELL_FRENZIED_RAGE               = 3490
};

struct MANGOS_DLL_DECL boss_aku_maiAI : public ScriptedAI
{
    boss_aku_maiAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackfathom_deeps*)pCreature->GetInstanceData();
        Reset();
    }

    bool bIsEnraged;
	uint32 uiPoisonCloudTimer;

    instance_blackfathom_deeps *m_pInstance;

    void Reset()
    {
        bIsEnraged = false;
		uiPoisonCloudTimer = urand(5000,7000);
    }

    void JustReachedHome()
    { 
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AKU_MAI, NOT_STARTED);
    }

    void Aggro(Unit* /*who*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AKU_MAI, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AKU_MAI, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (uiPoisonCloudTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_POISON_CLOUD);
            uiPoisonCloudTimer = urand(6000,8000);
        } else uiPoisonCloudTimer -= uiDiff;

        if (!bIsEnraged && HealthBelowPct(30))
        {
            DoCastSpellIfCan(m_creature,SPELL_FRENZIED_RAGE);
            bIsEnraged = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_aku_mai(Creature* pCreature)
{
    return new boss_aku_maiAI (pCreature);
}

void AddSC_boss_aku_mai()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_aku_mai";
    pNewscript->GetAI = &GetAI_boss_aku_mai;
    pNewscript->RegisterSelf();
}
