#!/usr/bin/php
<?php
// define some system variables
include 'config.inc';

// generate level files if they don't already exist
if (1) {
    LevelGenerator::levelOne();
}

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