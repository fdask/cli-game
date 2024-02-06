#!/usr/bin/php
<?php
// define some system variables
include 'config.inc';

/*
include 'wetable.inc';
include 'treadable.inc';

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
include 'power.inc';
include 'fossil.inc';
include 'worm.inc';
include 'pathfinding.inc';
include 'deer.inc';
include 'titlescreen.inc';
*/

// GAME START
echo "\n";

if (file_exists("rainsave")) {
    $map = unserialize(file_get_contents("rainsave"));    
} else {
    // display the title screen
    $choice = TitleScreen::run();

    if ($choice == 1) {
        // load the game
        $map = new Map(Config::$defaultLevelFile);
    } else if ($choice == 2) {
        $map = new Map(Config::$defaultLevelFile);
    }
}

// start the game
$map->gameLoop();
?>