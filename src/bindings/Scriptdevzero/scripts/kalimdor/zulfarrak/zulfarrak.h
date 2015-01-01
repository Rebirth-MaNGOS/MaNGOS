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

#ifndef DEF_ZULFARRAK_H
#define DEF_ZULFARRAK_H

enum Type
{
    TYPE_GAHZRILLA,
    TYPE_PYRAMIDE,
    TYPE_WEEGLI
};

enum Data
{
    DATA_WEEGLI,
    DATA_BLY,
    DATA_RAVEN,
    DATA_ORO,
    DATA_MURTA
};

enum Creatures
{
    NPC_DUSTWRAITH              = 10081,
    NPC_GAHZRILLA               = 7273,
    NPC_SANDARR_DUNEREAVER      = 10080,
    NPC_ZERILIS                 = 10082,
	NPC_SANDFURY_EXECUTIONER    = 7274,
    NPC_GOBLIN_LAND_MINE        = 7527,
	NPC_SNAKE					= 7268,

    // Pyramide prisoners
    NPC_WEEGLI_BLASTFUSE        = 7607,
    NPC_SERGEANT_BLY            = 7604,
    NPC_RAVEN                   = 7605,
    NPC_ORO_EYEGOUGE            = 7606,
    NPC_MURTA_GRIMGUT           = 7608,
    
    // Pyramide trash
    NPC_SANDFURY_SLAVE          = 7787,
    NPC_SANDFURY_DRUDGE         = 7788,
    NPC_SANDFURY_CRETIN         = 7789,
    NPC_SANDFURY_ACOLYTE        = 8876,
    NPC_SANDFURY_ZEALOT         = 8877,
    NPC_SANDFURY_SOUL_EATER     = 7247,
    NPC_SHADOWPRIEST_SEZZZIZZ   = 7275,
    NPC_NEKRUM_GUTCHEWER        = 7796
};

enum GameObjects
{
    GO_GONG_OF_ZULFARRAK        = 141832,
    GO_TROLL_CAGE1              = 141070,
    GO_TROLL_CAGE2              = 141071,
    GO_TROLL_CAGE3              = 141072,
    GO_TROLL_CAGE4              = 141073,
    GO_TROLL_CAGE5              = 141074,
    GO_END_DOOR                 = 146084,
    GO_DOOR_EXPLOSIVE           = 141612,
};

enum Misc 
{
    ITEM_MALLET_OF_ZULFARRAK    = 9240,
    FACTION_HOSTILE             = 54,
    FACTION_FRIENDLY            = 35,
    FACTION_FFP_EFE             = 250,
    MAX_ENCOUNTER               = 3
};

enum Spells
{
    SPELL_BLYS_BANDS_ESCAPE     = 11365         // Bly's Band's Heartstone
};

class MANGOS_DLL_DECL instance_zulfarrak : public ScriptedInstance
{
    public:
        instance_zulfarrak(Map* pMap);
        ~instance_zulfarrak() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
		void OnObjectCreate(GameObject* pGo);

		void OnPlayerDeath(Player * pPlayer);
		void OnCreatureDeath(Creature * pCreature);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

		void Update(uint32 uiDiff);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

		GUIDList m_uiSnakes; 
    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

		uint32 m_uiCheckPyramideTrash_Timer;
		uint32 m_uiBombExplodationExpire_Timer;
		uint32 m_uiBlysBandHeartstone_Timer;

		uint32 m_uiTrollsMovedCount;
		uint32 m_uiWave;

		GUIDList m_uiPyramideTrash; //All npcs of the event
		GUIDList m_uiPyramideTrashTemp; //All npcs that are currently sent out to fight
		GUIDList m_uiPyramideNPCs[5]; //One list for all npcs of the same type
};

#endif
