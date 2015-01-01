-- Create the table for warden checks.
CREATE TABLE `warden_data_result` (
  `id` int(4) NOT NULL AUTO_INCREMENT,
  `check` int(3) DEFAULT NULL,
  `data` tinytext,
  `str` tinytext,
  `address` int(8) DEFAULT NULL,
  `length` int(2) DEFAULT NULL,
  `result` tinytext,
  `comment` text,
	PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- Add the os column to the account database. (Used with Warden to select the correct module.)
ALTER TABLE `zp_realmd`.`account` 
ADD COLUMN `os` VARCHAR(4) NOT NULL DEFAULT '' AFTER `locale`;

-- Basic Warden checks from the mangoszero project.
INSERT INTO `warden_data_result` (`id`, `check`, `data`, `str`, `address`, `length`, `result`, `comment`) VALUES 
('1', '217', '', 'SPEEDHACK-X86_64.DLL', '0', '0', '', 'SPEED HACK - Injected DLL'),
('2', '217', '', 'WPESPY.DLL', '0', '0', '', 'WPE PRO - Injected DLL'),
('3', '217', '', 'IHOOK.DLL', '0', '0', '', 'ASM - Injected DLL'),
('4', '217', '', 'WHITEMAGIC.DLL', '0', '0', '', 'Application Inject Library - Injected DLL'),
('5', '217', '', 'GREYMAGIC.DLL', '0', '0', '', 'Automation Botting Library - Injected DLL'),
('6', '217', '', 'BLACKMAGIC.DLL', '0', '0', '', 'Memory Inject Library - Injected DLL'),
('7', '217', '', 'RPE.DLL', '0', '0', '', 'rEdoX Packet Editor - Injected DLL');

-- Memory scans
INSERT INTO `zp_realmd`.`warden_data_result` (`check`, `address`, `length`, `result`, `comment`) VALUES ('243', '8151565', '2', '0020', 'Air swim hack');

INSERT INTO `zp_realmd`.`warden_data_result` (`check`, `address`, `length`, `result`, `comment`) VALUES ('243', '8445948', '4', 'BB8D243F', 'Wallclimb');
INSERT INTO `zp_realmd`.`warden_data_result` (`check`, `address`, `length`, `result`, `comment`) VALUES ('243', '6382555', '1', '8A', 'Unstunnable');

INSERT INTO `zp_realmd`.`warden_data_result` (`check`, `address`, `length`, `result`, `comment`) VALUES ('243', '8151646', '1', '30', 'Infinite jumps');

INSERT INTO `zp_realmd`.`warden_data_result` (`check`, `address`, `length`, `result`, `comment`) VALUES ('243', '8152026', '1', '8B', 'No falldamage');


-- SQL for protected tickets.
ALTER TABLE `zp_charactersd`.`character_ticket` 
ADD COLUMN `is_protected` VARCHAR(2) NOT NULL DEFAULT 0 AFTER `ticket_lastchange`;


-- Anti-cheat system message.
INSERT INTO `zp_mangosd`.`mangos_string` (`entry`, `content_default`) VALUES ('11000', 'Anti-cheat: The player %s has failed the following check: %s');
