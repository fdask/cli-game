<?php
class Level {
    public $name;

    // whether we've finished the level or not
    public $completed;

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
    public $newRainStyle;

    public $enableMoisture;

    // whether we want rocks
    public $enableRocks;

    // maximum number of rocks
    public $maxRocks;

    // mineral settings
	public $enableMinerals;
    public $mineralsAtStart;
    public $mineralConcentrationMin;
    public $mineralConcentrationMax;

	// how many mineral spaces to allow on the map at once
	public $maxMinerals;

    // plant settings
	public $enablePlants;
    
	// how many plants to allow on the map at once
	public $maxPlants;
    public $plantsAtStart;

    // crystal settings
	public $enableCrystals;

    // % of crystals to allow at once
    public $maxCrystals;

    // add more if we dip below this %
    public $minCrystals;

    // how much health a user gets from a crystal
    public $crystalHealthBonus;
    public $crystalDepth;
    public $crystalsAtStart;

    // meteors
    public $enableMeteors;

    // how long between meteors
    public $meteorTicks;

    public $meteorSpeedMin;
    public $meteorSpeedMax;

    // toxicity
    public $enableToxicity;

    public $toxicityTicks;

    public $startToxicity;

    public $toxicityMin;
    public $toxicityMax;

    // how much damage done to player
    public $toxicityMultiplier;

    public $enableFossils;
    public $fossilQualityMin;
    public $fossilQualityMax;
    public $fossilInRockPercentage;

    // an array of points of interest
    public $pointsOfInterest;

    // what does the level want us to do
    public $objectives;

    // lay out a map
    public $map;

    public $nextLevel;

    public $playerStartX;
    public $playerStartY;

    public function __construct($filename = null) {
        if (is_null($filename) || !file_exists($filename)) {   
            $filename = Config::$fallbackLevelFilename;
        }
        
        // load the json encoded level data array
        $array = json_decode(file_get_contents($filename), true);

        // now break out all the variables
        $this->takeArray($array); 

        $this->completed = false;
    }

    public function getPlayerStart() {
        return [$this->playerStartX, $this->playerStartY];
    }

    public function getNextLevel() {
        return $this->nextLevel;
    }

    public function setNextLevel($nl) {
        $this->nextLevel = $nl;
    }

    public function setCompleted() {
        $this->completed = true;
    }

    public function getCompleted() {
        return $this->completed;
    }

    public function getMap() {
        return $this->map;
    }

    public function setMap($array) {
        $this->map = $array;

        // adjust the width based on the map
        $this->mapWidth = count($array[0]);
        $this->mapHeight = count($array) + $this->skyHeight;
    }

    public function getName() {
        return $this->name;
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

    public function getObjectives() {
        return $this->objectives;
    }

    /**
     * returns a boolean indicating whether or not
     * all objectives in this level are completed.
     */
    public function objectivesComplete() {
        if (empty($this->objectives)) {
            return true;
        } else {
            $ret = true;

            foreach ($this->objectives as $objective) {
                if (!$objective->completed) {
                    return false;
                }
            }

            return true;
        }
    }

    public function getObjectiveScreen() {
        $ret = array(

        );
    }

    public function addObjective($obj) {
        $this->objectives[] = $obj;
    }

    /**
     * converts a level object into a json encodable array
     * 
     * there has to be an easier way
     */
    public function makeArray() {
        $ret = array(
            'name' => $this->name,
            'playerStartX' => $this->playerStartX,
            'playerStartY' => $this->playerStartY,
            'nextLevel' => $this->nextLevel,
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
            'newRainStyle' => $this->newRainStyle,
            'enableMoisture' => $this->enableMoisture,
            'enableMinerals' => $this->enableMinerals,
            'maxMinerals' => $this->maxMinerals,
            'mineralsAtStart' => $this->mineralsAtStart,
            'mineralConcentrationMin' => $this->mineralConcentrationMin,
            'mineralConcentrationMax' => $this->mineralConcentrationMax,
            'enablePlants' => $this->enablePlants,
            'maxPlants' => $this->maxPlants,
            'plantsAtStart' => $this->plantsAtStart,
            'enableCrystals' => $this->enableCrystals,
            'maxCrystals' => $this->maxCrystals,
            'minCrystals' => $this->minCrystals,
            'crystalHealthBonus' => $this->crystalHealthBonus,
            'crystalsAtStart' => $this->crystalsAtStart,
            'crystalDepth' => $this->crystalDepth,
            'enableMeteors' => $this->enableMeteors,
            'meteorTicks' => $this->meteorTicks,
            'meteorSpeedMin' => $this->meteorSpeedMin,
            'meteorSpeedMax' => $this->meteorSpeedMax,
            'enableToxicity' => $this->enableToxicity,
            'toxicityTicks' => $this->toxicityTicks,
            'startToxicity' => $this->startToxicity,
            'toxicityMin' => $this->toxicityMin,
            'toxicityMax' => $this->toxicityMax,
            'toxicityMultiplier' => $this->toxicityMultiplier,
            'enableFossils' => $this->enableFossils,
            'fossilQualityMin' => $this->fossilQualityMin,
            'fossilQualityMax' => $this->fossilQualityMax,
            'fossilInRockPercentage' => $this->fossilInRockPercentage,
            'pointsOfInterest' => serialize($this->pointsOfInterest),
            'objectives' => serialize($this->objectives),
            'map' => $this->map
        );

        return $ret;
    }

    /**
     * takes an array and applies it to the level object 
     */
    public function takeArray($array) {
        $this->name = isset($array['name']) ? $array['name'] : "noname";
        $this->nextLevel = isset($array['nextLevel']) ? $array['nextLevel'] : null;
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
        $this->newRainStyle = isset($array['newRainStyle']) ? $array['newRainStyle'] : Config::$defaultNewRainStyle;
        $this->enableMoisture = isset($array['enableMoisture']) ? $array['enableMoisture'] : Config::$defaultEnableMoisture;
        $this->enableMinerals = isset($array['enableMinerals']) ? $array['enableMinerals'] : false;
        $this->maxMinerals = isset($array['maxMinerals']) ? $array['maxMinerals'] : Config::$defaultMaxMinerals;
        $this->mineralsAtStart = isset($array['mineralsAtStart']) ? $array['mineralsAtStart'] : Config::$defaultMineralsAtStart;
        $this->mineralConcentrationMin = isset($array['mineralConcentrationMin']) ? $array['mineralConcentrationMin'] : Config::$defaultMineralConcentrationMin;
        $this->mineralConcentrationMax = isset($array['mineralConcentrationMax']) ? $array['mineralConcentrationMax'] : Config::$defaultMineralConcentrationMax;
        $this->enablePlants = isset($array['enablePlants']) ? $array['enablePlants'] : false;
        $this->maxPlants = isset($array['maxPlants']) ? $array['maxPlants'] : Config::$defaultMaxPlants;
        $this->plantsAtStart = isset($array['plantsAtStart']) ? $array['plantsAtStart'] : Config::$defaultPlantsAtStart;
        $this->enableCrystals = isset($array['enableCrystals']) ? $array['enableCrystals'] : false;
        $this->maxCrystals = isset($array['maxCrystals']) ? $array['maxCrystals'] : Config::$defaultMaxCrystals;
        $this->minCrystals = isset($array['minCrystals']) ? $array['minCrystals'] : Config::$defaultMinCrystals;
        $this->crystalDepth = isset($array['crystalDepth']) ? $array['crystalDepth'] : Config::$defaultCrystalDepth;
        $this->crystalHealthBonus = isset($array['crystalHealthBonus']) ? $array['crystalHealthBonus'] : Config::$defaultCrystalHealthBonus;
        $this->crystalsAtStart = isset($array['crystalsAtStart']) ? $array['crystalsAtStart'] : Config::$defaultCrystalsAtStart;
        $this->enableMeteors = isset($array['enableMeteors']) ? $array['enableMeteors'] : false;
        $this->meteorTicks = isset($array['meteorTicks']) ? $array['meteorTicks'] : Config::$defaultMeteorTicks;
        $this->meteorSpeedMin = isset($array['meteorSpeedMin']) ? $array['meteorSpeedMin'] : Config::$defaultMeteorSpeedMin;
        $this->meteorSpeedMax = isset($array['meteorSpeedMax']) ? $array['meteorSpeedMax'] : Config::$defaultMeteorSpeedMax;
        $this->enableToxicity = isset($array['enableToxicity']) ? $array['enableToxicity'] : false;
        $this->toxicityTicks = isset($array['toxicityTicks']) ? $array['toxicityTicks'] : Config::$defaultToxicityTicks;
        $this->startToxicity = isset($array['startToxicity']) ? $array['startToxicity'] : Config::$defaultStartToxicity;
        $this->toxicityMin = isset($array['toxicityMin']) ? $array['toxicityMin'] : Config::$defaultToxicityMin;
        $this->toxicityMax = isset($array['toxicityMax']) ? $array['toxicityMax'] : Config::$defaultToxicityMax;
        $this->toxicityMultiplier = isset($array['toxicityMultiplier']) ? $array['toxicityMultiplier'] : Config::$defaultToxicityMultiplier;
        $this->enableFossils = isset($array['enableFossils']) ? $array['enableFossils'] : Config::$defaultEnableFossils;
        $this->fossilQualityMin = isset($array['fossilQualityMin']) ? $array['fossilQualityMin'] : Config::$defaultFossilQualityMin;
        $this->fossilQualityMax = isset($array['fossilQualityMax']) ? $array['fossilQualityMax'] : Config::$defaultFossilQualityMax;
        $this->fossilInRockPercentage = isset($array['fossilInRockPercentage']) ? $array['fossilInRockPercentage'] : Config::$defaultFossilInRockPercentage;
        $this->pointsOfInterest = isset($array['pointsOfInterest']) ? unserialize($array['pointsOfInterest']) : array();
        $this->objectives = isset($array['objectives']) ? unserialize($array['objectives']) : array();

        $this->map = isset($array['map']) ? $array['map'] : null;

        $this->playerStartX = isset($array['playerStartX']) ? $array['playerStartX'] : 0;
        $this->playerStartY = isset($array['playerStartY']) ? $array['playerStartY'] : $this->skyHeight;
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

