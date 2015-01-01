/* ZUL'FARRAK */
UPDATE `instance_template` SET `ScriptName`='instance_zulfarrak' WHERE `map`=209;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_raven' WHERE `entry`=7605;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_oro_eyegouge' WHERE `entry`=7606;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_murta_grimgut' WHERE `entry`=7608;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='boss_theka_the_martyr' WHERE `entry`=7272;
UPDATE `gameobject_template` SET `ScriptName`='go_troll_cage' WHERE `entry` IN (141070, 141071, 141072, 141073, 141074);
UPDATE `gameobject_template` SET `ScriptName`='go_gong_of_zulfarrak' WHERE `entry`=141832;
UPDATE `gameobject_template` SET `ScriptName`='go_shallow_grave' WHERE `entry` IN (128308,128403);
UPDATE `gameobject_template` SET `ScriptName`='go_table_theka' WHERE `entry`=142715;
DELETE FROM `scripted_areatrigger` WHERE `entry`=962;
INSERT INTO `scripted_areatrigger` (`entry`, `ScriptName`) VALUES('962','at_zumrah');
update creature_template set ScriptName = "boss_antu_sul"
where entry = 8127;
update creature_template set ScriptName = "boss_zum_rah_AND_hydromance_velratha"
where entry = 7271;
update creature_template set ScriptName = "boss_zum_rah_AND_hydromance_velratha"
where entry = 7795;
update creature_template set spell1 = 11087
where entry = 7785;
update creature_template set ScriptName = "boss_ukorz_sandscalp_AND_ruuzlu"
where entry = 7267;
update creature_template set ScriptName = "boss_ukorz_sandscalp_AND_ruuzlu"
where entry = 7797;
update creature_template set ScriptName = "boss_gahz_rilla"
where entry = 7273;

/*Scripts for zul'farrak event*/
update creature_template set ScriptName = 'npc_sergeant_bly' where entry = 7604;
update creature_template set ScriptName = 'npc_raven' where entry = 7605;
update creature_template set ScriptName = 'npc_oro_eyegouge' where entry = 7606;
update creature_template set ScriptName = 'npc_murta_grimgut' where entry = 7608;

/*Adjust delay for spawns at pyramide event*/
update event_scripts set delay = 30 where delay > 0 and id = 2609;

/*Lower walk speed for zul'farrak prisoners*/
update creature_template set speed_walk = 0.8 where entry >= 7604 and entry <= 7608;

delete from creature_template where entry = 6066;
8378 	
INSERT INTO `creature_template` (`entry`, `KillCredit1`, `KillCredit2`, `modelid_1`, `modelid_2`, `name`, `subname`, `gossip_menu_id`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `Civilian`, `RacialLeader`, `RegenHealth`, `equipment_id`, `trainer_id`, `vendor_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(6066, 0, 0, 2418, 0, 'Ward of Zum''rah', NULL, 0, 46, 46, 5, 5, 0, 0, 0, 37, 37, 0, 0.82, 1.14286, 0, 0, 111, 132, 0, 56, 1, 1550, 1705, 0, 0, 0, 0, 0, 0, 0, 0, 62.744, 86.273, 100, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11087, 0, 0, 0, 0, 38, 191, '', 0, 3, 0, 0, 1, 0, 0, 0, 0, 0, '');
