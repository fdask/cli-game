<?php
class Config {
    // initial map
    public static $defaultLevelFile = "level1.dat";

    // basic map settings
    public static $defaultSkyHeight = 10;
    public static $defaultGroundHeight = 15;
    public static $defaultVpWidth = 30;
    public static $defaultMapWidth = 60;

    // level environment setting defaults
    public static $defaultMaxRocks = 10;
    public static $defaultMaxRain = 2;
    public static $defaultRainStrengthMin = 6;
    public static $defaultRainStrengthMax = 9;
    public static $defaultMaxMinerals = 10;
    public static $defaultMaxPlants = 7;
    public static $defaultMaxCrystals = 5;
    public static $defaultMeteorTicks = 10;
    public static $defaultToxicityTicks = 20;
    
    // player defaults
    public static $defaultPlayerHealth = 1000;

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

}