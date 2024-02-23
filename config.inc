<?php
class Config {
    // turn on debugging
    public static $debug = true;

    // initial map
    public static $defaultLevelFile = "level1.dat";
    public static $fallbackLevelFilename = 'random.dat';
    
    // basic map settings
    public static $defaultSkyHeight = 10;
    public static $defaultMapHeight = 25;
    //public static $defaultGroundHeight = 15;
    public static $defaultVpWidth = 30;
    public static $defaultMapWidth = 60;

    // level environment setting defaults
    public static $defaultMaxRocks = 10;
    
    public static $defaultMaxRain = 2;
    public static $defaultRainStrengthMin = 6;
    public static $defaultRainStrengthMax = 9;
    public static $defaultNewRainStyle = false;
    
    public static $defaultEnableMoisture = false;
    public static $defaultMoistureMax = 5;
    public static $defaultMoistureTicks = 25;
    
    public static $defaultMaxMinerals = 10;
    public static $defaultMineralsAtStart = true;
    public static $defaultMineralConcentrationMin = 1;
    public static $defaultMineralConcentrationMax = 9;
    public static $defaultNewMineralsPlantsOnly = false;

    public static $defaultMaxPlants = 7;
    public static $defaultPlantsAtStart = true;
    public static $defaultPlantTicks = 5;
    public static $defaultPlantEnlightenedAt = 2;
    
    public static $defaultMaxCrystals = 5;
    public static $defaultMinCrystals = 0;
    public static $defaultCrystalHealthBonus = 50;
    public static $defaultCrystalsAtStart = true;
    public static $defaultCrystalDepth = 5;

    public static $defaultMeteorTicks = 10;
    public static $defaultMeteorSpeedMin = 1;
    public static $defaultMeteorSpeedMax = 5;
    public static $defaultToxicityTicks = 20;
    public static $defaultStartToxicity = 0;
    public static $defaultToxicityMin = 1;
    public static $defaultToxicityMax = 9;
    public static $defaultToxicityMultiplier = 10;

    public static $defaultEnableFossils = false;
    public static $defaultFossilQualityMin = 1;
    public static $defaultFossilQualityMax = 99;
    public static $defaultFossilInRockPercentage = 20;

    public static $defaultEnableWorms = false;
    public static $defaultMaxWorms = 2;
    public static $defaultWormLengthMax = 4;
    public static $defaultWormHealth = 20;
    public static $defaultWormTicks = 5;
    public static $defaultBetweenWormCounter = 10;
    
    public static $defaultEnableWormHoles = false;
    
    public static $defaultEnableNibbits = false;

    public static $defaultEnableFloods = false;
    public static $defaultFloodDurationMin = 5;
    public static $defaultFloodDurationMax = 80;
    public static $defaultFloodWidthMin = 50;
    public static $defaultFloodWidthMax = 100;
    public static $defaultFloodStrength = 25;
    public static $defaultFloodChance = 5;

    public static $defaultEnableStorms = false;
    public static $defaultBetweenStormsMin = 0;
    public static $defaultBetweenStormsMax = 100;

    public static $defaultEnableDeer = false;
    public static $defaultDeerHeight = 2;
    public static $defaultMaxDeer = 1;
    public static $defaultDeerToxicAt = 20;
    public static $defaultDeerDamage = 25;
    public static $defaultBetweenDeerTicks = 20;

    public static $defaultEnableSeep = false;
    public static $defaultSeepTickMin = 2;
    public static $defaultSeepTickMax = 10;

    // player defaults
    public static $defaultPlayerHealth = 1000;
    public static $defaultPlayerInvulnerableState = false;
    public static $defaultScoreWater = 10;
	public static $defaultScoreMinerals = 10;
	public static $defaultScoreMoves = 1;
	public static $defaultScoreCrystals = 100;
    public static $defaultScoreFossils = 500;
  
    public static $termReset = false;
    public static $resetMovesCount = 100;

    public static $defaultEnableCompass = true;
    public static $compassRow = 0;

    public static $defaultEnableMushrooms = false;
    public static $defaultMushroomHealthCost = 500;

    // color data
    public static $rainColor = 'blue';
    public static $wormColor = 'white';

    public static $mineralColor = 'brown';
    public static $strongMineralColor = 'yellow';

    public static $playerHeadColor = 'bold_gray';
    public static $playerTailColor = 'bold_gray';

    public static $toxicWasteColor = 'bold_green';

    public static $crystalColor = 'purple';

    public static $meteorHeadColor = 'dark_gray';
    public static $meteorTailColor = 'red';

    public static $plantColor = 'green';
    public static $enlightenedPlantColor = 'green';
    public static $mushroomColor = 'white';
    public static $nibbitColor = 'dark_gray';
    public static $turnedNibbitColor = 'bold_red';
    public static $compassColor = 'black';
    public static $defaultBorderColor = 'black';
    
    // bg color
    public static $powerSelectSelected = 'blue';
    public static $deerColor = 'green';
    public static $floodColor = 'blue';
    public static $lightningColor = 'yellow';
    public static $thunderColor = 'light_gray';
    public static $defaultStormBorderColor = 'black';

    // fg
    public static $powerSelectUnselected = 'bold_blue';

    public static function shiftUpArrow() {
        return chr(27) . chr(91) . chr(49) . chr(59) . chr(50) . chr(65);
    }

    public static function upArrow() {
        return chr(27) . chr(91) . chr(65);
    }

    public static function downArrow() {
        return chr(27) . chr(91) . chr(66);
    }

    public static function shiftDownArrow() {
        return chr(27) . chr(91) . chr(49) . chr(59) . chr(50) . chr(66);
    }

    public static function leftArrow() {
        return chr(27) . chr(91) . chr(68);
    }

    public static function shiftLeftArrow() {
        return chr(27) . chr(91) . chr(49) . chr(59) . chr(50) . chr(68);
    }

    public static function rightArrow() {
        return chr(27) . chr(91) . chr(67);
    }

    public static function shiftRightArrow() {
        return chr(27) . chr(91) . chr(49) . chr(59) . chr(50) . chr(67);
    }

    public static function autoload($class) {
        $file = __DIR__ . "/include/" . str_replace('\\', '/', strtolower($class)) . ".inc";
        
        if (file_exists($file)) {
            require $file;
        } else {
            if (Config::$debug) {
                debug_print_backtrace();
            }
            
            die("Couldnt find file: $file\n");
        }
    }
}

spl_autoload_register("Config::autoload");