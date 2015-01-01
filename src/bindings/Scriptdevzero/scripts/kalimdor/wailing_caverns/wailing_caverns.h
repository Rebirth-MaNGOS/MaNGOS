/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_WAILING_CAVERNS_H
#define DEF_WAILING_CAVERNS_H

enum eTypes
{
    TYPE_LORD_COBRAHN,
    TYPE_LORD_PYTHAS,
    TYPE_LADY_ANACONDRA,
    TYPE_LORD_SERPENTIS,
    TYPE_NARALEX_EVENT,
    TYPE_NARALEX_PART1,
    TYPE_NARALEX_PART2,
    TYPE_NARALEX_PART3,
    TYPE_MUTANUS_THE_DEVOURER,
    TYPE_NARALEX_YELLED,
    MAX_ENCOUNTER
};

enum Creatures
{
    NPC_DEVIATE_FAERIE_DRAGON   = 5912,
    NPC_DISCIPLE_OF_NARALEX     = 3678,
    NPC_NARALEX                 = 3679,
    NPC_DEVIATE_RAVAGER         = 3636,
    NPC_DEVIATE_VIPER           = 5755,
    NPC_DEVIATE_MOCCASIN        = 5762,
    NPC_NIGHTMARE_ECTOPLASM     = 5763,
    NPC_MUTANUS_THE_DEVOURER    = 3654,
};

class MANGOS_DLL_DECL instance_wailing_caverns : public ScriptedInstance
{
    public:
        instance_wailing_caverns(Map* pMap);
        ~instance_wailing_caverns() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);
    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

		bool m_bYelled;
};

#endif
