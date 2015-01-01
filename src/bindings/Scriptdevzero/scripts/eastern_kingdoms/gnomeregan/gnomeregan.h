/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_GNOMEREGAN_H
#define DEF_GNOMEREGAN_H

enum Data
{
    TYPE_EMI,
    TYPE_THERMAPLUG,
    
    TYPE_GNOME_FACE_01,
    TYPE_GNOME_FACE_02,
    TYPE_GNOME_FACE_03,
    TYPE_GNOME_FACE_04,
    TYPE_GNOME_FACE_05,
    TYPE_GNOME_FACE_06,
};

enum Data64
{
    DATA_THERMAPLUG,
    DATA_KERNOBEE,

    DATA_GO_CAVE_IN_1,
    DATA_GO_CAVE_IN_2,
};

enum Creatures
{
    NPC_MEKGINEER_THERMAPLUG = 7800,
    NPC_WALKING_BOMB         = 7915,
    NPC_KERNOBEE             = 7850,
};

enum GameObjects
{
    GO_CAVE_IN_1             = 146086,
    GO_CAVE_IN_2             = 146085,
    GO_WEEGLIS_BARREL        = 141612,

    GO_THE_FINAL_CHAMBER     = 142207,

    GO_BUTTON_01             = 142214,
    GO_BUTTON_02             = 142215,
    GO_BUTTON_03             = 142216,
    GO_BUTTON_04             = 142217,
    GO_BUTTON_05             = 142218,
    GO_BUTTON_06             = 142219,

    GO_GNOME_FACE_01         = 142211,
    GO_GNOME_FACE_02         = 142210,
    GO_GNOME_FACE_03         = 142209,
    GO_GNOME_FACE_04         = 142208,
    GO_GNOME_FACE_05         = 142213,
    GO_GNOME_FACE_06         = 142212,
};

enum Misc
{
    MAX_ENCOUNTER = 2,

    // Mekgineer Thermaplug yells
    YELL_AGGRO                = -1090000,
    YELL_COMBAT               = -1090001,
    YELL_BOMB_SUMMON          = -1090002,
    YELL_KILL                 = -1090003,
};

static float SpawnLocations[6][3]=
{
    {-551.55f, 697.81f, -327.25f},
    {-522.39f, 702.70f, -327.23f},
    {-500.29f, 683.80f, -327.23f},
    {-500.92f, 654.92f, -327.24f},
    {-524.92f, 637.73f, -327.22f},
    {-552.42f, 644.81f, -327.22f}
};

class MANGOS_DLL_DECL instance_gnomeregan : public ScriptedInstance
{
    public:
        instance_gnomeregan(Map* pMap);
        ~instance_gnomeregan() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
		void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

		void Update(uint32 uiDiff);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

		void ActivateGnomeFace(uint8 no_gnome_face, bool activate = true);
		void ResetWalkingBombs(GUIDList m_uiList);
    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

		uint32 m_uiSummonWalkingBombTimer;
		uint32 m_uiActivateNewGnomeFaceTimer;

		ObjectGuid m_uiGnomeFaceGUID[6];

		GUIDList lExplosiveBarrels;
		GUIDList lGnomeFace1BombsGUID;
		GUIDList lGnomeFace2BombsGUID;
		GUIDList lGnomeFace3BombsGUID;
		GUIDList lGnomeFace4BombsGUID;
		GUIDList lGnomeFace5BombsGUID;
		GUIDList lGnomeFace6BombsGUID;
};

#endif
