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
include 'objective.inc';
include 'portal.inc';

if (!(file_exists("level1.dat"))) {
    // generate a default level 
    $ret = array(
        'mapWidth' => 60,
        'mapHeight' => 22,
        'vpWidth' => 30,
        'skyHeight' => 8,
        'enableRocks' => true,
        'maxRocks' => 15,
        'rocksAtStart' => false,
        'enableRain' => true,
        'maxRain' => 7,
        'rainStrengthMax' => 9,
        'rainStrengthMin' => 7,
        'rainAtStart' => true,
        'enableMinerals' => true,
        'maxMinerals' => 10,
        'mineralsAtStart' => true,
        'enablePlants' => true,
        'maxPlants' => 8,
        'plantsAtStart' => true,
        'enableCrystals' => true,
        'maxCrystals' => 15,
        'crystalHealthBonus' => 50,
        'crystalsAtStart' => true,
        'enableMeteors' => true,
        'meteorTicks' => 15,
        'meteorSpeedMin' => 2,
        'meteorSpeedMax' => 3,
        'enableToxicity' => true,
        'toxicityTicks' => 20,
        'startToxicity' => 0,
        'toxicityMin' => 2,
        'toxicityMax' => 4,
        'toxicityMultiplier' => 5,
        'map' => array(
            "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDRRRRRDDDDDDDDDDDRRRRRRDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDRRRRRRRRDDDDDDDDDTTTTTTTTDDDD",
            "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDDDDDDDTDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDTDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDRRRDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDRDRDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDRRRDDDDDTDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDTDDDDDDDDDDDDDDDDDDDDDDDDD"
        ),
        'nextLevel' => "level2.dat"
    );

    $l = new Level();
    $l->takeArray($ret);

    $l->addObjective(new CollectObjective("health", "1200"));
    $l->addObjective(new CollectObjective("crystals", 10));
    
    $l->saveToJSON(Config::$defaultLevelFile);
}

// create level 2.
if (!file_exists("level2.dat")) {
    $ret = array();

    $l = new Level();
    $l->takeArray($ret);
    $l->saveToJSON("level2.dat");
}

// GAME START
echo "\n";

if (file_exists("rainsave")) {
    $map = unserialize(file_get_contents("rainsave"));
    
} else {
    $map = new Map(Config::$defaultLevelFile);
}

$map->gameLoop();
?>