/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SCHOLOMANCE_H
#define DEF_SCHOLOMANCE_H

enum Data
{
    TYPE_KIRTONOS_THE_HERALD,
    TYPE_JANDICE_BAROV,
    TYPE_GREAT_OSSUARY,
    TYPE_RAS_FROSTWHISPER,
    TYPE_KORMOK,
    TYPE_LOREKEEPER_POLKELT,
    TYPE_DOCTOR_THEOLEN_KRASTINOV,
    TYPE_INSTRUCTOR_MALICIA,
    TYPE_LADY_ILLUCIA_BAROV,
    TYPE_LORD_ALEXEI_BAROV,
    TYPE_THE_RAVENIAN,
    TYPE_DARKMASTER_GANDLING,
    TYPE_CHAMBER_OPEN,
    TYPE_CHAMBER_CLOSE,
    TYPE_VIEWING_ROOM_DOOR,
	MAX_ENCOUNTER
};

enum Creatures
{
    NPC_BLOOD_STEWARD_OF_KIRTONOS   = 14861,
    NPC_KIRTONOS_THE_HERALD         = 10506,
    NPC_ILLUSION_OF_JANDICE_BAROV   = 11439,
    NPC_RISEN_CONSTRUCT             = 10488,
    NPC_ASPECT_OF_BANALITY          = 14518,
    NPC_BANAL_SPIRIT                = 14514,
    NPC_ASPECT_OF_MALICE            = 14520,
    NPC_MALICIOUS_SPIRIT            = 14513,
    NPC_ASPECT_OF_CORRUPTION        = 14519,
    NPC_CORRUPTED_SPIRIT            = 14512,
    NPC_ASPECT_OF_SHADOW            = 14521,
    NPC_SHADOWED_SPIRIT             = 14511,
    NPC_DEATH_KNIGHT_DARKREAVER     = 14516,
    NPC_DARKREAVERS_FALLEN_CHARGER  = 14568,
    NPC_BONE_MINION                 = 16119,
    NPC_BONE_MAGE                   = 16120,
    NPC_SCHOLOMANCE_STUDENT         = 10475,
    NPC_MARDUK_BLACKPOOL            = 10433,
    NPC_VECTUS                      = 10432,
	NPC_VECTUS_PASSIVE				= 800016,
    NPC_KORMOK                      = 16118,
    NPC_RISEN_ABERRATION            = 10485,
	NPC_RISEN_GUARDIAN				= 11598,
    NPC_DARKMASTER_GANDLING         = 1853,
	NPC_PALADIN_EVENT				= 800469,
};

enum GameObjects
{
    GO_BRAZIER_OF_BECKONING         = 181045,
    GO_BRAZIER_OF_INVOCATION        = 181051,
    GO_BRAZIER_OF_THE_HERALD        = 175564,
    GO_GATE_KIRTONOS                = 175570,
    GO_JOURNAL_OF_JANDICE_BAROV     = 180794,
    GO_GATE_GREAT_OSSUARY           = 179724,
    GO_GREAT_OSSUARY                = 300144,
    GO_GATE_RAS_FROSTWHISPER        = 177370,
    GO_GATE_ILLUCIA                 = 177371,
    GO_GATE_RAVENIAN                = 177372,
    GO_GATE_ALEXEI                  = 177373,
    GO_GATE_GANDLING                = 177374,
    GO_GATE_MALICIA                 = 177375,
    GO_GATE_POLKELT                 = 177376,
    GO_GATE_THEOLEN                 = 177377,
    GO_VIEWING_ROOM_DOOR            = 175167,
    GO_DAWNS_GAMBIT_1               = 176110,
    GO_DAWNS_GAMBIT_2               = 177304,
};

enum Misc
{
    FACTION_FRIENDLY                = 35,
    FACTION_HOSTILE                 = 14,
    ITEM_BLOOD_OF_INNOCENTS         = 13523,
    SAY_VECTUS_DAWN_GAMBIT          = -1289005,
};

struct Loc
{
	uint32 gate;
    float x, y, z;
};

class MANGOS_DLL_DECL instance_scholomance : public ScriptedInstance
{
    public:
        instance_scholomance(Map* pMap);
        ~instance_scholomance() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);
		void OnCreatureEnterCombat(Creature * pCreature);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

        void Update(uint32 uiDiff);

		void HandleGate(uint32 uiEntry, uint32 uiData, bool bTrySummon);
		void OssuaryStartEvent();
		void TrySummonGandling(GameObject* pGo);
    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

		uint32 m_uiWaveCount;
		uint32 m_uiViewingRoomTimer;
		bool m_bPaladin;
};

#endif
