#!/usr/bin/php
<?php
// define some system variables
include 'wetable.inc';
include 'dirt.inc';
include 'mailbox.inc';
include 'rock.inc';
include 'map.inc';
include 'player.inc';
include 'sky.inc';
include 'plant.inc';
include 'poi.inc';
include 'meteor.inc';
include 'colors.inc';
include 'config.inc';
include 'level.inc';

/* generate a default level 
$ret = array(
    'mapWidth' => 60,
    'mapHeight' => 18,
    'vpWidth' => 30,
    'skyHeight' => 8,
    'enableRocks' => true,
    'maxRocks' => 15,
    'enableRain' => true,
    'maxRain' => 7,
    'rainStrengthMax' => 9,
    'rainStrengthMin' => 7,
    'enableMinerals' => true,
    'maxMinerals' => 10,
    'enablePlants' => true,
    'maxPlants' => 8,
    'enableCrystals' => true,
    'maxCrystals' => 15,
    'crystalHealthBonus' => 50,
    'enableMeteors' => true,
    'meteorTicks' => 15,
    'enableToxicity' => true,
    'toxicityTicks' => 20,
    'startToxicity' => 0,
    'toxicityMin' => 2,
    'toxicityMax' => 4,
    'toxicityMultiplier' => 5
);

$l = new Level();
$l->takeArray($ret);
$l->saveToJSON("level1.dat");

exit;
*/

// GAME START
echo "\n";

if (file_exists("rainsave")) {
    $map = unserialize(file_get_contents("rainsave"));
    $map->gameLoop();
} else {
    $map = new Map(Config::$defaultLevelFile);
}
?>