<?php
class Level {
    // how wide is the total map	
	public $mapWidth;

	// how high is the map (hehehe)
	public $mapHeight;

    // how wide is the viewport
    public $vpWidth;

    // how much of the map is sky
    public $skyHeight;

    // rain settings
    public $enableRain;

	// how many raindrops allowed on the map at once
	public $maxRain;

    public $rainStrengthMax;
    
    public $rainStrengthMin;

    // whether we want rocks
    public $enableRocks;

    // maximum number of rocks
    public $maxRocks;

    // mineral settings
	public $enableMinerals;

	// how many mineral spaces to allow on the map at once
	public $maxMinerals;

    // plant settings
	public $enablePlants;

	// how many plants to allow on the map at once
	public $maxPlants;

    // crystal settings
	public $enableCrystals;

    // how many crystals to allow at once
    public $maxCrystals;

    // meteors
    public $enableMeteors;

    // how long between meteors
    public $meteorTicks;

    // toxicity
    public $enableToxicity;

    public $toxicityTicks;

    public $startToxicity;

    // an array of points of interest
    public $pointsOfInterest;

    // what does the level want us to do
    public $objectives;

    public function __construct($filename = null) {
        if (!is_null($filename)) {
            if (file_exists($filename)) {
                // load the json encoded level data array
                $array = json_decode(file_get_contents($filename), true);

                // now break out all the variables
                $this->takeArray($array);
            }
        }
    }

    /**
     * adds a new point of interest object into the array
     */
    public function addPointOfInterest($poi) {
        $this->pointsOfInterest[] = $poi;
    }

    /**
     * looks for the supplied poi and removes from the array
     */
    public function removePointOfInterest($poi) {
        // remove the given object from the array
        for ($x = 0; $x < count($this->pointsOfInterest); $x++) {
            if ($this->pointsOfInterest[$x] == $poi) {
                array_splice($this->pointsOfInterest, $x, 1);

                return true;
            }
        }

        return false;
    }

    /**
     * converts a level object into a json encodable array
     * 
     * there has to be an easier way
     */
    public function makeArray() {
        $ret = array(
            'mapWidth' => $this->mapWidth,
            'mapHeight' => $this->mapHeight,
            'vpWidth' => $this->vpWidth,
            'skyHeight' => $this->skyHeight,
            'enableRocks' => $this->enableRocks,
            'maxRocks' => $this->maxRocks,
            'enableRain' => $this->enableRain,
            'maxRain' => $this->maxRain,
            'rainStrengthMax' => $this->rainStrengthMax,
            'rainStrengthMin' => $this->rainStrengthMin,
            'enableMinerals' => $this->enableMinerals,
            'maxMinerals' => $this->maxMinerals,
            'enablePlants' => $this->enablePlants,
            'maxPlants' => $this->maxPlants,
            'enableCrystals' => $this->enableCrystals,
            'maxCrystals' => $this->maxCrystals,
            'enableMeteors' => $this->enableMeteors,
            'meteorTicks' => $this->meteorTicks,
            'enableToxicity' => $this->enableToxicity,
            'toxicityTicks' => $this->toxicityTicks,
            'startToxicity' => $this->startToxicity,
            'pointsOfInterest' => $this->pointsOfInterest
        );

        return $ret;
    }

    /**
     * takes an array and applies it to the level object
     */
    public function takeArray($array) {
        $this->mapWidth = isset($array['mapWidth']) ? $array['mapWidth'] : Config::$defaultMapWidth;
        $this->mapHeight = isset($array['mapHeight']) ? $array['mapHeight'] : Config::$defaultMapHeight;
        $this->vpWidth = isset($array['vpWidth']) ? $array['vpWidth'] : Config::$defaultVpWidth;
        $this->skyHeight = isset($array['skyHeight']) ? $array['skyHeight'] : Config::$defaultSkyHeight;
        $this->enableRocks = isset($array['enableRocks']) ? $array['enableRocks'] : false;
        $this->maxRocks = isset($array['maxRocks']) ? $array['maxRocks'] : Config::$defaultMaxRocks;
        $this->enableRain = isset($array['enableRain']) ? $array['enableRain'] : false;
        $this->maxRain = isset($array['maxRain']) ? $array['maxRain'] : Config::$defaultMaxRain;
        $this->rainStrengthMax = isset($array['rainStrengthMax']) ? $array['rainStrengthMax'] : Config::$defaultRainStrengthMax;
        $this->rainStrengthMin = isset($array['rainStrengthMin']) ? $array['rainStrengthMin'] : Config::$defaultRainStrengthMin;
        $this->enableMinerals = isset($array['enableMinerals']) ? $array['enableMinerals'] : false;
        $this->maxMinerals = isset($array['maxMinerals']) ? $array['maxMinerals'] : Config::$defaultMaxMinerals;
        $this->enablePlants = isset($array['enablePlants']) ? $array['enablePlants'] : false;
        $this->maxPlants = isset($array['maxPlants']) ? $array['maxPlants'] : Config::$defaultMaxPlants;
        $this->enableCrystals = isset($array['enableCrystals']) ? $array['enableCrystals'] : false;
        $this->maxCrystals = isset($array['maxCrystals']) ? $array['maxCrystals'] : Config::$defaultMaxCrystals;
        $this->enableMeteors = isset($array['enableMeteors']) ? $array['enableMeteors'] : false;
        $this->meteorTicks = isset($array['meteorTicks']) ? $array['meteorTicks'] : Config::$defaultMeteorTicks;
        $this->enableToxicity = isset($array['enableToxicity']) ? $array['enableToxicity'] : false;
        $this->toxicityTicks = isset($array['toxicityTicks']) ? $array['toxicityTicks'] : Config::$defaultToxicityTicks;
        $this->startToxicity = isset($array['startToxicity']) ? $array['startToxicity'] : Config::$defaultStartToxicity;
        $this->pointsOfInterest = isset($array['pointsOfInterest']) ? $array['pointsOfInterest'] : array();
    }

    /**
     * write the level to a json file
     */
    public function saveToJSON($filename) {
        if (file_put_contents($filename, json_encode($this->makeArray()))) {
            return true;
        }

        return false;
    }
}

