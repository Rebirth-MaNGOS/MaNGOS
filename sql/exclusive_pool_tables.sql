CREATE TABLE `zp_mangosd`.`exclusive_pool_creature` (
      `id` INT NOT NULL AUTO_INCREMENT,
      `poolID` INT NULL,
      `groupID` INT NULL DEFAULT 0,
      `guid` INT NOT NULL,
      PRIMARY KEY (`id`));

CREATE TABLE `zp_mangosd`.`exclusive_pool_spot` (
      `id` INT NOT NULL AUTO_INCREMENT,
      `poolID` INT NULL,
      `mapID` INT NULL,
      `x` FLOAT NULL,
      `y` FLOAT NULL,
      `z` FLOAT NULL,
      PRIMARY KEY (`id`));

CREATE TABLE `zp_mangosd`.`exclusive_pool` (
      `id` INT NOT NULL AUTO_INCREMENT,
      `poolID` INT NULL,
      `respawnTime` INT NULL,
      PRIMARY KEY (`id`));

