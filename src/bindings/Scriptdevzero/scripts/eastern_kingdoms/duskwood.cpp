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
SDName: Duskwood
SD%Complete:
SDComment: Quest support: 0
SDCategory: Duskwood
EndScriptData */

/* ContentData
EndContentData */

#include "precompiled.h"

/*######
## npc_morgan_ladimore
######*/

enum
{
	SAY_1 = -1720010,
	SAY_2 = -1720011,
};

struct MANGOS_DLL_DECL npc_morgan_ladimoreAI : public ScriptedAI
{
    npc_morgan_ladimoreAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	uint32 m_uiSpeechTimer;
    uint8 m_uiSpeechStep;

    void Reset()
    {
		m_creature->SetStandState(UNIT_STAND_STATE_STAND);
		m_uiSpeechTimer = 0;
        m_uiSpeechStep = 1;
    }


    void UpdateAI(const uint32 uiDiff)					// handle Rp at end of A daughter's love quest, it's a hack but can't do it in DB
    {
		if (!m_uiSpeechStep)
            return;

        if (m_uiSpeechTimer < uiDiff)
        {
            switch(m_uiSpeechStep)
            {
                case 1:
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    m_uiSpeechTimer = 3000;
                    break;
                case 2:
                    DoScriptText(SAY_1, m_creature);
                    m_uiSpeechTimer = 5000;
                    break;
				case 3:
					DoScriptText(SAY_2, m_creature);
                    m_uiSpeechTimer = 3000;
                    break;
				case 4:
					m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
					m_uiSpeechStep = 0;
					return;

                default:
                    m_uiSpeechStep = 0;
                    return;
            }
            ++m_uiSpeechStep;
        }
        else
            m_uiSpeechTimer -= uiDiff;
    }

};

CreatureAI* GetAI_npc_morgan_ladimore(Creature* pCreature)
{
    return new npc_morgan_ladimoreAI(pCreature);
}
void AddSC_duskwood()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_morgan_ladimore";
    pNewscript->GetAI = &GetAI_npc_morgan_ladimore;
    pNewscript->RegisterSelf();
}
