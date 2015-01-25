-- Update Thunderfury with the correct procrate.
UPDATE zp_mangosd.item_template SET item_template.spellppmRate_1 = 7 WHERE item_template.entry = 19019;


    /*************** ESCAPE THROUGH FORCE (REQUIRES RECOMPILATION OF SERVER)************************/

    /** Remove old DB script for the quest Escape through force. **/

    UPDATE `zp_mangosd`.`quest_template` SET `StartScript`=0 WHERE  `entry`=994 LIMIT 1;

    /** Add new script for the quest Escape through Force. **/

    UPDATE `zp_mangosd`.`creature_template` SET `ScriptName`='npc_volcor' WHERE  `entry`=3692 LIMIT 1;

    /** Waypoints for the quest Escape through force. **/

    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 1, 4603.96, -1.05598, 69.4241, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 2, 4609.73, 9.10512, 69.6864, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 3, 4616.13, 21.5553, 70.8523, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 4, 4622.7, 33.9179, 69.5292, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 5, 4629.34, 46.241, 68.321, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 6, 4635.54, 61.7684, 65.7255, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 7, 4642.01, 81.7467, 61.1461, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 8, 4650.08, 96.786, 59.6032, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 9, 4662.89, 111.15, 58.5598, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 10, 4675.21, 123.578, 58.4745, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 11, 4687.58, 135.957, 55.695, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 12, 4702.11, 151.113, 52.6661, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 13, 4716.87, 166.045, 53.045, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 14, 4726.81, 175.909, 53.9219, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 15, 4739.59, 187.848, 54.4278, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 16, 4752.77, 199.37, 54.3507, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 17, 4765.97, 208.221, 51.7664, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 18, 4774.69, 217.74, 49.0849, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (3692, 19, 4787.83, 220.133, 48.8246, 0, NULL);

    /** Text for the quest Escape through force. **/

    REPLACE INTO `zp_scriptdevzerod`.`script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES (-1713002, 'Oh, hello Grimclaw.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, NULL);
    REPLACE INTO `zp_scriptdevzerod`.`script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES (-1713001, 'Ok, $N. Now i can go the the Auberdine alone. Go to Terenthis. He waits for you. Thanks for the help.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 'Volcor escort - Escape Through Force');


    /**************** ESCAPE THROUGH STEALTH (CAN BE RUN WITHOUT RECOMPILING SERVER) ********************/

    /** Remove old DB script for the quest Escape through stealth. **/

    DELETE FROM zp_mangosd.quest_start_scripts WHERE id = 995;

    /** DB script for the quest Escape through stealth. **/

    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 0, 28, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, ' ');
    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 0, 0, 0, 0, 0, 0, 0, 2000005666, 0, 0, 0, 0, 0, 0, 0, ' ');
    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 0, 22, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ' ');
    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ' ');
    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 0, 25, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ' ');
    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 4, 23, 11449, 0, 10, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, ' ');
    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 5, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4604, -1.3, 69.4, 0, ' ');
    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 6, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4622.6, 30, 69.8, 0, ' ');
    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 10, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4639.3, 32.8, 68.5, 0, ' ');
    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 10, 22, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ' ');
    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 10, 7, 995, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ' ');
    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 14, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ' ');
    REPLACE INTO `zp_mangosd`.`quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES (995, 14, 23, 1899, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, ' ');

    /** New text for the quest. **/

    REPLACE INTO `zp_mangosd`.`db_script_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES (2000005666, 'Thank you again, $N. I\'ll make my way to the road now. When you can, find Terenthis and let him know we escaped.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);


-- Script for Grimclaw in Darkshore.

    REPLACE INTO `zp_mangosd`.`creature_template` (`entry`, `KillCredit1`, `KillCredit2`, `modelid_1`, `modelid_2`, `name`, `subname`, `gossip_menu_id`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `aggrorangeoverride`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `Civilian`, `RacialLeader`, `RegenHealth`, `equipment_id`, `trainer_id`, `vendor_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES (3695, 0, 0, 913, 0, 'Grimclaw', NULL, 0, 13, 13, 546, 546, 0, 0, 608, 80, 80, 0, 0.94, 1.11286, 0, 0, 19, 23, 0, 10, 1, 2000, 0, 0, 1, 4096, 0, 0, 0, 0, 0, 0, 21.5072, 29.5724, 100, 1, 0, 3695, 0, 100007, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 1, 3, 1, 0, 1, 0, 0, 0, 0, 2, 'npc_grimclaw');

-- SQL for the Heart of Hakkar event in Stranglethorn Vale.
-- Spell target for the channeling spell.
insert into zp_mangosd.spell_script_target (`entry`, `type`, `targetEntry`) VALUES ('24217', '1', '15073');
-- AI for the spawning guy.
update zp_mangosd.creature_template set ScriptName = "npc_pats_hellfire_guy" where entry = 15073;
-- Target coordinates for Molthor's spell.
REPLACE INTO zp_mangosd.spell_target_position (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES ('24214', '0', '-11818.61', '1344.06', '13.5', '0');
-- AI script for Molthor.
UPDATE zp_mangosd.creature_template SET ScriptName = "npc_molthor" WHERE entry = 14875;
-- Scale for Pat's Hellfire Guy
UPDATE zp_mangosd.creature_template SET scale = '3' WHERE entry = 15073;
-- Lower the speeed of Molthor slightly.
UPDATE zp_mangosd.creature_template SET speed_run = 1.05 WHERE entry = 14875;
-- Sizes for the objects
UPDATE `zp_mangosd`.`gameobject_template` SET `size`='2' WHERE `entry`='180249';
UPDATE `zp_mangosd`.`gameobject_template` SET `size`='2' WHERE `entry`='180250';
UPDATE `zp_mangosd`.`gameobject_template` SET `size`='3' WHERE `entry`='180402';
-- Delete the old DB script.
DELETE FROM zp_mangosd.quest_end_scripts WHERE id = 8183 LIMIT 1;
-- Change the size for the summoning circle swirly animation.
UPDATE `zp_mangosd`.`gameobject_template` SET `size`='2' WHERE `entry`='180404';
-- Talk for Molothar
INSERT INTO `zp_scriptdevzerod`.`script_texts` (`entry`, `content_default`) VALUES ('-1720001', 'Now, only one step remains to rid us of the Soulflayer\'s threat...');
INSERT INTO `zp_scriptdevzerod`.`script_texts` (`entry`, `content_default`) VALUES ('-1720002', 'Begin the ritual, my servants. We must banish the heart of Hakkar back into the void!');
UPDATE `zp_scriptdevzerod`.`script_texts` SET `type`='1' WHERE `entry`='-1720002';

	-- Script for Rabid Thistle Bear.
	
	REPLACE INTO `zp_mangosd`.`creature_template` (`entry`, `KillCredit1`, `KillCredit2`, `modelid_1`, `modelid_2`, `name`, `subname`, `gossip_menu_id`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `aggrorangeoverride`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `Civilian`, `RacialLeader`, `RegenHealth`, `equipment_id`, `trainer_id`, `vendor_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES (2164, 0, 0, 8840, 0, 'Rabid Thistle Bear', NULL, 0, 13, 14, 341, 491, 0, 0, 270, 44, 44, 0, 0.94, 1.11286, 0, 0, 19, 25, 0, 10, 1, 2000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 22.792, 31.339, 100, 1, 0, 2164, 0, 2164, 0, 0, 0, 0, 0, 0, 3150, 0, 0, 0, 0, 0, 0, 'EventAI', 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 'mob_rabid_thistle_bear');

-- Adjustment for whelp spawn times in BWL.
UPDATE zp_mangosd.creature SET spawntimesecs = FLOOR(30 + RAND() * (121 - 30)) WHERE id IN ('14022', '14023', '14024', '14025');

-- Immunities for Flamewaker Protector.
UPDATE `zp_mangosd`.`creature_template` SET `mechanic_immune_mask`=617299803 WHERE  `entry`=12119 LIMIT 1;

/******************** Update size + interact state for pylons in DM West ***********************************/

UPDATE `zp_mangosd`.`gameobject_template` SET `faction`=114, `flags`=1, `size`=6.527, `ScriptName`='' WHERE `entry`=177259;
UPDATE `zp_mangosd`.`gameobject_template` SET `faction`=114, `flags`=1, `size`=6.527, `ScriptName`='' WHERE `entry`=177257;
UPDATE `zp_mangosd`.`gameobject_template` SET `faction`=114, `flags`=1, `size`=6.527, `ScriptName`='' WHERE `entry`=177258;
UPDATE `zp_mangosd`.`gameobject_template` SET `faction`=114, `flags`=1, `size`=6.527, `ScriptName`='' WHERE `entry`=179504;
UPDATE `zp_mangosd`.`gameobject_template` SET `faction`=114, `flags`=1, `size`=6.527, `ScriptName`='' WHERE `entry`=179505;
UPDATE `zp_mangosd`.`gameobject_template` SET `faction`=114, `flags`=32 WHERE  `entry`=179506;

/********************* Pyrewood village event ***************************/

-- Add events to events list

INSERT INTO `zp_mangosd`.`game_event` VALUES (50, '2006-02-08 06:00:00', '2020-12-31 20:59:59', 1440, 960, 0, 'Pyrewood Village - Day');
INSERT INTO `zp_mangosd`.`game_event` VALUES (51, '2006-02-08 21:00:00', '2020-12-31 05:59:59', 1440, 480, 0, 'Pyrewood Village - Worgen Curse');

-- Add all Worgen

INSERT INTO `zp_mangosd`.`creature` VALUES (3008870, 1892, 0, 0, 0, -354.096, 1421.48, 28.1202, 5.37542, 25, 5, 0, 842, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008871, 1892, 0, 0, 0, -350.452, 1448.51, 24.2547, 4.63715, 25, 5, 0, 897, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008872, 1893, 0, 0, 0, -380.696, 1435.16, 25.684, 6.27863, 25, 5, 0, 979, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008873, 1896, 0, 0, 0, -392.203, 1432.1, 26.8478, 6.1137, 25, 5, 0, 823, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008874, 1893, 0, 0, 0, -390.635, 1466.56, 26.0207, 5.30082, 25, 5, 0, 1242, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008875, 1892, 0, 0, 0, -404.243, 1458.93, 26.9134, 0.00330591, 25, 5, 0, 897, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008876, 1896, 0, 0, 0, -408.024, 1451.09, 26.9036, 0.835828, 25, 5, 0, 889, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008877, 1893, 0, 0, 0, -404.465, 1460.8, 31.6819, 5.84274, 25, 5, 0, 979, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008878, 1893, 0, 0, 0, -413.611, 1456.66, 31.6789, 0.0190139, 25, 5, 0, 1242, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008879, 1892, 0, 0, 0, -377.734, 1491.9, 18.3213, 5.26155, 25, 5, 0, 897, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008880, 3531, 0, 0, 0, -361.536, 1500.37, 17.2484, 4.48794, 25, 5, 0, 647, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008881, 3529, 0, 0, 0, -346.413, 1500.84, 18.8082, 0.875111, 25, 5, 0, 981, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008882, 1896, 0, 0, 0, -343.165, 1505.29, 18.7815, 4.001, 25, 5, 0, 889, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008883, 1896, 0, 0, 0, -395.136, 1545.23, 16.8954, 3.04674, 25, 5, 0, 889, 0, 0, 1);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008884, 1896, 0, 0, 0, -423.653, 1543.42, 18.4704, 6.1687, 25, 5, 0, 889, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008885, 1892, 0, 0, 0, -429.356, 1533.42, 18.5726, 0.674839, 25, 5, 0, 842, 0, 0, 1);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008886, 1893, 0, 0, 0, -444.301, 1540.62, 19.6352, 6.27473, 25, 5, 0, 979, 0, 0, 1);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008887, 1892, 0, 0, 0, -434.829, 1562.94, 19.9626, 5.22622, 25, 5, 0, 897, 0, 0, 1);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008888, 1892, 0, 0, 0, -436.403, 1518.17, 19.5468, 0.800507, 25, 5, 0, 842, 0, 0, 1);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008889, 1896, 0, 0, 0, -411.785, 1518.87, 19.7699, 4.73535, 25, 5, 0, 889, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008890, 1893, 0, 0, 0, -411.989, 1508.83, 19.7702, 1.61339, 25, 5, 0, 1242, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008891, 1893, 0, 0, 0, -384.272, 1563.06, 17.1922, 4.28375, 25, 5, 0, 1242, 0, 0, 1);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008892, 1893, 0, 0, 0, -357.268, 1580.44, 18.0479, 0.0229683, 25, 5, 0, 979, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008893, 1892, 0, 0, 0, -344.681, 1601.51, 16.9805, 2.09643, 25, 5, 0, 842, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008894, 3533, 0, 0, 0, -407.482, 1602.53, 18.1017, 5.59146, 25, 5, 0, 836, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008895, 1893, 0, 0, 0, -391.615, 1602.18, 16.9907, 4.94744, 25, 5, 0, 1242, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008896, 1896, 0, 0, 0, -379.114, 1623.75, 16.8078, 1.73515, 25, 5, 0, 889, 0, 0, 1);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008897, 1893, 0, 0, 0, -387.662, 1634.56, 17.4309, 6.02735, 25, 5, 0, 1242, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008898, 1892, 0, 0, 0, -381.525, 1650.85, 17.4179, 4.61757, 25, 5, 0, 842, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008899, 1893, 0, 0, 0, -381.366, 1656.47, 10.2629, 2.79545, 25, 5, 0, 1242, 0, 0, 1);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008900, 1893, 0, 0, 0, -372.209, 1611.23, 24.2887, 1.62913, 25, 5, 0, 979, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008901, 1893, 0, 0, 0, -384.223, 1618.96, 24.2887, 4.93173, 25, 5, 0, 1242, 0, 0, 1);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008902, 1893, 0, 0, 0, -384.372, 1633.35, 24.2887, 3.20778, 25, 5, 0, 979, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008903, 1892, 0, 0, 0, -363.295, 1545.28, 24.398, 3.21956, 25, 5, 0, 842, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008904, 1893, 0, 0, 0, -341.577, 1553.61, 24.3428, 3.22348, 25, 5, 0, 1242, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008905, 1892, 0, 0, 0, -358.1, 1527.82, 24.9345, 1.34638, 25, 5, 0, 897, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008906, 1893, 0, 0, 0, -333.162, 1548.77, 25.2865, 4.53902, 25, 5, 0, 1242, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008907, 1896, 0, 0, 0, -325.607, 1550.69, 25.2832, 1.34245, 25, 5, 0, 889, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008908, 1896, 0, 0, 0, -329.513, 1545.99, 30.053, 2.22995, 25, 5, 0, 889, 0, 0, 1);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008909, 1893, 0, 0, 0, -337.335, 1548.11, 30.0304, 1.13432, 25, 5, 0, 979, 0, 0, 1);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008910, 1892, 0, 0, 0, -390.462, 1571.06, 17.002, 0.0190455, 25, 5, 0, 897, 0, 0, 2);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008911, 1893, 0, 0, 0, -400.889, 1550.64, 16.7986, 0.372473, 25, 5, 0, 1242, 0, 0, 2);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008912, 1892, 0, 0, 0, -349.412, 1609.79, 16.9815, 5.22624, 25, 5, 0, 842, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008913, 1892, 0, 0, 0, -417.046, 1576.94, 19.5426, 3.39626, 25, 5, 0, 897, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008914, 1896, 0, 0, 0, -411.877, 1579.63, 19.5426, 5.62286, 25, 5, 0, 823, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008915, 1896, 0, 0, 0, -406.599, 1583.59, 24.3014, 3.71042, 25, 5, 0, 823, 0, 0, 0);
INSERT INTO `zp_mangosd`.`creature` VALUES (3008916, 1896, 0, 0, 0, -421.6, 1580.97, 24.3086, 3.48265, 25, 5, 0, 889, 0, 0, 0);

-- Add the Humans to Day event

INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (17663, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (17905, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (17908, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (17910, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (17911, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18220, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18221, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18235, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18236, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18237, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18238, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18239, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18291, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18306, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18307, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18309, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18310, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18350, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18354, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18355, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18400, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18404, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18405, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18407, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18408, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18409, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18411, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18413, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (18996, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19002, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19005, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19007, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19008, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19012, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19015, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19019, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19084, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19219, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19220, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19222, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19223, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19224, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19227, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19228, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19229, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19316, 50);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (19343, 50);

-- Add Worgen to Night event

INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008870, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008871, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008872, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008873, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008874, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008875, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008876, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008877, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008878, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008879, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008880, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008881, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008882, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008883, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008884, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008885, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008886, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008887, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008888, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008889, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008890, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008891, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008892, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008893, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008894, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008895, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008896, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008897, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008898, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008899, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008900, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008901, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008902, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008903, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008904, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008905, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008906, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008907, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008908, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008909, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008910, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008911, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008912, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008913, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008914, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008915, 51);
INSERT INTO `zp_mangosd`.`game_event_creature` VALUES (3008916, 51);

-- Add pat for Worgen

INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008910, 1, -382.181, 1570.67, 16.961, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.403891, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008910, 2, -379.38, 1572.53, 16.7838, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.694488, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008910, 3, -369.441, 1579.26, 16.7701, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.97076, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008910, 4, -373.897, 1583.97, 17.1676, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.2967, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008910, 5, -379.014, 1589.39, 17.2118, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.2967, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008910, 6, -386.714, 1593.99, 16.7651, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.63442, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008910, 7, -393.862, 1588.19, 17.1849, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4.12668, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008910, 8, -393.784, 1578.47, 17.8744, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4.88459, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008910, 9, -390.237, 1570.61, 16.9771, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5.46971, 0, 0);

INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 1, -392.727, 1555.82, 17.1764, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.592381, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 2, -387.342, 1554.73, 16.7328, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5.94095, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 3, -390.143, 1544.12, 16.8032, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4.40549, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 4, -391.954, 1534.89, 16.8775, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.88182, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 5, -401.508, 1536.62, 17.173, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.99963, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 6, -410.951, 1543.39, 17.1922, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.65798, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 7, -416.812, 1548.05, 17.7608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.60693, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 8, -424.208, 1551.16, 18.3696, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.60693, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 9, -429.021, 1552.79, 18.3278, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.9368, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 10, -416.235, 1546.81, 17.6732, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5.65427, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 11, -415.047, 1540.61, 17.2976, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3.84001, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 12, -419.956, 1536.64, 17.63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3.80859, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 13, -425.084, 1531.36, 18.2603, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3.96175, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 14, -429.651, 1525.79, 18.5796, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4.07956, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 15, -423.778, 1532.59, 18.1165, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.773029, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 16, -416.668, 1538.53, 17.1238, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.670927, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 17, -409.094, 1544.45, 16.9799, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.62773, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (3008911, 18, -400.958, 1550.35, 16.7739, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.62773, 0, 0);

-- Updated ReqCreatureOrGo for the quests Curing Sickness (6124 & 6129).

REPLACE INTO `zp_mangosd`.`quest_template` (`entry`, `Method`, `ZoneOrSort`, `MinLevel`, `QuestLevel`, `Type`, `RequiredClasses`, `RequiredRaces`, `RequiredSkill`, `RequiredSkillValue`, `RepObjectiveFaction`, `RepObjectiveValue`, `RequiredMinRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepFaction`, `RequiredMaxRepValue`, `SuggestedPlayers`, `LimitTime`, `QuestFlags`, `SpecialFlags`, `PrevQuestId`, `NextQuestId`, `ExclusiveGroup`, `NextQuestInChain`, `SrcItemId`, `SrcItemCount`, `SrcSpell`, `Title`, `Details`, `Objectives`, `OfferRewardText`, `RequestItemsText`, `EndText`, `ObjectiveText1`, `ObjectiveText2`, `ObjectiveText3`, `ObjectiveText4`, `ReqItemId1`, `ReqItemId2`, `ReqItemId3`, `ReqItemId4`, `ReqItemCount1`, `ReqItemCount2`, `ReqItemCount3`, `ReqItemCount4`, `ReqSourceId1`, `ReqSourceId2`, `ReqSourceId3`, `ReqSourceId4`, `ReqSourceCount1`, `ReqSourceCount2`, `ReqSourceCount3`, `ReqSourceCount4`, `ReqCreatureOrGOId1`, `ReqCreatureOrGOId2`, `ReqCreatureOrGOId3`, `ReqCreatureOrGOId4`, `ReqCreatureOrGOCount1`, `ReqCreatureOrGOCount2`, `ReqCreatureOrGOCount3`, `ReqCreatureOrGOCount4`, `ReqSpellCast1`, `ReqSpellCast2`, `ReqSpellCast3`, `ReqSpellCast4`, `RewChoiceItemId1`, `RewChoiceItemId2`, `RewChoiceItemId3`, `RewChoiceItemId4`, `RewChoiceItemId5`, `RewChoiceItemId6`, `RewChoiceItemCount1`, `RewChoiceItemCount2`, `RewChoiceItemCount3`, `RewChoiceItemCount4`, `RewChoiceItemCount5`, `RewChoiceItemCount6`, `RewItemId1`, `RewItemId2`, `RewItemId3`, `RewItemId4`, `RewItemCount1`, `RewItemCount2`, `RewItemCount3`, `RewItemCount4`, `RewRepFaction1`, `RewRepFaction2`, `RewRepFaction3`, `RewRepFaction4`, `RewRepFaction5`, `RewRepValue1`, `RewRepValue2`, `RewRepValue3`, `RewRepValue4`, `RewRepValue5`, `RewOrReqMoney`, `RewMoneyMaxLevel`, `RewSpell`, `RewSpellCast`, `RewMailTemplateId`, `RewMailDelaySecs`, `PointMapId`, `PointX`, `PointY`, `PointOpt`, `DetailsEmote1`, `DetailsEmote2`, `DetailsEmote3`, `DetailsEmote4`, `DetailsEmoteDelay1`, `DetailsEmoteDelay2`, `DetailsEmoteDelay3`, `DetailsEmoteDelay4`, `IncompleteEmote`, `CompleteEmote`, `OfferRewardEmote1`, `OfferRewardEmote2`, `OfferRewardEmote3`, `OfferRewardEmote4`, `OfferRewardEmoteDelay1`, `OfferRewardEmoteDelay2`, `OfferRewardEmoteDelay3`, `OfferRewardEmoteDelay4`, `StartScript`, `CompleteScript`) VALUES (6124, 2, -263, 14, 14, 0, 1024, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6123, 0, 0, 6125, 15826, 1, 0, 'Curing the Sick', 'The amount of salve made out of the reagents you brought me is good enough for ten doses, $N.  While I work with the village to try and make some more salve, I\'d like for you to help us get a head start.  All around Darkshore are sickly deer; use the salve on them and cure their malaise.$B$BDendrite Starblaze in Moonglade will want to talk to you when you\'re done, I\'m sure.  Please extend Auberdine\'s thanks to the Cenarion Circle there... and let me say thanks to you directly for the help you\'ve given!', 'Use the Curative Animal Salve on 10 Sickly Deer that are located throughout Darkshore; doing so should cure them.  Sickly Deer have been reported starting south of the Cliffspring River to the north of Auberdine and extending all the way into southern Darkshore where the edge of Ashenvale begins.', 'You have done well in tending to the sickly deer of Darkshore; they are unwitting victims of a greater taint and corruption that is spreading all throughout the land.  There are those in Darnassus who choose to recognize this threat... and there are those who ignore the growing signs.  I hope you will not be one to put blinders on about what facts stare you in the face, $N.$B$BRegardless, you\'ve proven your worth as a friend of nature and the Circle - take this as a symbol of such.', 'Have you completed what was asked of you by Alanndarian Nightsong of Auberdine? Your work is not complete unless it is.', '', 'Sickly Deer cured', '', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12298, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15866, 0, 0, 0, 1, 0, 0, 0, 609, 0, 0, 0, 0, 150, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
REPLACE INTO `zp_mangosd`.`quest_template` (`entry`, `Method`, `ZoneOrSort`, `MinLevel`, `QuestLevel`, `Type`, `RequiredClasses`, `RequiredRaces`, `RequiredSkill`, `RequiredSkillValue`, `RepObjectiveFaction`, `RepObjectiveValue`, `RequiredMinRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepFaction`, `RequiredMaxRepValue`, `SuggestedPlayers`, `LimitTime`, `QuestFlags`, `SpecialFlags`, `PrevQuestId`, `NextQuestId`, `ExclusiveGroup`, `NextQuestInChain`, `SrcItemId`, `SrcItemCount`, `SrcSpell`, `Title`, `Details`, `Objectives`, `OfferRewardText`, `RequestItemsText`, `EndText`, `ObjectiveText1`, `ObjectiveText2`, `ObjectiveText3`, `ObjectiveText4`, `ReqItemId1`, `ReqItemId2`, `ReqItemId3`, `ReqItemId4`, `ReqItemCount1`, `ReqItemCount2`, `ReqItemCount3`, `ReqItemCount4`, `ReqSourceId1`, `ReqSourceId2`, `ReqSourceId3`, `ReqSourceId4`, `ReqSourceCount1`, `ReqSourceCount2`, `ReqSourceCount3`, `ReqSourceCount4`, `ReqCreatureOrGOId1`, `ReqCreatureOrGOId2`, `ReqCreatureOrGOId3`, `ReqCreatureOrGOId4`, `ReqCreatureOrGOCount1`, `ReqCreatureOrGOCount2`, `ReqCreatureOrGOCount3`, `ReqCreatureOrGOCount4`, `ReqSpellCast1`, `ReqSpellCast2`, `ReqSpellCast3`, `ReqSpellCast4`, `RewChoiceItemId1`, `RewChoiceItemId2`, `RewChoiceItemId3`, `RewChoiceItemId4`, `RewChoiceItemId5`, `RewChoiceItemId6`, `RewChoiceItemCount1`, `RewChoiceItemCount2`, `RewChoiceItemCount3`, `RewChoiceItemCount4`, `RewChoiceItemCount5`, `RewChoiceItemCount6`, `RewItemId1`, `RewItemId2`, `RewItemId3`, `RewItemId4`, `RewItemCount1`, `RewItemCount2`, `RewItemCount3`, `RewItemCount4`, `RewRepFaction1`, `RewRepFaction2`, `RewRepFaction3`, `RewRepFaction4`, `RewRepFaction5`, `RewRepValue1`, `RewRepValue2`, `RewRepValue3`, `RewRepValue4`, `RewRepValue5`, `RewOrReqMoney`, `RewMoneyMaxLevel`, `RewSpell`, `RewSpellCast`, `RewMailTemplateId`, `RewMailDelaySecs`, `PointMapId`, `PointX`, `PointY`, `PointOpt`, `DetailsEmote1`, `DetailsEmote2`, `DetailsEmote3`, `DetailsEmote4`, `DetailsEmoteDelay1`, `DetailsEmoteDelay2`, `DetailsEmoteDelay3`, `DetailsEmoteDelay4`, `IncompleteEmote`, `CompleteEmote`, `OfferRewardEmote1`, `OfferRewardEmote2`, `OfferRewardEmote3`, `OfferRewardEmote4`, `OfferRewardEmoteDelay1`, `OfferRewardEmoteDelay2`, `OfferRewardEmoteDelay3`, `OfferRewardEmoteDelay4`, `StartScript`, `CompleteScript`) VALUES (6129, 2, -263, 14, 14, 0, 1024, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6128, 0, 0, 6130, 15826, 1, 0, 'Curing the Sick', 'I was able to make enough salve for ten doses of curing, $N.  While I work with local authorities to try and make some more salve, I\'d like for you to help us get a head start.  All around the northern Barrens are sickly gazelles; use the salve on them and cure their malaise.$B$BDendrite Starblaze in Moonglade will want to talk to you when you\'re done, I\'m sure.  Please extend our thanks to the Cenarion Circle there... and let me say thanks to you directly for the help you\'ve given!', 'Use the Curative Animal Salve on 10 Sickly Gazelles that are located throughout the northern part of the Barrens; doing so should cure them.  Sickly Gazelles have been reported north of the east-west road that runs through the Crossroads.', 'You have done well in tending to the sickly gazelles of the Barrens; they are unwitting victims of a greater taint and corruption that is spreading all throughout the land. You $R are most eager to understand the ways of Cenarius; you, like your elders in Thunder Bluff, are proving that the need for balance transcends artificial racial barriers.$B$B$N, you\'ve proven your worth as an friend of nature and the Circle - take this as a symbol of such.', 'Have you completed what was asked of you by Tonga Runetotem at the Crossroads? Your work is not complete unless it is.', '', 'Sickly Gazelle cured', '', '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12296, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15866, 0, 0, 0, 1, 0, 0, 0, 609, 0, 0, 0, 0, 75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

/************************ Barthilas event Strat UD + remove port at baron on aggro **************************************/

-- add yell
REPLACE INTO `zp_scriptdevzerod`.`script_texts` VALUES (-1329020, 'Intruders at the Service Gate! Lord Rivendare must be warned!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 6, 0, 0, 'barthilas SAY_WARN_BARON');

-- add waypoints 
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (54237, 1, 3690.36, -3604.17, 138.395, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3010930, 0.020707, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (54237, 2, 3714.41, -3605.97, 141.494, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3010931, 0.283816, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (54237, 3, 3778.16, -3580.09, 142.436, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3010932, 5.05904, 0, 0);
INSERT INTO `zp_mangosd`.`creature_movement` VALUES (54237, 4, 3781.45, -3588.16, 143.669, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3010933, 5.0826, 0, 0);

-- Make sure he doesn't use waypoint movement on spawn
UPDATE `zp_mangosd`.`creature` SET `MovementType`=0 WHERE  `guid`=54237;

-- Dalinda Malem no longer hostile to mobs around, unless on escort q
UPDATE `zp_mangosd`.`creature_template` SET `faction_A`=35 WHERE  `entry`=5644;