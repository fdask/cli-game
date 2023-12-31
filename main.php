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
include 'levelGenerator.inc';

$ret = LevelGenerator::generate();

$l = new Level();
$l->takeArray($ret);
$l->saveToJSON("random.dat");

// GAME START
echo "\n";

if (file_exists("rainsave")) {
    $map = unserialize(file_get_contents("rainsave"));
    
} else {
    $map = new Map(Config::$defaultLevelFile);
}

$map->gameLoop();
?>