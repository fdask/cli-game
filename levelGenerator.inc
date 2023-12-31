<?php
class levelGenerator {
    public static function generate() {
        $ret = array(
            'mapWidth' => rand(40, 200),
            'mapHeight' => rand(20, 30),
            'vpWidth' => rand(25, 40),
            'skyHeight' => rand(2, 12),
            'enableRocks' => rand(0, 1) ? true : false,
            'maxRocks' => rand(10, 35),
            'rocksAtStart' => rand(0, 1) ? true : false,
            'enableRain' => rand(0, 1) ? true : false,
            'maxRain' => rand(2, 15),
            'rainStrengthMax' => 9,
            'rainStrengthMin' => 7,
            'rainAtStart' => rand(0, 1) ? true : false,
            'enableMinerals' => rand(0, 1) ? true : false,
            'maxMinerals' => rand(5, 35),
            'mineralsAtStart' => rand(0, 1) ? true : false,
            'enablePlants' => rand(0, 1) ? true : false,
            'maxPlants' => rand(2, 15),
            'plantsAtStart' => rand(0, 1) ? true : false,
            'enableCrystals' => rand(0, 1) ? true : false,
            'maxCrystals' => rand(2, 15),
            'crystalHealthBonus' => rand(50, 200),
            'crystalsAtStart' => rand(0, 1) ? true : false,
            'crystalDepth' => rand(1, 6),
            'enableMeteors' => rand(0, 1) ? true : false,
            'meteorTicks' => rand(5, 50),
            'meteorSpeedMin' => 1,
            'meteorSpeedMax' => 8,
            'enableToxicity' => rand(0, 1) ? true : false,
            'toxicityTicks' => rand(10, 30),
            'startToxicity' => rand(0, 20),
            'toxicityMin' => rand(1, 3),
            'toxicityMax' => rand(4, 9),
            'toxicityMultiplier' => rand(5, 20),
            'nextLevel' => "generate"
        );

        if (!$ret['enableRain']) {
            // we can't have moisture
            $ret['enableMoisture'] = false;
        }

        if ($ret['maxPlants'] > $ret['mapWidth']) {
            $ret['maxPlants'] = $ret['mapWidth'];
        }

        // generate some objectives
        $type = rand(0, 5);
        
        switch ($type) {
            case 0:
            case 1:
            case 2:
            case 3:
            default:
                $ret['objectives'] = serialize(array(new CollectObjective("health", "1200")));
        }
        
        return $ret;
    }
}

// CREATE THE LEVEL FILES

// 1
//if (!(file_exists("level1.dat"))) {
    // generate a default level 
    $ret = array(
        'mapWidth' => 60,
        'mapHeight' => 22,
        'vpWidth' => 30,
        'skyHeight' => 8,
        'enableRocks' => true,
        'maxRocks' => 15,
        'rocksAtStart' => true,
        'enableRain' => true,
        'maxRain' => 7,
        'rainStrengthMax' => 9,
        'rainStrengthMin' => 7,
        'rainAtStart' => true,
        'enableMinerals' => true,
        'maxMinerals' => 10,
        'mineralsAtStart' => true,
        'enablePlants' => true,
        'maxPlants' => 8,
        'plantsAtStart' => true,
        'enableCrystals' => true,
        'maxCrystals' => 15,
        'crystalHealthBonus' => 50,
        'crystalsAtStart' => true,
        'enableMeteors' => true,
        'meteorTicks' => 15,
        'meteorSpeedMin' => 2,
        'meteorSpeedMax' => 3,
        'enableToxicity' => true,
        'toxicityTicks' => 20,
        'startToxicity' => 0,
        'toxicityMin' => 2,
        'toxicityMax' => 4,
        'toxicityMultiplier' => 5,
        'map' => array(
            "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDRRRRRDDDDDDDDDDDRRRRRRDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDRRRRRRRRDDDDDDDDDTTTTTTTTDDDD",
            "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDDDDDDDTDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDTDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDRRRDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDRDRDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDRRRDDDDDTDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
            "DDDDDDDDDDDDDDDDDDTDDDDDDDDDDDDDDDDDDDDDDDDD"
        ),
        'nextLevel' => "level2.dat"
    );

    $l = new Level();
    $l->takeArray($ret);

    $l->addObjective(new CollectObjective("health", "1200"));
    $l->addObjective(new CollectObjective("crystals", 10));
    
    $l->saveToJSON("level1.dat");
//}

// 2
//if (!file_exists("level2.dat")) {
    $ret = array();

    $l = new Level();
    $l->takeArray($ret);
    $l->saveToJSON("level2.dat");
///}

// 3
//if (!file_exists("level3.dat")) {
    $three = array(
        'name' => "testing percentages",
        'mapWidth' => 50,
        'mapHeight' => 22,
        'vpWidth' => 30,
        'skyHeight' => 8,
        'enableRocks' => false,
        'maxRocks' => 25,
        'rocksAtStart' => true,
        'enableRain' => false,
        'maxRain' => 7,
        'rainStrengthMax' => 9,
        'rainStrengthMin' => 7,
        'rainAtStart' => false,
        'enableMinerals' => true,
        'maxMinerals' => 50,
        'mineralsAtStart' => true,
        'enablePlants' => true,
        'maxPlants' => 8,
        'plantsAtStart' => true,
        'enableCrystals' => false,
        'maxCrystals' => 50,
        'crystalHealthBonus' => 50,
        'crystalsAtStart' => true,
        'crystalDepth' => 8,
        'enableMeteors' => false,
        'meteorTicks' => 15,
        'meteorSpeedMin' => 2,
        'meteorSpeedMax' => 3,
        'enableToxicity' => false,
        'toxicityTicks' => 20,
        'startToxicity' => 0,
        'toxicityMin' => 2,
        'toxicityMax' => 4,
        'toxicityMultiplier' => 5,
        'nextLevel' => "level4.dat"
    );

    $threeL = new Level();
    $threeL->takeArray($three);
    $threeL->addObjective(new CollectObjective("health", "1200"));
    $threeL->saveToJSON("level3.dat");
//}