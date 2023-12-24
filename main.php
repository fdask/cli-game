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

$skyHeight = 10;
$groundHeight = 15;
$vpWidth = 40;
$mapWidth = 50;
$maxRain = 8;
$maxMinerals = 15;

echo "\n";

$p = new Player($skyHeight, 20);
$map = new Map($skyHeight, $groundHeight, $vpWidth, $mapWidth, $maxRain, $maxMinerals, $p);
?>