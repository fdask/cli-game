<?php
class LevelGenerator {
    public static function random() {
        $ret = array(
            'mapWidth' => rand(40, 200),
            'mapHeight' => rand(20, 30),
            'vpWidth' => rand(25, 40),
            'skyHeight' => rand(2, 12),
            'enableRocks' => rand(0, 1) ? true : false,
            'maxRocks' => rand(10, 35),
            'enableRain' => rand(0, 1) ? true : false,
            'maxRain' => rand(2, 15),
            'rainStrengthMax' => 9,
            'rainStrengthMin' => 7,
            'newRainStyle' => rand(0, 1) ? true : false,
            'enableMinerals' => rand(0, 1) ? true : false,
            'maxMinerals' => rand(5, 35),
            'mineralsAtStart' => rand(0, 1) ? true : false,
            'enablePlants' => rand(0, 1) ? true : false,
            'maxPlants' => rand(2, 15),
            'plantsAtStart' => rand(0, 1) ? true : false,
            'enableCrystals' => rand(0, 1) ? true : false,
            'maxCrystals' => rand(10, 25),
            'minCrystals' => rand(0, 9),
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
            'enableFossils' => rand(0, 1) ? true : false,
            'fossilQualityMin' => rand(1, 25), 
            'fossilQualityMax' => rand(50, 100),
            'fossilInRockPercentage' => rand(20, 80),
            'enableWorms' => rand(0, 1) ? true : false, 
            'maxWorms' => rand(1, 4),
            'wormHealth' => rand(20, 80),
            'nextLevel' => "generate"
        );

        if (!$ret['enableRain']) {
            // we can't have moisture
            $ret['enableMoisture'] = false;
        } else {
            // rain is enabled, make sure it's not too much
            if ($ret['maxRain'] > $ret['mapWidth']) {
                $ret['maxRain'] = $ret['mapWidth'];
            }
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

    public static function levelOne() {
        $ret = array(
            'name' => "Level 1",
            'mapWidth' => 60,
            'mapHeight' => 22,
            'vpWidth' => 30,
            'skyHeight' => 8,
            'enableSeep' => true,
            'seepTickMin' => 2,
            'seepTickMax' => 8,
            'enableRocks' => false,
            'maxRocks' => 25,
            'enableRain' => true,
            'maxRain' => 7,
            'rainStrengthMax' => 9,
            'rainStrengthMin' => 7,
            'enableMoisture' => false,
            'enableMinerals' => true,
            'maxMinerals' => 10,
            'mineralsAtStart' => true,
            'newMineralsPlantsOnly' => true,
            'enablePlants' => true,
            'maxPlants' => 15,
            'plantsAtStart' => false,
            'plantEnlightenedAt' => 1,
            'enableCrystals' => true,
            'maxCrystals' => 20,
            'minCrystals' => 10,
            'crystalHealthBonus' => 50,
            'crystalsAtStart' => true,
            'enableMeteors' => true,
            'meteorTicks' => 25,
            'meteorSpeedMin' => 2,
            'meteorSpeedMax' => 3,
            'enableToxicity' => true,
            'toxicityTicks' => 20,
            'startToxicity' => 20,
            'toxicityMin' => 2,
            'toxicityMax' => 4,
            'toxicityMultiplier' => 5,
            'enableFossils' => true,
            'fossilQualityMin' => 50,
            'fossilQualityMax' => 100,
            'fossilInRockPercentage' => 50,
            'enableWorms' => false,
            'maxWorms' => 3,
            'wormHealth' => 25,
            'wormTicks' => 2,
            'enableWormHoles' => true,
            'enableNibbits' => true,
            'enableFloods' => true,
            'floodChance' => 2,
            'enableDeer' => false,
            'maxDeer' => 1,
            'deerDamage' => 50,
            'betweenDeerTicks' => 10,
            'playerStartX' => 10,
            'playerStartY' => 0,
            'enableStorms' => true,
            'enableMushrooms' => true,
            'enableCompass' => false,
            'map' => array(
                "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
                "DDDDDDDDDRRRRRDDDDDDDDDDDRRRRRRDDDDDDDDDDDDD",
                "DDDDDDDDDDDDDDDRRRRRRRRDDDDDDDDDTTTTTTTTDDDD",
                "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
                "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
                "DDDDDDDDDDDDDDDDDDDDDDDDTDDDDDDDDDDDDDDDDDDD",
                "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD",
                "DDDDDDDDDDDDDDDDDDTDDDDDDDDDDDDDDDDDDDDDDDDD",
                "DDDDDDDRRRDDDDRRRRRRRRRRRRRRRRRRRRRDDDDDDDDD",
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
    
        /*
        $l->addObjective(new CollectObjective("health", "1200"));
        $l->addObjective(new CollectObjective("crystals", 10));
        $l->addObjective(new ConnectObjective(18, 20, 20, 22));
        $l->addObjective(new NibbitObjective(true));
        $l->addObjective(new CollectObjective("moves", 10));
        */
        $l->addObjective(new DirectionObjective("S"));

        $l->wormHoles = array(
            new WormHole(15, 15, 20, 20, true)
        );

        $l->nibbits = array(
            new Nibbit(9, 30, 3),
            new Nibbit(15, 29, 2),
            new Nibbit(12, 0, 3)
        );

        $l->saveToJSON("level1.dat");
    }
}