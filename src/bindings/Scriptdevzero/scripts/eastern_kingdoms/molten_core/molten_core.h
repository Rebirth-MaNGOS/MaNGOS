/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_MOLTEN_CORE_H
#define DEF_MOLTEN_CORE_H

enum Data
{
    TYPE_LUCIFRON,
    TYPE_MAGMADAR,
    TYPE_GEHENNAS,
    TYPE_GARR,
    TYPE_SHAZZRAH,
    TYPE_GEDDON,
    TYPE_SULFURON,
    TYPE_GOLEMAGG,
    TYPE_MAJORDOMO,
    TYPE_RAGNAROS,
    TYPE_MAJORDOMO_FIRST_SPAWN,  // please, use only these values: NOT_STARTED, DONE

    MAX_ENCOUNTER
};

enum Creatures
{
    // Bosses
    NPC_LUCIFRON                = 12118,
    NPC_MAGMADAR                = 11982,
    NPC_GEHENNAS                = 12259,
    NPC_GARR                    = 12057,
    NPC_GEDDON                  = 12056,
    NPC_SHAZZRAH                = 12264,
    NPC_GOLEMAGG                = 11988,
    NPC_SULFURON                = 12098,
    NPC_MAJORDOMO               = 12018,
    NPC_RAGNAROS                = 11502,

    // Adds
    NPC_FIRESWORN               = 12099,
    NPC_FLAMEWAKER              = 11661,
    NPC_FLAMEWAKER_PROTECTOR    = 12119,
    NPC_FLAMEWAKER_PRIEST       = 11662,
    NPC_FLAMEWAKER_HEALER       = 11663,
    NPC_FLAMEWAKER_ELITE        = 11664,
    NPC_CORE_HOUND              = 11671,
    NPC_CORE_RAGER              = 11672,

    // Trash
    NPC_ANCIENT_CORE_HOUND      = 11673,
    NPC_LAVA_SURGER             = 12101,
    NPC_SON_OF_FLAME            = 12143,
};

enum GameObjects
{
    GO_RUNE_OF_KORO             = 176951,
    GO_RUNE_OF_ZETH             = 176952,
    GO_RUNE_OF_MAZJ             = 176953,
    GO_RUNE_OF_THERI            = 176954,
    GO_RUNE_OF_BLAZ             = 176955,
    GO_RUNE_OF_KRESS            = 176956,
    GO_RUNE_OF_MOHN             = 176957,

    GO_HOT_COALS                = 177000,
    GO_FIRELORD_CACHE           = 179703,
    GO_LAVA_BOMB                = 177704,
};

enum YellsToZone
{
    // Majordomo Executus
    SAY_DOMO_SPAWN              = -1409004,
};

enum Misc
{
    FACTION_FRIENDLY            = 35,
	FACTION_ENEMY				= 54,
};

class MANGOS_DLL_DECL instance_molten_core : public ScriptedInstance
{
    public:
        instance_molten_core(Map* pMap);
        ~instance_molten_core() {}

        void Initialize();

		bool IsEncounterInProgress() const;

        void OnPlayerEnter(Player* pPlayer);
        void OnPlayerLeave(Player* pPlayer);

        void OnCreatureCreate(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

        void Update(uint32 uiDiff);

		void HandleRune(uint32 uiEntry, uint32 uiData);
		void HandleRune(GameObject *obj, uint32 uiData);
		bool CanSpawnDomoOrRagnaros(bool spawn_majordomo);
		void SetMoltenCoreCreatureVisibility(Creature* pCreature, ObjectGuid pCreatureGUID, bool set_visible, bool m_bKillAndRespawn = false);
		void DoSpawnMajordomoIfCan(bool m_bKillAndRespawn = true, bool force = false);
		void RespawnBossAdds(GUIDList list);
		void StopSpawningAllTrashMobs(uint32 type);

		GUIDList const& GetSulfuronAdds() {return lFlamewakerPriest; };
    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
		std::multimap<uint32, ObjectGuid> m_trashGuids;
		std::string strInstData;

        uint32 m_uiDomoAddsDeadCount;
        uint32 m_uiInitiateAddsTimer;
        uint32 m_uiRagnarosDespawnTimer;
        uint32 m_uiMajordomoRespawnTimer;

        GUIDList lFiresworn;
        GUIDList lFlamewaker;
        GUIDList lFlamewakerProtector;
        GUIDList lFlamewakerPriest;
        GUIDList lFlamewakerHealerElite;
        GUIDList lLavaBomb;
        GUIDList lCoreRager;
};

#endif
