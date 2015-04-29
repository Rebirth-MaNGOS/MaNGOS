CREATE TABLE `zp_mangosd`.`exclusive_pool_creature` (
      `guid` INT NOT NULL,
      `poolID` INT NULL,
      `groupID` INT NULL DEFAULT 0,
      PRIMARY KEY (`guid`));

CREATE TABLE `zp_mangosd`.`exclusive_pool_spot` (
      `id` INT NOT NULL AUTO_INCREMENT,
      `poolID` INT NULL,
      `mapID` INT NULL,
      `x` FLOAT NULL,
      `y` FLOAT NULL,
      `z` FLOAT NULL,
      PRIMARY KEY (`id`));

CREATE TABLE `zp_mangosd`.`exclusive_pool` (
      `poolID` INT NOT NULL,
      `respawnTime` INT NULL,
      PRIMARY KEY (`poolID`));

CREATE TABLE `zp_mangosd`.`exclusive_pool_respawn` (
      `poolID` INT NOT NULL,
      `spawntime` INT NULL,
      PRIMARY KEY (`poolID`));
