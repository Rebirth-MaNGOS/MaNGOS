/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#include "precompiled.h"

//battlegrounds
extern void AddSC_battleground();

//custom
extern void AddSC_patrol();

//world
extern void AddSC_areatrigger_scripts();
extern void AddSC_bosses_ancient_leaf_demons();			// hunter epic quest chain
extern void AddSC_bosses_emerald_dragons();
extern void AddSC_boss_lord_kazzak();
extern void AddSC_generic_creature();
extern void AddSC_go_scripts();
extern void AddSC_spell_scripts();
extern void AddSC_guards();
extern void AddSC_item_scripts();
extern void AddSC_npc_building_mount_guard();
extern void AddSC_npc_professions();
extern void AddSC_npcs_special();
extern void AddSC_dungeon_set2();
extern void AddSC_engineering();
extern void AddSC_hallows_end();

// Ahn'Qiraj gate event
extern void AddSC_gate_turnin_scripts();

//eastern kingdoms
extern void AddSC_alterac_valley();                         // alterac valley
extern void AddSC_boss_balinda_stonehearth();
extern void AddSC_boss_drekthar();
extern void AddSC_boss_galvangar();
extern void AddSC_boss_vanndar_stormpike();
extern void AddSC_npc_wing_commanders();
extern void AddSC_blackrock_depths();                       //blackrock_depths
extern void AddSC_boss_ambassador_flamelash();
extern void AddSC_boss_anubshiah();
extern void AddSC_boss_draganthaurissan();
extern void AddSC_boss_general_angerforge();
extern void AddSC_boss_gorosh_the_dervish();
extern void AddSC_boss_grizzle();
extern void AddSC_boss_lord_incendius();
extern void AddSC_boss_high_interrogator_gerstahn();
extern void AddSC_boss_lord_argelmarch();
extern void AddSC_boss_magmus();
extern void AddSC_boss_tomb_of_seven();
extern void AddSC_instance_blackrock_depths();
extern void AddSC_blackrock_spire();                        //blackrock_spire
extern void AddSC_boss_drakkisath();
extern void AddSC_boss_gyth();
extern void AddSC_boss_halycon_gizrul();
extern void AddSC_boss_highlord_omokk();
extern void AddSC_boss_lord_valthalak();
extern void AddSC_boss_mor_grayhoof();
extern void AddSC_boss_mother_smolderweb();
extern void AddSC_boss_overlord_wyrmthalak();
extern void AddSC_boss_pyroguard_emberseer();
extern void AddSC_boss_quatermaster_zigris();
extern void AddSC_boss_rend_blackhand();
extern void AddSC_boss_shadow_hunter_voshgajin();
extern void AddSC_boss_solakar_flamewreath();
extern void AddSC_boss_the_beast();
extern void AddSC_boss_urok_doomhowl();
extern void AddSC_boss_war_master_voone();
extern void AddSC_instance_blackrock_spire();
extern void AddSC_boss_razorgore();                         //blackwing_lair
extern void AddSC_boss_vaelastrasz();
extern void AddSC_boss_broodlord();
extern void AddSC_boss_blackwing_dragon();
extern void AddSC_boss_chromaggus();
extern void AddSC_boss_victor_nefarius();
extern void AddSC_boss_nefarian();
extern void AddSC_blackwing_lair();
extern void AddSC_instance_blackwing_lair();
extern void AddSC_deadmines();                              //deadmines
extern void AddSC_instance_deadmines();
extern void AddSC_gnomeregan();                             //gnomeregan
extern void AddSC_boss_mekgineer_thermaplug();
extern void AddSC_instance_gnomeregan();
extern void AddSC_boss_lucifron();                          //molten_core
extern void AddSC_boss_magmadar();
extern void AddSC_boss_gehennas();
extern void AddSC_boss_garr();
extern void AddSC_boss_baron_geddon();
extern void AddSC_boss_shazzrah();
extern void AddSC_boss_golemagg();
extern void AddSC_boss_sulfuron();
extern void AddSC_boss_majordomo();
extern void AddSC_boss_ragnaros();
extern void AddSC_instance_molten_core();
extern void AddSC_molten_core();
extern void AddSC_boss_anubrekhan();                        //naxxramas
extern void AddSC_boss_four_horsemen();
extern void AddSC_boss_faerlina();
extern void AddSC_boss_gluth();
extern void AddSC_boss_gothik();
extern void AddSC_boss_kelthuzad();
extern void AddSC_boss_loatheb();
extern void AddSC_boss_maexxna();
extern void AddSC_boss_noth();
extern void AddSC_boss_heigan();
extern void AddSC_boss_patchwerk();
extern void AddSC_boss_razuvious();
extern void AddSC_boss_sapphiron();
extern void AddSC_instance_naxxramas();
extern void AddSC_boss_arcanist_doan();                     //scarlet_monastery
extern void AddSC_boss_azshir_the_sleepless();
extern void AddSC_boss_bloodmage_thalnos();
extern void AddSC_boss_herod();
extern void AddSC_boss_high_inquisitor_fairbanks();
extern void AddSC_boss_houndmaster_loksey();
extern void AddSC_boss_interrogator_vishas();
extern void AddSC_boss_mograine_and_whitemane();
extern void AddSC_scarlet_monastery();
extern void AddSC_instance_scarlet_monastery();
extern void AddSC_boss_darkmaster_gandling();               //scholomance
extern void AddSC_boss_death_knight_darkreaver();
extern void AddSC_boss_doctor_theolen_krastinov();
extern void AddSC_boss_lady_illucia_barov();
extern void AddSC_boss_instructor_malicia();
extern void AddSC_boss_jandice_barov();
extern void AddSC_boss_kirtonos_the_herald();
extern void AddSC_boss_kormok();
extern void AddSC_boss_lord_alexei_barov();
extern void AddSC_boss_lorekeeper_polkelt();
extern void AddSC_boss_marduk_blackpool();
extern void AddSC_boss_ras_frostwhisper();
extern void AddSC_boss_rattlegore();
extern void AddSC_boss_the_ravenian();
extern void AddSC_boss_vectus();
extern void AddSC_instance_scholomance();
extern void AddSC_scholomance();
extern void AddSC_shadowfang_keep();                        // Shadowfang_keep
extern void AddSC_instance_shadowfang_keep();
extern void AddSC_boss_magistrate_barthilas();              //stratholme
extern void AddSC_boss_maleki_the_pallid();
extern void AddSC_boss_nerubenkan();
extern void AddSC_boss_cannon_master_willey();
extern void AddSC_boss_baroness_anastari();
extern void AddSC_boss_ramstein_the_gorger();
extern void AddSC_boss_timmy_the_cruel();
extern void AddSC_boss_postmaster_malown();
extern void AddSC_boss_baron_rivendare();
extern void AddSC_boss_jarien_sothos();
extern void AddSC_boss_dathrohan_balnazzar();
extern void AddSC_instance_stratholme();
extern void AddSC_stratholme();
extern void AddSC_boss_atalalarion();                         // Sunken temple
extern void AddSC_boss_atalai_defender();
extern void AddSC_boss_atalai_dragon();
extern void AddSC_boss_avatar_of_hakkar();
extern void AddSC_boss_jammalan_ogom();
extern void AddSC_boss_shade_of_eranikus();
extern void AddSC_instance_sunken_temple();
extern void AddSC_sunken_temple();
extern void AddSC_boss_archaedas();                           //uldaman
extern void AddSC_boss_ironaya();
extern void AddSC_instance_uldaman();
extern void AddSC_uldaman();
extern void AddSC_boss_arlokk();                            //zulgurub
extern void AddSC_boss_gahzranka();
extern void AddSC_boss_grilek();
extern void AddSC_boss_hakkar();
extern void AddSC_boss_hazzarah();
extern void AddSC_boss_jeklik();
extern void AddSC_boss_jindo();
extern void AddSC_boss_mandokir();
extern void AddSC_boss_marli();
extern void AddSC_boss_ouro();
extern void AddSC_boss_renataki();
extern void AddSC_boss_thekal();
extern void AddSC_boss_venoxis();
extern void AddSC_boss_wushoolay();
extern void AddSC_zulgurub();
extern void AddSC_instance_zulgurub();

//extern void AddSC_alterac_mountains();
extern void AddSC_arathi_highlands();
extern void AddSC_badlands();
extern void AddSC_blasted_lands();
extern void AddSC_boss_kruul();
extern void AddSC_burning_steppes();
extern void AddSC_dun_morogh();
extern void AddSC_duskwood();
extern void AddSC_eastern_plaguelands();
extern void AddSC_elwynn_forest();
extern void AddSC_hillsbrad_foothills();
extern void AddSC_hinterlands();
extern void AddSC_ironforge();
extern void AddSC_loch_modan();
extern void AddSC_redridge_mountains();
extern void AddSC_searing_gorge();
extern void AddSC_silverpine_forest();
extern void AddSC_southshore();
extern void AddSC_stormwind_city();
extern void AddSC_stranglethorn_vale();
extern void AddSC_swamp_of_sorrows();
extern void AddSC_tirisfal_glades();
extern void AddSC_undercity();
extern void AddSC_western_plaguelands();
extern void AddSC_westfall();
extern void AddSC_wetlands();

//kalimdor
extern void AddSC_blackfathom_deeps();                      //Blackfathom Depths
extern void AddSC_boss_aku_mai();
extern void AddSC_boss_gelihast();
extern void AddSC_boss_kelris();
extern void AddSC_instance_blackfathom_deeps();
extern void AddSC_boss_alzzin_the_wildshaper();             //Dire Maul
extern void AddSC_boss_captain_kromcrush();
extern void AddSC_boss_guard();
extern void AddSC_boss_hydrospawn();
extern void AddSC_boss_chorush_the_observer();
extern void AddSC_boss_illyanna_ravenoak();
extern void AddSC_boss_immolthar();
extern void AddSC_boss_isalien();
extern void AddSC_boss_king_gordok();
extern void AddSC_boss_lethtendris();
extern void AddSC_boss_magister_kalendris();
extern void AddSC_boss_prince_tortheldrin();
extern void AddSC_boss_pusillin();
extern void AddSC_boss_stomper_kreeg();
extern void AddSC_boss_tendris_warpwood();
extern void AddSC_boss_zevrim_thornhoof();
extern void AddSC_dire_maul();
extern void AddSC_instance_dire_maul();
extern void AddSC_boss_celebras_the_cursed();               //maraudon
extern void AddSC_boss_landslide();
extern void AddSC_boss_noxxion();
extern void AddSC_boss_ptheradras();
extern void AddSC_maraudon();
extern void AddSC_instance_maraudon();
extern void AddSC_boss_onyxia();
extern void AddSC_instance_onyxias_lair();                  //onyxias_lair
extern void AddSC_boss_amnennar_the_coldbringer();          //razorfen_downs
extern void AddSC_razorfen_downs();
extern void AddSC_instance_razorfen_downs();
extern void AddSC_razorfen_kraul();                         //Razorfen Kraul
extern void AddSC_instance_razorfen_kraul();
extern void AddSC_boss_ayamiss();                           //ruins_of_ahnqiraj
extern void AddSC_boss_buru();
extern void AddSC_boss_kurinnaxx();
extern void AddSC_boss_moam();
extern void AddSC_boss_ossirian();
extern void AddSC_boss_rajaxx();
extern void AddSC_ruins_of_ahnqiraj();
extern void AddSC_instance_ruins_of_ahnqiraj();
extern void AddSC_boss_cthun();                             //temple_of_ahnqiraj
extern void AddSC_boss_fankriss();
extern void AddSC_boss_huhuran();
extern void AddSC_bug_trio();
extern void AddSC_boss_viscidus();
extern void AddSC_boss_sartura();
extern void AddSC_boss_skeram();
extern void AddSC_boss_twinemperors();
extern void AddSC_mob_anubisath_sentinel();
extern void AddSC_temple_of_ahnqiraj();
extern void AddSC_instance_temple_of_ahnqiraj();
extern void AddSC_boss_mutanus_the_devourer();              //Wailing caverns
extern void AddSC_wailing_caverns();
extern void AddSC_instance_wailing_caverns();
extern void AddSC_instance_zulfarrak();                     // Zul'Farrak
extern void AddSC_zulfarrak();
extern void AddSC_boss_ukorz_sandscalp();
extern void AddSC_boss_antu_sul();
extern void AddSC_boss_zum_rah();

extern void AddSC_ashenvale();
extern void AddSC_azshara();
extern void AddSC_boss_azuregos();
extern void AddSC_darkshore();
extern void AddSC_desolace();
extern void AddSC_durotar();
extern void AddSC_dustwallow_marsh();
extern void AddSC_felwood();
extern void AddSC_feralas();
extern void AddSC_moonglade();
extern void AddSC_mulgore();
extern void AddSC_orgrimmar();
extern void AddSC_silithus();
extern void AddSC_stonetalon_mountains();
extern void AddSC_tanaris();
extern void AddSC_teldrassil();
extern void AddSC_the_barrens();
extern void AddSC_thousand_needles();
extern void AddSC_thunder_bluff();
extern void AddSC_ungoro_crater();
extern void AddSC_winterspring();


void AddScripts()
{
    //battlegrounds
    AddSC_battleground();

    //custom
	AddSC_patrol();

    //world
    AddSC_areatrigger_scripts();
	AddSC_bosses_ancient_leaf_demons();					// hunter epic quest chain
    AddSC_bosses_emerald_dragons();
    AddSC_boss_lord_kazzak();
    AddSC_generic_creature();
    AddSC_spell_scripts();
    AddSC_go_scripts();
    AddSC_guards();
    AddSC_item_scripts();
    AddSC_npc_building_mount_guard();
    AddSC_npc_professions();
    AddSC_npcs_special();
	AddSC_dungeon_set2();
	AddSC_engineering();
	AddSC_hallows_end();

    // Ahn'Qiraj gate event
    AddSC_gate_turnin_scripts();

    //eastern kingdoms
    AddSC_alterac_valley();                                 //alterac valley
	AddSC_npc_wing_commanders();
    AddSC_boss_balinda_stonehearth();
    AddSC_boss_drekthar();
    AddSC_boss_galvangar();
    AddSC_boss_vanndar_stormpike();
    AddSC_blackrock_depths();                               //blackrock_depths
    AddSC_boss_ambassador_flamelash();
    AddSC_boss_anubshiah();
    AddSC_boss_draganthaurissan();
    AddSC_boss_general_angerforge();
    AddSC_boss_gorosh_the_dervish();
    AddSC_boss_lord_incendius();
    AddSC_boss_grizzle();
    AddSC_boss_high_interrogator_gerstahn();
    AddSC_boss_lord_argelmarch();
    AddSC_boss_magmus();
    AddSC_boss_tomb_of_seven();
    AddSC_instance_blackrock_depths();
    AddSC_blackrock_spire();                                //blackrock_spire
    AddSC_boss_drakkisath();
    AddSC_boss_gyth();
    AddSC_boss_halycon_gizrul();
    AddSC_boss_highlord_omokk();
    AddSC_boss_lord_valthalak();
    AddSC_boss_mor_grayhoof();
    AddSC_boss_mother_smolderweb();
    AddSC_boss_overlord_wyrmthalak();
    AddSC_boss_pyroguard_emberseer();
    AddSC_boss_quatermaster_zigris();
    AddSC_boss_rend_blackhand();
    AddSC_boss_shadow_hunter_voshgajin();
    AddSC_boss_solakar_flamewreath();
    AddSC_boss_the_beast();
    AddSC_boss_urok_doomhowl();
    AddSC_boss_war_master_voone();
    AddSC_instance_blackrock_spire();
    AddSC_boss_razorgore();                                 //blackwing_lair
    AddSC_boss_vaelastrasz();
    AddSC_boss_broodlord();
    AddSC_boss_blackwing_dragon();
    AddSC_boss_chromaggus();
    AddSC_boss_victor_nefarius();
	AddSC_boss_nefarian();
    AddSC_blackwing_lair();
    AddSC_instance_blackwing_lair();
    AddSC_deadmines();                                      //deadmines
    AddSC_instance_deadmines();
    AddSC_gnomeregan();                                     //gnomeregan
    AddSC_boss_mekgineer_thermaplug();
    AddSC_instance_gnomeregan();
    AddSC_boss_lucifron();                                  //molten_core
    AddSC_boss_magmadar();
    AddSC_boss_gehennas();
    AddSC_boss_garr();
    AddSC_boss_baron_geddon();
    AddSC_boss_shazzrah();
    AddSC_boss_golemagg();
    AddSC_boss_sulfuron();
    AddSC_boss_majordomo();
    AddSC_boss_ragnaros();
    AddSC_instance_molten_core();
    AddSC_molten_core();
    AddSC_boss_anubrekhan();                                //naxxramas
    AddSC_boss_faerlina();
    AddSC_boss_four_horsemen();
    AddSC_boss_gluth();
    AddSC_boss_gothik();
    AddSC_boss_heigan();
    AddSC_boss_kelthuzad();
    AddSC_boss_loatheb();
    AddSC_boss_maexxna();
    AddSC_boss_noth();
    AddSC_boss_patchwerk();
    AddSC_boss_razuvious();
    AddSC_boss_sapphiron();
    AddSC_instance_naxxramas();
    AddSC_boss_arcanist_doan();                             //scarlet_monastery
    AddSC_boss_azshir_the_sleepless();
    AddSC_boss_bloodmage_thalnos();
    AddSC_boss_herod();
    AddSC_boss_high_inquisitor_fairbanks();
    AddSC_boss_houndmaster_loksey();
    AddSC_boss_interrogator_vishas();
    AddSC_boss_mograine_and_whitemane();
    AddSC_scarlet_monastery();
    AddSC_instance_scarlet_monastery();
    AddSC_boss_darkmaster_gandling();                       //scholomance
    AddSC_boss_death_knight_darkreaver();
    AddSC_boss_doctor_theolen_krastinov();
    AddSC_boss_lady_illucia_barov();
    AddSC_boss_instructor_malicia();
    AddSC_boss_jandice_barov();
    AddSC_boss_kirtonos_the_herald();
    AddSC_boss_kormok();
    AddSC_boss_lord_alexei_barov();
    AddSC_boss_lorekeeper_polkelt();
    AddSC_boss_marduk_blackpool();
    AddSC_boss_ras_frostwhisper();
    AddSC_boss_rattlegore();
    AddSC_boss_the_ravenian();
    AddSC_boss_vectus();
    AddSC_instance_scholomance();
    AddSC_scholomance();
    AddSC_instance_shadowfang_keep();                       // Shadowfang_keep
    AddSC_shadowfang_keep();
    AddSC_boss_magistrate_barthilas();                      //stratholme
    AddSC_boss_maleki_the_pallid();
    AddSC_boss_nerubenkan();
    AddSC_boss_cannon_master_willey();
    AddSC_boss_baroness_anastari();
    AddSC_boss_ramstein_the_gorger();
    AddSC_boss_timmy_the_cruel();
    AddSC_boss_postmaster_malown();
    AddSC_boss_baron_rivendare();
    AddSC_boss_jarien_sothos();
    AddSC_boss_dathrohan_balnazzar();
    AddSC_instance_stratholme();
    AddSC_stratholme();
    AddSC_boss_atalalarion();                         // Sunken temple
    AddSC_boss_atalai_defender();
    AddSC_boss_atalai_dragon();
    AddSC_boss_avatar_of_hakkar();
    AddSC_boss_jammalan_ogom();
    AddSC_boss_shade_of_eranikus();
    AddSC_instance_sunken_temple();
    AddSC_sunken_temple();
    AddSC_boss_archaedas();                           //uldaman
    AddSC_boss_ironaya();
    AddSC_instance_uldaman();
    AddSC_uldaman();
    AddSC_boss_arlokk();                                    //zulgurub
    AddSC_boss_gahzranka();
    AddSC_boss_grilek();
    AddSC_boss_hakkar();
    AddSC_boss_hazzarah();
    AddSC_boss_jeklik();
    AddSC_boss_jindo();
    AddSC_boss_mandokir();
    AddSC_boss_marli();
    AddSC_boss_ouro();
    AddSC_boss_renataki();
    AddSC_boss_thekal();
    AddSC_boss_venoxis();
    AddSC_boss_wushoolay();
    AddSC_zulgurub();
    AddSC_instance_zulgurub();

    //AddSC_alterac_mountains();
    AddSC_arathi_highlands();
    AddSC_badlands();
    AddSC_blasted_lands();
    AddSC_burning_steppes();
    AddSC_dun_morogh();
	AddSC_duskwood();
    AddSC_eastern_plaguelands();
    AddSC_elwynn_forest();
    AddSC_hillsbrad_foothills();
    AddSC_hinterlands();
    AddSC_ironforge();
    AddSC_loch_modan();
    AddSC_redridge_mountains();
    AddSC_searing_gorge();
    AddSC_silverpine_forest();
	AddSC_southshore();
    AddSC_stormwind_city();
    AddSC_stranglethorn_vale();
    AddSC_swamp_of_sorrows();
    AddSC_tirisfal_glades();
    AddSC_undercity();
    AddSC_western_plaguelands();
    AddSC_westfall();
    AddSC_wetlands();

    //kalimdor
    AddSC_blackfathom_deeps();                              //Blackfathom Depths
    AddSC_boss_aku_mai();
    AddSC_boss_gelihast();
    AddSC_boss_kelris();
    AddSC_instance_blackfathom_deeps();
    AddSC_boss_alzzin_the_wildshaper();                     //Dire Maul
    AddSC_boss_captain_kromcrush();
    AddSC_boss_guard();
    AddSC_boss_hydrospawn();
    AddSC_boss_chorush_the_observer();
    AddSC_boss_illyanna_ravenoak();
    AddSC_boss_immolthar();
    AddSC_boss_isalien();
    AddSC_boss_king_gordok();
    AddSC_boss_lethtendris();
    AddSC_boss_magister_kalendris();
    AddSC_boss_prince_tortheldrin();
    AddSC_boss_pusillin();
    AddSC_boss_stomper_kreeg();
    AddSC_boss_tendris_warpwood();
    AddSC_boss_zevrim_thornhoof();
    AddSC_dire_maul();
    AddSC_instance_dire_maul();
    AddSC_boss_celebras_the_cursed();                       //maraudon
    AddSC_boss_landslide();
    AddSC_boss_noxxion();
    AddSC_boss_ptheradras();
    AddSC_maraudon();
    AddSC_instance_maraudon();
    AddSC_boss_onyxia();
    AddSC_instance_onyxias_lair();                           //onyxias_lair
    AddSC_boss_amnennar_the_coldbringer();                  //razorfen_downs
    AddSC_razorfen_downs();
    AddSC_instance_razorfen_downs();
    AddSC_razorfen_kraul();                                 //Razorfen Kraul
    AddSC_instance_razorfen_kraul();
    AddSC_boss_ayamiss();                                   //ruins_of_ahnqiraj
    AddSC_boss_buru();
    AddSC_boss_kurinnaxx();
    AddSC_boss_moam();
    AddSC_boss_ossirian();
    AddSC_boss_rajaxx();
    AddSC_ruins_of_ahnqiraj();
    AddSC_instance_ruins_of_ahnqiraj();
    AddSC_boss_cthun();                                     //temple_of_ahnqiraj
    AddSC_boss_fankriss();
    AddSC_boss_huhuran();
    AddSC_bug_trio();
	AddSC_boss_viscidus();
    AddSC_boss_sartura();
    AddSC_boss_skeram();
    AddSC_boss_twinemperors();
    AddSC_mob_anubisath_sentinel();
    AddSC_temple_of_ahnqiraj();
    AddSC_instance_temple_of_ahnqiraj();
    AddSC_boss_mutanus_the_devourer();                      //Wailing caverns
    AddSC_wailing_caverns();
    AddSC_instance_wailing_caverns();
    AddSC_instance_zulfarrak();                             //Zul'Farrak
    AddSC_zulfarrak();
	AddSC_boss_ukorz_sandscalp();
	AddSC_boss_antu_sul();
	AddSC_boss_zum_rah();

    AddSC_ashenvale();
    AddSC_azshara();
    AddSC_boss_azuregos();
    AddSC_darkshore();
    AddSC_desolace();
    AddSC_durotar();
    AddSC_dustwallow_marsh();
    AddSC_felwood();
    AddSC_feralas();
    AddSC_moonglade();
    AddSC_mulgore();
    AddSC_orgrimmar();
    AddSC_silithus();
    AddSC_stonetalon_mountains();
    AddSC_tanaris();
    AddSC_teldrassil();
    AddSC_the_barrens();
    AddSC_thousand_needles();
    AddSC_thunder_bluff();
    AddSC_ungoro_crater();
    AddSC_winterspring();

}
