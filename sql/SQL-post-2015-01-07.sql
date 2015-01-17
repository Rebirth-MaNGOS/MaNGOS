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

