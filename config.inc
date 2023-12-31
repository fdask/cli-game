<?php
class Config {
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

    public static $defaultMaxPlants = 7;
    public static $defaultPlantsAtStart = true;
    
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

    // player defaults
    public static $defaultPlayerHealth = 1000;
    public static $defaultPlayerInvulnerableState = false;
    public static $defaultScoreWater = 10;
	public static $defaultScoreMinerals = 10;
	public static $defaultScoreMoves = 1;
	public static $defaultScoreCrystals = 100;

    // color data
    public static $rainColor = 'blue';

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

    // bg color
    public static $powerSelectSelected = 'blue';

    // fg
    public static $powerSelectUnselected = 'bold_blue';

}