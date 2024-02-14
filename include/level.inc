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

    public $enableSeep;
    public $seepTickMin;
    public $seepTickMax;

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
    public $newMineralsPlantsOnly;

	// how many mineral spaces to allow on the map at once
	public $maxMinerals;

    // plant settings
	public $enablePlants;
    
	// how many plants to allow on the map at once
	public $maxPlants;
    public $plantsAtStart;
    public $plantTicks;
    public $plantEnlightenedAt;

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

    public $enableWorms;
    public $maxWorms;
    public $wormLengthMax;
    public $wormHealth;
    public $wormTicks;
    public $betweenWormCounter;

    public $enableWormHoles;
    public $wormHoles;

    public $enableNibbits;
    public $nibbits;

    public $enableFloods;
    public $floodChance;

    public $enableDeer;
    public $maxDeer;
    public $deerDamage;
    public $betweenDeerTicks;

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
        if ($filename) {
            // load the json encoded level data array
            $array = json_decode(file_get_contents($filename), true);
        } else {
            $array = LevelGenerator::random();
        }

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
            'enableSeep' => $this->enableSeep,
            'seepTickMin' => $this->seepTickMin,
            'seepTickMax' => $this->seepTickMax,
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
            'newMineralsPlantsOnly' => $this->newMineralsPlantsOnly,
            'enablePlants' => $this->enablePlants,
            'maxPlants' => $this->maxPlants,
            'plantsAtStart' => $this->plantsAtStart,
            'plantTicks' => $this->plantTicks,
            'plantEnlightenedAt' => $this->plantEnlightenedAt,
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
            'enableWorms' => $this->enableWorms,
            'maxWorms' => $this->maxWorms,
            'wormLengthMax' => $this->wormLengthMax,
            'wormHealth' => $this->wormHealth,
            'wormTicks' => $this->wormTicks,
            'betweenWormCounter' => $this->betweenWormCounter,
            'enableWormHoles' => $this->enableWormHoles,
            'wormHoles' => serialize($this->wormHoles),
            'enableNibbits' => $this->enableNibbits,
            'nibbits' => serialize($this->nibbits),
            'enableFloods' => $this->enableFloods,
            'floodChance' => $this->floodChance,
            'enableDeer' => $this->enableDeer,
            'maxDeer' => $this->maxDeer,
            'deerDamage' => $this->deerDamage,
            'betweenDeerTicks' => $this->betweenDeerTicks,
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
        $this->enableSeep = isset($array['enableSeep']) ? $array['enableSeep'] : Config::$defaultEnableSeep;
        $this->seepTickMin = isset($array['seepTickMin']) ? $array['seepTickMin'] : Config::$defaultSeepTickMin;
        $this->seepTickMax = isset($array['seepTickMax']) ? $array['seepTickMax'] : Config::$defaultSeepTickMax;
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
        $this->newMineralsPlantsOnly = isset($array['newMineralsPlantsOnly']) ? $array['newMineralsPlantsOnly'] : Config::$defaultNewMineralsPlantsOnly;
        $this->enablePlants = isset($array['enablePlants']) ? $array['enablePlants'] : false;
        $this->maxPlants = isset($array['maxPlants']) ? $array['maxPlants'] : Config::$defaultMaxPlants;
        $this->plantsAtStart = isset($array['plantsAtStart']) ? $array['plantsAtStart'] : Config::$defaultPlantsAtStart;
        $this->plantTicks = isset($array['plantTicks']) ? $array['plantTicks'] : Config::$defaultPlantTicks;
        $this->plantEnlightenedAt = isset($array['plantEnlightenedAt']) ? $array['plantEnlightenedAt'] : Config::$defaultPlantEnlightenedAt;
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
        $this->enableWorms = isset($array['enableWorms']) ? $array['enableWorms'] : Config::$defaultEnableWorms;
        $this->maxWorms = isset($array['maxWorms']) ? $array['maxWorms'] : Config::$defaultMaxWorms;
        $this->wormLengthMax = isset($array['wormLengthMax']) ? $array['wormLengthMax'] : Config::$defaultWormLengthMax;
        $this->wormHealth = isset($array['wormHealth']) ? $array['wormHealth'] : Config::$defaultWormHealth;
        $this->wormTicks = isset($array['wormTicks']) ? $array['wormTicks'] : Config::$defaultWormTicks;
        $this->betweenWormCounter = isset($array['betweenWormCounter']) ? $array['betweenWormCounter'] : Config::$defaultBetweenWormCounter;
        $this->enableWormHoles = isset($array['enableWormHoles']) ? $array['enableWormHoles'] : Config::$defaultEnableWormHoles;
        $this->wormHoles = isset($array['wormHoles']) ? unserialize($array['wormHoles']) : array();
        $this->enableNibbits = isset($array['enableNibbits']) ? $array['enableNibbits'] : Config::$defaultEnableNibbits;
        $this->nibbits = isset($array['nibbits']) ? unserialize($array['nibbits']) : array();
        $this->enableFloods = isset($array['enableFloods']) ? $array['enableFloods'] : Config::$defaultEnableFloods;
        $this->floodChance = isset($array['floodChance']) ? $array['floodChance'] : Config::$defaultFloodChance;
        $this->enableDeer = isset($array['enableDeer']) ? $array['enableDeer'] : Config::$defaultEnableDeer;
        $this->maxDeer = isset($array['maxDeer']) ? $array['maxDeer'] : Config::$defaultMaxDeer;
        $this->deerDamage = isset($array['deerDamage']) ? $array['deerDamage'] : Config::$defaultDeerDamage;
        $this->betweenDeerTicks = isset($array['betweenDeerTicks']) ? $array['betweenDeerTicks'] : Config::$defaultBetweenDeerTicks;

        $this->pointsOfInterest = isset($array['pointsOfInterest']) ? unserialize($array['pointsOfInterest']) : array();
        $this->objectives = isset($array['objectives']) ? unserialize($array['objectives']) : array();

        $this->map = isset($array['map']) ? $array['map'] : null;

        $this->playerStartX = isset($array['playerStartX']) ? $array['playerStartX'] : $this->skyHeight;
        $this->playerStartY = isset($array['playerStartY']) ? $array['playerStartY'] : floor($this->vpWidth / 2);
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

