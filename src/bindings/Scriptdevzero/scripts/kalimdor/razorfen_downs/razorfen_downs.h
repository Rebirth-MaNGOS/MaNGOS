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

#ifndef DEF_RAZORFEN_DOWNS_H
#define DEF_RAZORFEN_DOWNS_H

enum Data
{
    DATA_GONG_USE,
    TYPE_TUTENKASH,
    TYPE_AMNENNAR
};

enum Creatures
{
    NPC_TUTENKASH               = 7355,
    NPC_TOMB_FIEND              = 7349,
    NPC_TOMB_REAVER             = 7351,
    NPC_FROZEN_SPECTRE          = 8585,
    NPC_RAGGLESNOUT             = 7354
};

enum Misc
{
    GO_GONG                     = 148917,
    MAX_ENCOUNTER               = 2,
    MAX_TOMB_FIEND              = 8,
    MAX_TOMB_REAVER             = 4,
};

class MANGOS_DLL_DECL instance_razorfen_downs : public ScriptedInstance
{
    public:
        instance_razorfen_downs(Map* pMap);
        ~instance_razorfen_downs() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
		void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

		int EventProgress;

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

		void DespawnSpectres();
		void DoEvent(int EventProgress);
    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

		uint32 m_uiGongUse;

		GUIDList m_uiSpectreGUID;
};

#endif
