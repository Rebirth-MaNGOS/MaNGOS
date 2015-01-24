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

/********************** Drakonids' incorrect summon spell **************************************************************/
-- Delet casting of the summon spell.
select * from zp_mangosd.creature_ai_scripts where creature_id in (select entry from creature_template where name like "%Drakonid");

/********************* §§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§ ***************************************************************/
INSERT INTO `zp_mangosd`.`creature_template` (`entry`, `KillCredit1`, `KillCredit2`, `modelid_1`, `modelid_2`, `name`, `gossip_menu_id`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `aggrorangeoverride`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `Civilian`, `RacialLeader`, `RegenHealth`, `equipment_id`, `trainer_id`, `vendor_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES ('800020', '0', '0', '1206', '0', 'Schnake', '0', '1', '1', '64', '64', '0', '0', '20', '7', '7', '0', '0.75', '0.95', '0', '0', '2', '2', '0', '1', '1', '2000', '0', '0', '1', '32768', '0', '0', '0', '0', '0', '0', '1.76', '2.42', '100', '8', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '1', '1', '0', '0', '1', '0', '0', '0', '0', '0', 'mob_schnake');
UPDATE `zp_mangosd`.`creature_template` SET `subname`='andkem\'s pet' WHERE `entry`='800020';

-- Shatterspear Drummer & Revantusk Drummer

REPLACE INTO `zp_mangosd`.`creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES (100001102, 11196, 1, 0, 100, 1, 0, 0, 0, 0, 5, 36, 0, 0, 5, 333, 0, 0, 0, 0, 0, 0, '');
REPLACE INTO `zp_mangosd`.`creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES (100001103, 14734, 1, 0, 100, 1, 0, 0, 0, 0, 5, 36, 0, 0, 5, 333, 0, 0, 0, 0, 0, 0, '');
REPLACE INTO `zp_mangosd`.`creature_template` (`entry`, `KillCredit1`, `KillCredit2`, `modelid_1`, `modelid_2`, `name`, `subname`, `gossip_menu_id`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `aggrorangeoverride`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `Civilian`, `RacialLeader`, `RegenHealth`, `equipment_id`, `trainer_id`, `vendor_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES (14734, 0, 0, 14764, 14765, 'Revantusk Drummer', NULL, 0, 50, 50, 3517, 3517, 0, 0, 3900, 1494, 1494, 0, 1.1, 1.11286, 0, 0, 85, 109, 0, 45, 1, 2000, 0, 0, 1, 36864, 0, 0, 0, 0, 0, 0, 66.44, 91.355, 100, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'EventAI', 0, 3, 0, 0, 1, 2067, 0, 0, 0, 2, '');

-- SQL for the quests Egg Collection and Egg Freezing

REPLACE INTO `zp_mangosd`.`item_template` (`entry`, `class`, `subclass`, `name`, `displayid`, `Quality`, `Flags`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `dmg_min3`, `dmg_max3`, `dmg_type3`, `dmg_min4`, `dmg_max4`, `dmg_type4`, `dmg_min5`, `dmg_max5`, `dmg_type5`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`, `Duration`, `ExtraFlags`) VALUES (12241, 12, 0, 'Collected Dragon Egg', 1659, 1, 0, 1, 0, 0, 0, -1, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 8, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 4, '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'internalitemhandler', 0, 0, 0, 0, 0, 0);
REPLACE INTO `zp_mangosd`.`spell_script_target` (`entry`, `type`, `targetEntry`) VALUES (15958, 0, 175124);

-- SQL for the quests Triage (Horde & Alliance)

REPLACE INTO `zp_mangosd`.`quest_template` (`entry`, `Method`, `ZoneOrSort`, `MinLevel`, `QuestLevel`, `Type`, `RequiredClasses`, `RequiredRaces`, `RequiredSkill`, `RequiredSkillValue`, `RepObjectiveFaction`, `RepObjectiveValue`, `RequiredMinRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepFaction`, `RequiredMaxRepValue`, `SuggestedPlayers`, `LimitTime`, `QuestFlags`, `SpecialFlags`, `PrevQuestId`, `NextQuestId`, `ExclusiveGroup`, `NextQuestInChain`, `SrcItemId`, `SrcItemCount`, `SrcSpell`, `Title`, `Details`, `Objectives`, `OfferRewardText`, `RequestItemsText`, `EndText`, `ObjectiveText1`, `ObjectiveText2`, `ObjectiveText3`, `ObjectiveText4`, `ReqItemId1`, `ReqItemId2`, `ReqItemId3`, `ReqItemId4`, `ReqItemCount1`, `ReqItemCount2`, `ReqItemCount3`, `ReqItemCount4`, `ReqSourceId1`, `ReqSourceId2`, `ReqSourceId3`, `ReqSourceId4`, `ReqSourceCount1`, `ReqSourceCount2`, `ReqSourceCount3`, `ReqSourceCount4`, `ReqCreatureOrGOId1`, `ReqCreatureOrGOId2`, `ReqCreatureOrGOId3`, `ReqCreatureOrGOId4`, `ReqCreatureOrGOCount1`, `ReqCreatureOrGOCount2`, `ReqCreatureOrGOCount3`, `ReqCreatureOrGOCount4`, `ReqSpellCast1`, `ReqSpellCast2`, `ReqSpellCast3`, `ReqSpellCast4`, `RewChoiceItemId1`, `RewChoiceItemId2`, `RewChoiceItemId3`, `RewChoiceItemId4`, `RewChoiceItemId5`, `RewChoiceItemId6`, `RewChoiceItemCount1`, `RewChoiceItemCount2`, `RewChoiceItemCount3`, `RewChoiceItemCount4`, `RewChoiceItemCount5`, `RewChoiceItemCount6`, `RewItemId1`, `RewItemId2`, `RewItemId3`, `RewItemId4`, `RewItemCount1`, `RewItemCount2`, `RewItemCount3`, `RewItemCount4`, `RewRepFaction1`, `RewRepFaction2`, `RewRepFaction3`, `RewRepFaction4`, `RewRepFaction5`, `RewRepValue1`, `RewRepValue2`, `RewRepValue3`, `RewRepValue4`, `RewRepValue5`, `RewOrReqMoney`, `RewMoneyMaxLevel`, `RewSpell`, `RewSpellCast`, `RewMailTemplateId`, `RewMailDelaySecs`, `PointMapId`, `PointX`, `PointY`, `PointOpt`, `DetailsEmote1`, `DetailsEmote2`, `DetailsEmote3`, `DetailsEmote4`, `DetailsEmoteDelay1`, `DetailsEmoteDelay2`, `DetailsEmoteDelay3`, `DetailsEmoteDelay4`, `IncompleteEmote`, `CompleteEmote`, `OfferRewardEmote1`, `OfferRewardEmote2`, `OfferRewardEmote3`, `OfferRewardEmote4`, `OfferRewardEmoteDelay1`, `OfferRewardEmoteDelay2`, `OfferRewardEmoteDelay3`, `OfferRewardEmoteDelay4`, `StartScript`, `CompleteScript`) VALUES (6622, 2, -324, 35, 45, 0, 0, 178, 129, 225, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 16991, 1, 0, 'Triage', 'Good day, doctor. You will be tested on your ability to triage patients accordingly this afternoon. Should you pass your examination, you will join the prestigious ranks of Horde Trauma.$B$BNow pay attention! The critically injured must be tended to first. After the criticals come the badly injured. The injured should be tended to last. You must save 15 patients to pass this exam. You will fail should 6 of your patients die in triage.$B$BTriage bandages will be supplied.$B$BGood luck, doctor.\r\n', 'Save patients by using Triage Bandages to tend to their wounds. You must save 15 patients before 6 of them die from their injuries.$B$BRemember the order of triage! Critically Injured Soldiers die the fastest followed by Badly Injured Soldiers. Injured Soldiers take the longest to die and should be saved AFTER your Critically Injured and Badly Injured Soldiers have been triaged.', 'Doctor $N, welcome to Horde Trauma.', 'Miraculous work, doctor. Simply miraculous.', '15 Patients Saved!', '', '', '', '', 16991, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3300, 10846, 10847, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
REPLACE INTO `zp_mangosd`.`quest_template` (`entry`, `Method`, `ZoneOrSort`, `MinLevel`, `QuestLevel`, `Type`, `RequiredClasses`, `RequiredRaces`, `RequiredSkill`, `RequiredSkillValue`, `RepObjectiveFaction`, `RepObjectiveValue`, `RequiredMinRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepFaction`, `RequiredMaxRepValue`, `SuggestedPlayers`, `LimitTime`, `QuestFlags`, `SpecialFlags`, `PrevQuestId`, `NextQuestId`, `ExclusiveGroup`, `NextQuestInChain`, `SrcItemId`, `SrcItemCount`, `SrcSpell`, `Title`, `Details`, `Objectives`, `OfferRewardText`, `RequestItemsText`, `EndText`, `ObjectiveText1`, `ObjectiveText2`, `ObjectiveText3`, `ObjectiveText4`, `ReqItemId1`, `ReqItemId2`, `ReqItemId3`, `ReqItemId4`, `ReqItemCount1`, `ReqItemCount2`, `ReqItemCount3`, `ReqItemCount4`, `ReqSourceId1`, `ReqSourceId2`, `ReqSourceId3`, `ReqSourceId4`, `ReqSourceCount1`, `ReqSourceCount2`, `ReqSourceCount3`, `ReqSourceCount4`, `ReqCreatureOrGOId1`, `ReqCreatureOrGOId2`, `ReqCreatureOrGOId3`, `ReqCreatureOrGOId4`, `ReqCreatureOrGOCount1`, `ReqCreatureOrGOCount2`, `ReqCreatureOrGOCount3`, `ReqCreatureOrGOCount4`, `ReqSpellCast1`, `ReqSpellCast2`, `ReqSpellCast3`, `ReqSpellCast4`, `RewChoiceItemId1`, `RewChoiceItemId2`, `RewChoiceItemId3`, `RewChoiceItemId4`, `RewChoiceItemId5`, `RewChoiceItemId6`, `RewChoiceItemCount1`, `RewChoiceItemCount2`, `RewChoiceItemCount3`, `RewChoiceItemCount4`, `RewChoiceItemCount5`, `RewChoiceItemCount6`, `RewItemId1`, `RewItemId2`, `RewItemId3`, `RewItemId4`, `RewItemCount1`, `RewItemCount2`, `RewItemCount3`, `RewItemCount4`, `RewRepFaction1`, `RewRepFaction2`, `RewRepFaction3`, `RewRepFaction4`, `RewRepFaction5`, `RewRepValue1`, `RewRepValue2`, `RewRepValue3`, `RewRepValue4`, `RewRepValue5`, `RewOrReqMoney`, `RewMoneyMaxLevel`, `RewSpell`, `RewSpellCast`, `RewMailTemplateId`, `RewMailDelaySecs`, `PointMapId`, `PointX`, `PointY`, `PointOpt`, `DetailsEmote1`, `DetailsEmote2`, `DetailsEmote3`, `DetailsEmote4`, `DetailsEmoteDelay1`, `DetailsEmoteDelay2`, `DetailsEmoteDelay3`, `DetailsEmoteDelay4`, `IncompleteEmote`, `CompleteEmote`, `OfferRewardEmote1`, `OfferRewardEmote2`, `OfferRewardEmote3`, `OfferRewardEmote4`, `OfferRewardEmoteDelay1`, `OfferRewardEmoteDelay2`, `OfferRewardEmoteDelay3`, `OfferRewardEmoteDelay4`, `StartScript`, `CompleteScript`) VALUES (6624, 2, -324, 35, 45, 0, 0, 77, 129, 225, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 16991, 1, 0, 'Triage', 'Good day, doctor. You will be tested on your ability to triage patients accordingly this afternoon. Should you pass your examination, you will join the prestigious ranks of Alliance Trauma.$B$BNow pay attention! The critically injured must be tended to first. After the criticals come the badly injured. The injured should be tended to last. You must save 15 patients to pass this exam. You will fail should 6 of your patients die in triage.$B$BGood luck, doctor.\r\n', 'Save patients by using Triage Bandages to tend to their wounds. You must save 15 patients before 6 of them die from their injuries.$B$BRemember the order of triage! Critically Injured Soldiers die the fastest followed by Badly Injured Soldiers. Injured Soldiers take the longest to die and should be saved AFTER your Critically Injured and Badly Injured Soldiers have been triaged.', 'Doctor $N, welcome to Alliance Trauma.', 'Miraculous work, doctor. Simply miraculous.', '15 Patients Saved!', '', '', '', '', 16991, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3300, 10846, 10847, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

-- AI for the creepy children in Elwynn
UPDATE `zp_mangosd`.`creature_template` SET `ScriptName`='npc_creepy_child' WHERE `entry`='805';

-- Waypoints for the creepy children in Elwynn

REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 1, -9374.92, -70.0467, 69.2019, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 2, -9379.69, -74.8843, 69.1775, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 3, -9384.07, -77.3692, 69.1705, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 4, -9389.04, -74.0014, 66.6089, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 5, -9392.53, -70.8163, 64.4377, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 6, -9389.36, -67.4792, 64.4377, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 7, -9386.81, -71.2856, 64.4377, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 8, -9380.38, -78.1003, 64.4377, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 9, -9384.77, -86.1301, 63.3096, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 10, -9378.32, -91.5387, 63.3628, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 11, -9366.93, -87.3724, 64.7227, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 12, -9362.44, -78.7915, 65.6496, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 13, -9357.63, -66.2752, 64.6178, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 14, -9353.02, -53.6981, 65.9888, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 15, -9352.38, -41.4122, 64.7904, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 16, -9358.69, -33.3503, 63.9862, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 17, -9368.49, -23.3577, 62.9392, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 18, -9380.61, -10.734, 61.6121, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 19, -9392.31, 2.27052, 60.347, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 20, -9401.73, 12.6294, 59.6123, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 21, -9410.96, 23.1564, 58.4466, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 22, -9422.29, 36.4951, 57.2348, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 23, -9431.41, 47.1088, 56.6259, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 24, -9436.99, 54.8988, 56.0985, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 25, -9438.42, 69.6597, 56.0359, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 26, -9429.29, 76.4232, 56.3587, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 27, -9417.98, 84.6701, 56.7489, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 28, -9406.75, 93.027, 57.9189, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 29, -9389.73, 101.474, 59.2328, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 30, -9373.39, 107.718, 60.6218, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 31, -9353.93, 115.592, 62.1915, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 32, -9335.41, 125.434, 63.566, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 33, -9320.17, 134.038, 64.3909, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 34, -9307.98, 140.921, 65.5701, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 35, -9293.66, 146.645, 66.246, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 36, -9276.67, 150.723, 66.8527, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 37, -9261.17, 154.937, 67.4326, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 38, -9248.02, 164.703, 67.9164, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 39, -9238.82, 178.407, 67.7639, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 40, -9227.79, 191.745, 68.0667, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 41, -9215.91, 204.589, 69.2351, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 42, -9201.65, 220.004, 71.1137, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 43, -9194.51, 227.711, 71.743, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 44, -9186.31, 239.502, 72.5472, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 45, -9182.13, 250.62, 73.5495, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 46, -9179.94, 265.456, 74.6507, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 47, -9179.54, 286.441, 76.4524, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 48, -9179.59, 300.441, 78.3119, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 49, -9179.64, 314.441, 79.4413, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 50, -9178.06, 329.582, 81.8173, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 51, -9171.07, 344.218, 85.2986, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 52, -9160.35, 356.351, 88.5854, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 53, -9150.53, 366.324, 90.3022, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 54, -9138.25, 378.787, 90.6227, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 55, -9127.8, 388.084, 91.1491, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 56, -9114.19, 399.07, 92.4896, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 57, -9100.33, 409.752, 92.5327, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 58, -9083.43, 422.222, 92.5269, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 59, -9072.17, 430.535, 93.056, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 60, -9060.9, 438.847, 93.056, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 61, -9046.13, 448.738, 93.056, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 62, -9059.93, 438.016, 93.056, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 63, -9079.35, 423.079, 92.6157, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 64, -9102.09, 406.754, 92.6328, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 65, -9119.66, 395.253, 92.1658, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 66, -9140.88, 378.26, 90.6774, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 67, -9155.75, 360.831, 89.6287, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 68, -9168.26, 347.892, 86.3892, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 69, -9178.03, 332.599, 82.4261, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 70, -9180.47, 303.438, 78.5695, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 71, -9180.01, 271.951, 75.0707, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 72, -9187.23, 239.67, 72.4794, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 73, -9195.91, 226.552, 71.6698, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 74, -9215.97, 207.023, 69.5021, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 75, -9237.47, 179.994, 67.7288, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 76, -9250.79, 162.61, 67.8353, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 77, -9263.8, 154.776, 67.2889, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 78, -9295.33, 149.064, 66.398, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 79, -9320.21, 134.59, 64.4413, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 80, -9347.28, 118.486, 62.9028, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 81, -9375.48, 106.63, 60.5317, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 82, -9400.67, 97.5674, 58.5999, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 83, -9427.47, 77.8518, 56.494, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 84, -9456.88, 63.8368, 55.886, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 85, -9444.73, 56.9771, 56.0383, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 86, -9430.64, 45.0147, 56.7478, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 87, -9405.16, 16.174, 59.324, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 88, -9379.97, -12.9339, 61.778, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 89, -9360.99, -32.0807, 63.8395, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 90, -9342.64, -47.2905, 65.394, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 91, -9321.29, -54.5829, 66.4491, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 92, -9297.33, -59.6229, 67.2861, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 93, -9279.05, -70.7701, 68.2362, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 94, -9257.18, -87.4148, 69.6432, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 95, -9232.42, -102.215, 70.9959, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 96, -9205.05, -110.504, 71.0409, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 97, -9179.16, -112.823, 71.0026, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 98, -9163.96, -114.831, 72.6991, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 99, -9157.83, -130.137, 74.5694, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 100, -9155.91, -142.614, 73.5992, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 101, -9156.63, -128.637, 74.5852, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 102, -9167.42, -112.97, 72.1702, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 103, -9186.93, -113.558, 70.9205, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 104, -9209.51, -108.805, 71.1493, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 105, -9236.34, -100.77, 70.7459, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 106, -9256.71, -87, 69.6934, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 107, -9281.33, -67.3692, 68.1128, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 108, -9298.62, -59.5126, 67.1812, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 109, -9337.44, -50.5205, 65.6429, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 110, -9350.88, -53.3826, 65.8652, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 111, -9361.18, -71.2618, 63.9362, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 112, -9370.88, -87.1024, 64.2855, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 113, -9379.53, -91.6156, 63.1967, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 114, -9383.95, -85.1205, 63.356, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 115, -9379.52, -77.2238, 64.4379, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 116, -9386.38, -72.2163, 64.4379, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 117, -9391.07, -68.3173, 64.4379, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 118, -9392.32, -72.1618, 64.4379, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 119, -9388.09, -74.8549, 67.4644, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 120, -9382.2, -77.9452, 69.1685, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 121, -9377.15, -72.666, 69.1971, 0, NULL);
REPLACE INTO `zp_scriptdevzerod`.`script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES (805, 122, -9372.93, -66.503, 69.2026, 0, NULL);
