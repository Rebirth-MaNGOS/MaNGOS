/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ZULGURUB_H
#define DEF_ZULGURUB_H

enum Data
{
    TYPE_ARLOKK,
    TYPE_JEKLIK,
    TYPE_VENOXIS,
    TYPE_MARLI,
    TYPE_THEKAL,
    TYPE_ZATH,
    TYPE_LORKHAN,
    TYPE_MANDOKIR,
    TYPE_OHGAN,
    TYPE_HAKKAR,
    MAX_ENCOUNTER,
};

enum Creatures
{
    // Bosses
    NPC_JEKLIK                      = 14517,
    NPC_VENOXIS                     = 14507,
    NPC_MARLI                       = 14510,
    NPC_OHGAN                       = 14988,
    NPC_MANDOKIR                    = 11382,
    NPC_LORKHAN                     = 11347,
    NPC_ZATH                        = 11348,
    NPC_THEKAL                      = 14509,
    NPC_ARLOKK                      = 14515,
    NPC_JINDO                       = 11380,
    NPC_HAKKAR                      = 14834,

    // Trash
    NPC_BLOODSEEKER_BAT             = 11368,
    NPC_FRENZIED_BLOODSEEKER_BAT    = 14965,
    NPC_GURUBASHI_BAT_RIDER         = 14750,
	NPC_SUMMONED_GURUBASHI_BAT_RIDER = 14749,
    NPC_CHAINED_SPIRIT              = 15117,
    NPC_NIGHTMARE_ILLUSION          = 15163,
    NPC_POWERFUL_HEALING_WARD       = 14987,
    NPC_RAZZASHI_COBRA              = 11373,
    NPC_SACRIFICED_TROLL            = 14826,
    NPC_SHADE_OF_JINDO              = 14986,
    NPC_SPAWN_OF_MARLI              = 15041,
    NPC_ZULIAN_GUARDIAN             = 15068,
    NPC_ZULIAN_PROWLER              = 15101,
    NPC_ZULIAN_TIGER                = 11361,
    NPC_POISONOUS_CLOUD             = 14989,
	NPC_WITHERED_MISTRESS			= 14825,
	NPC_VOODOO_SLAVE				= 14883,
	NPC_ATALAI_MISTRESS				= 14882

};

enum GameObjects
{
    GO_GONG_OF_BETHEKK              = 180526,
    GO_FORCE_FIELD                  = 180497,
    GO_SPIDER_EGG                   = 179985,
    GO_LIQUID_FIRE                  = 180125,
};

enum Misc
{
    AT_ZULGURUB_ENTRANCE_1          = 3958,
    AT_ZULGURUB_ENTRANCE_2          = 3956,
    AT_ALTAR_OF_THE_BLOOD_GOD_DOWN  = 3964,
    AT_ALTAR_OF_THE_BLOOD_GOD_UP    = 3960,
    FACTION_ENEMY                   = 14,
    FACTION_FRIENDLY                = 35,
    SAY_HAKKAR_MINION_DESTROY       = -1309024,         // Yell when player steps into ZG instance
    SAY_HAKKAR_PROTECT_ALTAR        = -1309025,         // Yell when player steps on Altar of Blood
};

struct Loc
{
    float x, y, z, o;
};

class MANGOS_DLL_DECL instance_zulgurub : public ScriptedInstance
{
    public:
        instance_zulgurub(Map* pMap);
        ~instance_zulgurub() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature);
        void OnCreatureEnterCombat(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

        void Update(uint32 uiDiff);

        void LowerHakkarHitPoints();
        void HakkarYell(uint32 atEntry);
        void HandleThekalEvent(uint32 uiData);

    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

        bool m_bEnterZulGurubYelled;
        bool m_bProtectAltarYelled;
        bool m_bThekalEvent;

        uint32 m_uiThekalEventTimer;

        GUIDList lJeklikFire;
        GUIDList lChainedSpirits;
};

#endif
