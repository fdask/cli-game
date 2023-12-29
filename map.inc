<?php
class Map {
	// an array that holds the objects on the map
	private $map;

	// stores current term settings to restore post game
	private $term;

	// holds the player object
	public $player;

	// the starting y position of the viewport
	public $vpY;

	// an array holding the integers 1-3 
	// to indicate whether char shows on
	// that screen
	public $charOnScreens;

	// which view we want to show the user
	// 1 - shows the rain/wetness
	// 2 - mineral view
	// 3 - plant view
	public $viewType;

	// whether we have enabled three screen view, or not.   toggles
	private $threeScreen;

    // an instance of the mailbox object
	public $mailbox;

	// a single meteor object, if one exists
	public $meteor;

	// countdown to launch
	public $meteorTicker;

	// the level data goes here
	public $level;

	public function __construct($filename = null) {
		if (!is_null($filename)) {
			$this->level = new Level($filename);
		} else {
			$this->level = new Level();
		}

		// screen settings
		$this->viewType = 1;
        $this->vpY = 0;
		$this->threeScreen = false;
		$this->charOnScreens = array(1, 2, 3);

		$this->mailbox = new Mailbox();
		$this->mailbox->addInstructions();
		
		$this->player = new Player($this->level->skyHeight, floor($this->level->vpWidth / 2));
		
		$this->initialize();
		$this->gameLoop();
	}

    public function checkPointsOfInterest() {
        // using the x, y of the player, 
        // lets figure out if any of the current 
        // points of interests apply
        foreach ($this->level->pointsOfInterest as $poi) {
            if ($poi instanceof messageX && $this->player->getX() == $poi->getX()) {
                $msg = $poi->getMessage();

                // add the message to the mailbox
                $this->mailbox->addMessage($msg);
                $poi->setDisplayed();
            } else if ($poi instanceof messageY && $this->player->getY() == $poi->getY()) {
                $msg = $poi->getMessage();

                // add the message to the mailbox
                $this->mailbox->addMessage($msg);
                $poi->setDisplayed();
            } else if ($poi instanceof messageXY && [$this->player->getX(), $this->player->getY()] == $poi->getXY()) {
                $msg = $poi->getMessage();

                // add the message to the mailbox
                $this->mailbox->addMessage($msg);
                $poi->setDisplayed();
            }

            if ($poi->getDisplayed()) {
                $this->removePointOfInterest($poi);
            }
        }
    }

	public function save() {
		file_put_contents("rainsave", serialize($this));
	}

	public function initialize() {
		// initialize the map array
		$this->initializeMap();

		// fill in the sky
		$this->fillMap(0, 0, $this->level->skyHeight, $this->level->mapWidth, "Sky");

		if ($this->level->enableRain) {
			// add in random rains up to maxRain
			for ($x = 0; $x < $this->level->maxRain; $x++) {
				$this->addRain();
			}
		}

		// fill in the dirt
		$this->fillMap($this->level->skyHeight, 0, $this->level->mapHeight, $this->level->mapWidth, "Dirt");		

		if ($this->level->enableMinerals) {
			// add in random minerals up to maxMinerals
			for ($x = 0; $x < $this->level->maxMinerals; $x++) {
				$this->addMineral();
			}
		}

		// add in plants up to maxPlants
		if ($this->level->enablePlants) {
			for ($x = 0; $x < $this->level->maxPlants; $x++) {
				$this->addPlant();
			}
		}

		if ($this->level->enableRocks) {
			// add in the rocks
			for ($x = 0; $x < $this->level->maxRocks; $x++) {
				$this->addRock();
			}
		}

		if ($this->level->enableCrystals) {
			// add in the crystals
			for ($x = 0; $x < $this->level->maxCrystals; $x++) {
				$this->addCrystal();
			}
		}

		if ($this->level->enableMeteors) {
			// decrement the tick.  if we're at zero, add a new one
			if ($this->meteorTicker > 0) {
				$this->meteorTicker--;
			} else {
				$this->meteorTicker = $this->level->meteorTicks;
				$this->addMeteor();
			}
		}

		if ($this->level->enableToxicity) {
			if ($this->level->startToxicity > 0) {
				for ($x = 0; $x < $this->level->startToxicity; $x++) {
					$this->addToxicWaste();
				}
			}
		}
	}

	private function initializeMap() {
		// fills the map array out with falses
		$this->map = array();

		for ($x = 0; $x < ($this->level->mapHeight); $x++) {
			for ($y = 0; $y < $this->level->mapWidth; $y++) {
				$this->map[$x][$y] = false;
			}
		}
	}

	public function gameOver() {
		echo "GAME OVER\n\n";

		// quit the game
		system("stty $this->term");

		exit;
	}

	public function gameLoop() {
		$this->term = `stty -g`;
		system("stty -icanon -echo");

		echo $this;

		// keypress handler
		while ($c = fread(STDIN, 1)) {
			$tick = true;

			switch ($c) {
                case '?':
                    // show keyboard commands
                    $this->mailbox->showKeyboardMsg();
                    $tick = false;

                    break;
				case '1':
					// switch to wetness view
					$this->viewType = 1;
					$tick = false;
	
					break;
				case '2':
					// switch to the mineral view
					$this->viewType = 2;
					$tick = false;

					break;
				case '3':
					// switch to plant view
					$this->viewType = 3;
					$tick = false;

					break;
				case '5':
					// save the game
					$this->save();
					$tick = false;

					break;
				case '9':
					$ss = $this->player->getScoreScreen();
					$this->mailbox->addMessageToStart($ss);
					$tick = false;

					break;
				case 'a': 
					// move the character left
				case 'A':
					// grow character to the left
					$newX = $this->player->getX();
					$newY = $this->player->getY() - 1;

					if ($newY < 0) {
						$newY = $this->level->mapWidth - 1;
					} 
					
					// dont let player move onto themselves
					if (!$this->player->onCoords($newX, $newY)) {
						if ($this->map[$newX][$newY] instanceof Rock) {
                            $tick = false;

							break;
						}

						$this->player->setY($newY);
						
						if ($c == 'a') {
							$popped = $this->player->coordPop();
						} else {
							$this->player->scoreGrown++;
						}

						// collect rain
						$wetness = $this->map[$newX][$newY]->getWetness();

						if ($wetness > 0) {
							$this->player->incrMoisture($wetness);
							$this->map[$newX][$newY]->setWetness(0);
						}

						$this->checkPlayerMoisture();

						$concentration = $this->map[$newX][$newY]->getConcentration();

						if ($concentration > 0) {
							$this->player->setLife($concentration + $this->player->getLife());
							$this->player->scoreMinerals += $concentration;
							$this->map[$newX][$newY]->setConcentration(0);
						}

						$toxicity = $this->map[$newX][$newY]->getToxicity();

						if ($toxicity > 0) {
							$this->player->setLife($this->player->getLife() - ($toxicity * $this->level->toxicityMultiplier));
							$this->player->scoreToxicWaste += $toxicity;
							$this->map[$newX][$newY]->setToxicity(0);
						}

						if ($this->player->decrLife() < 0) {
							$this->gameOver();
						}

						// move viewport to the left
						if ($this->vpY - 1 < 0) {
							$this->vpY = $this->level->mapWidth - 1;
						} else {
							$this->vpY--;
						}

                        // check if we're on a crystal square.
                        // if so, take it.
                        if ($this->map[$newX][$newY] instanceof Dirt && $this->map[$newX][$newY]->isCrystal()) {
                            // remove the crystal, and add to players inventory
                            $this->player->incrCrystals();
                            $this->map[$newX][$newY]->setCrystal(false);
                            $this->player->setLife($this->player->getLife() + $this->level->crystalHealthBonus);
                        }
					} else {
						$tick = false;
					}

					break;
				case 'c':
					$playerInWindow = false;

					// move viewport to the right
					$y = $this->player->getY() - 1;

					// count out vpWidth spaces to the right
					for ($i = 0; $i < $this->level->vpWidth; $i++) {
						$realY = $i + $this->vpY;

						if ($realY >= $this->level->mapWidth) {
							$realY = $realY - $this->level->mapWidth;
						} 
						
						if ($y == $realY) {
							$playerInWindow = true;
							break;
						}
					}

					if ($playerInWindow) {
						if ($this->vpY + 1 >= $this->level->mapWidth) {
							$this->vpY = 0;
						} else {
							$this->vpY++;
						}
					}

					$tick = false;

					break;
				case 'd':
					// move the character to the right
				case 'D':
					// grow the character to the right
					$newX = $this->player->getX();
					$newY = $this->player->getY() + 1;

					if ($newY > $this->level->mapWidth - 1) {
						$newY = 0;
					} 
					
					if (!$this->player->onCoords($newX, $newY)) {
						if ($this->map[$newX][$newY] instanceof Rock) {
                            $tick = false;
							break;
						} 

						$this->player->setY($newY);
						
						if ($c == 'd') {
							$this->player->coordPop();
						} else {
							$this->player->scoreGrown++;
						}

						// collect rain
						$wetness = $this->map[$newX][$newY]->getWetness();

						if ($wetness > 0) {
							$this->player->incrMoisture($wetness);
							$this->map[$newX][$newY]->setWetness(0);
						}

						$this->checkPlayerMoisture();

						$concentration = $this->map[$newX][$newY]->getConcentration();

						if ($concentration > 0) {
							$this->player->setLife($concentration + $this->player->getLife());
							$this->player->scoreMinerals += $concentration;
							$this->map[$newX][$newY]->setConcentration(0);
						}

						$toxicity = $this->map[$newX][$newY]->getToxicity();

						if ($toxicity > 0) {
							$this->player->setLife($this->player->getLife() - ($toxicity * $this->level->toxicityMultiplier));
							$this->player->scoreToxicWaste += $toxicity;
							$this->map[$newX][$newY]->setToxicity(0);
						}
						
						if ($this->player->decrLife() < 0) {
							$this->gameOver();
						}

						// move viewport to the right
						if ($this->vpY + 1 >= $this->level->mapWidth) {
							$this->vpY = 0;
						} else {
							$this->vpY++;
						}

                        // check if we're on a crystal square.
                        // if so, take it.
                        if ($this->map[$newX][$newY] instanceof Dirt && $this->map[$newX][$newY]->isCrystal()) {
                            // remove the crystal, and add to players inventory
                            $this->player->incrCrystals();
                            $this->map[$newX][$newY]->setCrystal(false);
                            $this->player->setLife($this->player->getLife() + $this->level->crystalHealthBonus);
                        }
					} else {
						$tick = false;
					}
					
					break;
				case 'g':
					// debug the map here
					$tick = false;

					system("stty echo");
					$x = readline("X: ");

					if (is_null($x)) {
						break;
						}

					$y = readline("Y: ");

					system("stty -echo");

					var_dump($this->map[$x][$y]);

					break;
	
				case 'h':
					// show/hide character on selected viewport
					if (in_array($this->viewType, $this->charOnScreens)) {
						// remove from the array
						for ($x = 0; $x < count($this->charOnScreens); $x++) {
							if ($this->charOnScreens[$x] == $this->viewType) {
								array_splice($this->charOnScreens, $x, 1);

								break;
							}
						}
					} else {
						$this->charOnScreens[] = $this->viewType;
					}

					break;
				case 'j':
					// show the level screen journal if we aren't already
					$curMsg = $this->mailbox->readCurrentMessage();
					$levelScreen = $this->getLevelScreen();

					if (is_array($curMsg) && $curMsg[0] == $levelScreen[0]) {
						$this->mailbox->deleteCurrentMessage();
					} else {
						$this->mailbox->addMessageToStart($this->getLevelScreen());
					}
					
					$tick = false;

					break;
				case 'm':
					// delete the current message
					$this->mailbox->deleteCurrentMessage();
					$tick = false;
					
					break;
				case 'M':
					$this->mailbox->clearMessages();
					$tick = false;

					break;
				case 'n':
					// new.  re-initializes the map
					$this->initialize();
					
					break;		
				case 'p':
					// add a new plant
					$this->addPlant();
					$tick = false;
					
					break;
				case 'q':
					// quit the game
					system("stty $this->term");

					exit;
				case 'r':
					// reverse the snake
					$this->player->reverseCoords();

					$tick = false;
			
					break;
				
				case 's':
					// move character down
				case 'S':
					// grow character down
					$newX = $this->player->getX() + 1;
					$newY = $this->player->getY();

					// don't go below the bottom
					if ($newX > $this->level->mapHeight - 1) {
						$newX = $this->level->mapHeight - 1;
					} 
					
					if (!$this->player->onCoords($newX, $newY)) {
						if ($this->map[$newX][$newY] instanceof Rock) {
                            $tick = false;
							break;
						} 

						$this->player->setX($newX);

						if ($c == 's') {
							$this->player->coordPop();
						} else {
							$this->player->scoreGrown++;
						}

						// collect rain
						$wetness = $this->map[$newX][$newY]->getWetness();

						if ($wetness > 0) {
							$this->player->incrMoisture($wetness);
							$this->map[$newX][$newY]->setWetness(0);
						}
						
						$this->checkPlayerMoisture();
						
						$concentration = $this->map[$newX][$newY]->getConcentration();

						// add any life from minerals
						if ($concentration > 0) {
							$this->player->setLife($concentration + $this->player->getLife());
							$this->player->scoreMinerals += $concentration;
							$this->map[$newX][$newY]->setConcentration(0);
						}

						$toxicity = $this->map[$newX][$newY]->getToxicity();

						if ($toxicity > 0) {
							$this->player->setLife($this->player->getLife() - ($toxicity * $this->level->toxicityMultiplier));
							$this->player->scoreToxicWaste += $toxicity;
							$this->map[$newX][$newY]->setToxicity(0);
						}
						
						// turnly upkeep tax
						if ($this->player->decrLife() < 0) {
							$this->gameOver();
						}

                        // check if we're on a crystal square.
                        // if so, take it.
                        if ($this->map[$newX][$newY] instanceof Dirt && $this->map[$newX][$newY]->isCrystal()) {
                            // remove the crystal, and add to players inventory
                            $this->player->incrCrystals();
                            $this->map[$newX][$newY]->setCrystal(false);
                            $this->player->setLife($this->player->getLife() + $this->level->crystalHealthBonus);
                        }
					} else {
						$tick = false;
					}

					break;
				case 't':
					// toggle three screen
					$this->threeScreen ? $this->threeScreen = false : $this->threeScreen = true;

					$tick = false;
					break;
				
				case 'w':
					// move character up
				case 'W':
					// grow character up
					$newX = $this->player->getX() - 1;
					$newY = $this->player->getY();

					// character cannot go into the sky
					if ($newX < $this->level->skyHeight) {
						$newX = $this->level->skyHeight;
					}

					// character cannot pass onto itself
					if (!$this->player->onCoords($newX, $newY)) {
						// character cannot go through a rock
						if ($this->map[$newX][$newY] instanceof Rock) {
							$tick = false;

							break;
						}

						$this->player->setX($newX);

						if ($c == 'w') {
							$this->player->coordPop();
						} else {
							$this->player->scoreGrown++;
						}

						// collect rain
						$wetness = $this->map[$newX][$newY]->getWetness();

						if ($wetness > 0) {
							$this->player->incrMoisture($wetness);
							$this->map[$newX][$newY]->setWetness(0);
						}

						$this->checkPlayerMoisture();
						
						// collect minerals 
						$concentration = $this->map[$newX][$newY]->getConcentration();

						if ($concentration > 0) {
							$this->player->setLife($concentration + $this->player->getLife());
							$this->player->scoreMinerals += $concentration;
							$this->map[$newX][$newY]->setConcentration(0);
						}
						
						// collect toxicity
						$toxicity = $this->map[$newX][$newY]->getToxicity();

						if ($toxicity > 0) {
							$this->player->setLife($this->player->getLife() - ($toxicity * $this->level->toxicityMultiplier));
							$this->player->scoreToxicWaste += $toxicity;
							$this->map[$newX][$newY]->setToxicity(0);
						}
						
						// upkeep tax
						if ($this->player->decrLife() < 0) {
							$this->gameOver();
						}

                        // check if we're on a crystal square.
                        // if so, take it.
                        if ($this->map[$newX][$newY] instanceof Dirt && $this->map[$newX][$newY]->isCrystal()) {
                            // remove the crystal, and add to players inventory
                            $this->player->incrCrystals();
                            $this->map[$newX][$newY]->setCrystal(false);
                            $this->player->setLife($this->player->getLife() + $this->level->crystalHealthBonus);
                        }
					} else {
						$tick = false;
					}

					break;
				case 'x':
					// shrink the player down by one
					if ($this->player->getSize() > 1) {
						$this->player->coordPop();
						$this->player->scoreShrink++;
					}

					$tick = false;

					break;
				case 'z':
					$playerInWindow = false;

					// move viewport to the left
					$y = $this->player->getY() + 1;

					// coun 40 squares
					// count out vpWidth spaces to the right
					for ($i = 0; $i < $this->level->vpWidth; $i++) {
						$realY = $i + $this->vpY;

						if ($realY >= $this->level->mapWidth) {
							$realY = $realY - $this->level->mapWidth;
						} 
						
						if ($y == $realY) {
							$playerInWindow = true;
							break;
						}
					}

					if ($playerInWindow) {
						if ($this->vpY - 1 < 0) {
							$this->vpY = $this->level->mapWidth - 1;
						} else {
							$this->vpY--;
						}
					}

					$tick = false;
					
					break;
				default:
					// do nothing here
			}

			if ($tick) {
				$this->tick();
			} else {
				// redraw the screen
				echo $this;
			}
		}
	}

	public function checkPlayerMoisture() {
		if ($this->player->getMoisture() >= ($this->player->getMoistureMax() * 2)) {
			echo "YOU GOT TOO WET AND DIED\n";

			$this->gameOver();
		} else if ($this->player->getMoisture() <= ($this->player->getMoistureMax() * -2)) {
			echo "YOU DRIED OUT AND DIED\n";

			$this->gameOver();
		}
	}

	private function tick() {
		// check for points of interest
        $this->checkPointsOfInterest();
        $this->checkObjectives();
    	$this->updateRain();
		$this->updatePlants();
		$this->updateMeteor();

		// will only add up to the maxes
		if ($this->level->enableRain) {
			$this->addRain();
		}

		if ($this->level->enableMinerals) {
			$this->addMineral();
		}

		if ($this->level->enablePlants) {
			$this->addPlant();
		}

		if ($this->level->enableCrystals) {
			$this->addCrystal();
		}

		if ($this->level->enableMeteors) {
			if ($this->meteorTicker > 0) {
				$this->meteorTicker--;
			} else {
				$this->meteorTicker = $this->level->meteorTicks;
				$this->addMeteor();
			}
		}

		// mostly for moisture, mostly
		$this->player->tick();

	    // redraw the screen
		echo $this;
	}

	/**
	 * loops through the objectives array of the level object
	 * 
	 * any objectives that are complete, will be marked as such
	 * a message sent to player
	 * and rewards taken
	 */
	public function checkObjectives() {
		$objectives = $this->level->getObjectives();

		if (!empty($objectives)) {
			for ($x = 0; $x < count($objectives); $x++) {
				$objective = $this->level->objectives[$x];

				switch (get_class($objective)) {
					case 'CollectObjective':
						$complete = false;

						switch ($objective->what) {
							case 'health':
								if ($this->player->getLife() >= $objective->amount) {
									$this->level->objectives[$x]->setCompleted();
									$complete = true;

								}
								
								break;
							case 'crystals':
								if ($this->player->getCrystals() >= $objective->amount) {
									$this->level->objectives[$x]->setCompleted();
									$complete = true;
								}			

								break;
							case 'minerals':
								if ($this->player->scoreMinerals >= $objective->amount) {
									$this->level->objectives[$x]->setCompleted();
									$complete = true;
								}

								break;
							case 'moves':
								if ($this->player->scoreMoves >= $objective->amount) {
									$this->level->objectives[$x]->setCompleted();
									$complete = true;
								}

								break;
							case 'water':
								if ($this->player->scoreWater >= $objective->amount) {
									$this->level->objectives[$x]->setCompleted();
									$complete = true;
								}

								break;
						}

						if ($complete) {
							// send a message about the completed objective
							$this->mailbox->addMessage("Objective complete! (" . $objective->getShortName() . ")");

							// get the rewards from the objects
							$this->player->addXP($objective->getRewardXP());
						}

						break;
				}        
			}
		}
	}

	public function fillMap($x1, $y1, $x2, $y2, $fillObj) {
		for ($x = $x1; $x < $x2; $x++) {
			for ($y = $y1; $y < $y2; $y++) {
				$this->map[$x][$y] = new $fillObj;

				if ($fillObj == "Dirt") {
					$this->map[$x][$y]->setToxicTicks($this->level->toxicityTicks);
				}
			}
		}	
	}

	/**
	 * add a raindrop to the sky
	 */
	private function addRain() {
		$existingRain = $this->getSkyRainCoords();
		$ys = array();

		// only allow rain up to maxRain
		if (count($existingRain) >= $this->level->maxRain) {
			return false;
		}

		// find a random unoccupied y value
		foreach ($existingRain as $raindrop) {
			$ys[] = $raindrop[1];	
		}

		// generate an unoccupied rain space
		do {
			$rainY = rand(0, count($this->map[0]) - 1);
		} while (in_array($rainY, $ys));

		$this->map[0][$rainY]->setWetness(rand($this->level->rainStrengthMin, $this->level->rainStrengthMax));
	}

	/**
	 * add a mineral to a dirt square
	 */
	private function addMineral() {
		$existingMs = $this->getMineralCoords();
	
		// only allow up to maxMinerals
		if (count($existingMs) >= $this->level->maxMinerals) {
			return false;
		}

		// add existing rocks and player coordinates
		$existingMs = array_merge($existingMs, $this->getRockCoords(), $this->player->getCoords());

		$dirtHeight = $this->level->mapHeight - $this->level->skyHeight;

		// if all squares are taken, go back
		// this is our failsafe to prevent an infinite loop in the do/while below
		if (count($existingMs) >= ($dirtHeight * $this->level->mapWidth)) {
			return false;
		}

		// find a random unoccupied x,y value
		do {
			$mX = rand($this->level->skyHeight, $this->level->mapHeight - 1);
			$mY = rand(0, $this->level->mapWidth - 1);
		} while (in_array([$mX, $mY], $existingMs));

		$this->map[$mX][$mY]->setConcentration(rand(1, 5));
	}

	private function addPlant() {
		// which dirt coords already have plants?
		$plantCoords = $this->getPlantCoords();

		if (count($plantCoords) >= $this->level->maxPlants) {
			return false;
		}

		$ys = array();

		foreach ($plantCoords as $plant) {
			$ys[] = $plant[1];
		}

		do {
			$newY = rand(0, $this->level->mapWidth - 1);
		} while (in_array($newY, $ys) || $this->map[$this->level->skyHeight][$newY] instanceof Rock);

		// create the new plan
		$plant = new Plant();

		$this->map[$this->level->skyHeight][$newY]->addPlant($plant);
	}

	public function addRock() {
		// look for a dirt square we can take
		$rockCoords = $this->getRockCoords();

		if (count($rockCoords) >= $this->level->maxRocks) {
			return false;
		}
		
		do {
			$newX = rand($this->level->skyHeight, $this->level->mapHeight - 1);
			$newY = rand(0, $this->level->mapWidth - 1);
		} while (in_array([$newX, $newY], $rockCoords));

		$rock = new Rock();

		$this->map[$newX][$newY] = $rock;
	}

	public function addCrystal() {
        // look for a dirt square we can take
        $cCoords = $this->getCrystalCoords();
        $rockCoords = $this->getRockCoords();
		$player = $this->player->getCoords();

        if (count($cCoords) >= $this->level->maxCrystals) {
            return false;
        }

        $allCoords = array_merge($cCoords, $rockCoords, $player);

        do {
            $newX = rand($this->level->mapHeight - 5, $this->level->mapHeight - 1);
            $newY = rand(0, $this->level->mapWidth - 1);
        } while(in_array([$newX, $newY], $allCoords));

        $this->map[$newX][$newY]->setCrystal(true);
    }

	public function addMeteor() {
		// only one at a time!
		if ($this->meteor && $this->meteor instanceof Meteor) {
			return false;
		}

		$dirChance = rand(0, 1);

		if ($dirChance == 0) {
			$dir = "L";
		} else {
			$dir = "R";
		}

		// add a new meteor of speed 1.
		$this->meteor = new Meteor($dir, rand($this->level->meteorSpeedMin, $this->level->meteorSpeedMax));

		// the x is 0 to start, but the Y is random
		$this->meteor->setXY(0, rand(0, $this->level->mapWidth - 1));
	}

	public function addToxicWaste() {
		$tw = $this->getToxicCoords();
		$rocks = $this->getRockCoords();
		$player = $this->player->getCoords();

		$taken = array_merge($tw, $rocks, $player);

		do {
			$newX = rand($this->level->skyHeight, $this->level->mapHeight - 1);
			$newY = rand(0, $this->level->mapWidth - 1);
		} while (in_array([$newX, $newY], $tw));

		$this->map[$newX][$newY]->setToxicity(rand($this->level->toxicityMin, $this->level->toxicityMax));
	}

	public function getDirtCoords() {
		$ret = array();

		for ($x = $this->level->skyHeight; $x < $this->level->mapHeight; $x++) {
			for ($y = 0; $y < $this->level->mapWidth; $y++) {
				if ($this->map[$x][$y] instanceof Dirt) {
					$ret[] = array($x, $y);
				}
			}
		}

		return $ret;
	}

	public function getToxicCoords() {
		$ret = array();

		for ($x = $this->level->skyHeight; $x < $this->level->mapHeight; $x++) {
			for ($y = 0; $y < $this->level->mapWidth; $y++) {
				if ($this->map[$x][$y] instanceof Dirt && $this->map[$x][$y]->isToxic()) {
					$ret[] = array($x, $y);
				}
			}
		}

		return $ret;
	}

	public function getRockCoords() {
		$ret = array();

		for ($x = $this->level->skyHeight; $x < $this->level->mapHeight; $x++) {
			for ($y = 0; $y < $this->level->mapWidth; $y++) {
				if ($this->map[$x][$y] instanceof Rock) {
					$ret[] = array($x, $y);
				}
			}
		}

		return $ret;
	}

    public function getCrystalCoords() {
        $ret = array();

        for ($x = $this->level->skyHeight; $x < $this->level->mapHeight; $x++) {
            for ($y = 0; $y < $this->level->mapWidth; $y++) {
                if ($this->map[$x][$y] instanceof Dirt && $this->map[$x][$y]->isCrystal()) {
                    $ret[] = [$x, $y];
                }
            }
        }

        return $ret;
    }

    private function getMineralCoords() {
		$ret = array();

		// scan the dirt for existing minerals
		for ($x = 0; $x < count($this->map); $x++) {
			for ($y = 0; $y < count($this->map[0]); $y++) {
				if ($this->map[$x][$y] instanceof Dirt && ($this->map[$x][$y]->hasMinerals())) {
					$ret[] = [$x, $y];
				}
			}
		}

		return $ret;	
	}

	private function getPlantCoords() {
		$ret = array();

		for ($x = 0; $x < count($this->map); $x++) {
			for ($y = 0; $y < count($this->map[0]); $y++) {
				if ($this->map[$x][$y] instanceof Dirt && $this->map[$x][$y]->hasPlant()) {
					$ret[] = [$x, $y];
				}
			}
		}

		return $ret;
	}

	private function getRainCoords() {
		$ret = array();

		for ($x = 0; $x < count($this->map); $x++) {
			for ($y = 0; $y < count($this->map[0]); $y++) {
				if ($this->map[$x][$y] instanceof Rock) {
					continue;
				} else if ($this->map[$x][$y]->isWet()) {
					$ret[] = [$x, $y];
				}
			}
		}

		return $ret;
	}

	private function getSkyRainCoords() {
		$ret = array();

		// scan the skies for existing rain
		for ($x = 0; $x < count($this->map); $x++) {
			for ($y = 0; $y < count($this->map[0]); $y++) {
				if ($this->map[$x][$y] instanceof Sky) {
					if ($this->map[$x][$y]->isWet()) {
						$ret[] = [$x, $y];
					}
				}
			}
		}

		return $ret;
	}

	private function getMeteorCoords() {
		if ($this->meteor && $this->meteor instanceof Meteor) {
			$xy = $this->meteor->getXY();

			return array_merge($this->meteor->getTailCoords(), $x[0], $x[1]);
		}
		
		return false;
	}

	private function getTotalToxicity() {
		$ret = 0;
		$toxic = $this->getToxicCoords();

		foreach ($toxic as $coord) {
			$x = $coord[0];
			$y = $coord[1];

			$ret += $this->map[$x][$y]->getToxicity();
		}

		return $ret;
	}

	private function getToxicWastePercentage() {
		$tt = $this->getTotalToxicity();
		$rocks = $this->getRockCoords();
		$dirt = $this->getDirtCoords();

		$ret = round(($tt / (count($dirt) - count($rocks))) * 100, 1);

		return $ret;
	}

	private function updateMeteor() {
		// find any meteor objects in the sky
		if (!is_null($this->meteor) && $this->meteor instanceof Meteor) {
			if (!$this->meteor->tick()) {
				$xy = $this->meteor->getXY();
				$mx = $xy[0];
				$my = $xy[1];
				$speed = $this->meteor->getSpeed();
				$weight = $this->meteor->getWeight();
				$dir = $this->meteor->getDirection();

				if ($dir == "R") {
					// we add the speed
					$newMy = $my + $speed;

					// if we are greater than the map width, loop around
					if ($newMy >= $this->level->mapWidth) {
						$newMy = $newMy - $this->level->mapWidth;
					}
				} else if ($dir == "L") {
					// we subtract the speed
					$newMy = $my - $speed;

					if ($newMy < 0) {
						$newMy = $my + $this->level->mapWidth;
					}
				}

				$newMx = $mx + 1;

				if ($newMx >= $this->level->skyHeight) {
					// we've crashed!
					$this->meteor = null;

					// put the trail in (if toxicity is enabled)
					if ($this->level->enableToxicity) {
						$target = $this->level->skyHeight + $weight;

						if ($target > $this->level->mapHeight) {
							$target = $this->level->mapHeight;
						}

						for ($x = $this->level->skyHeight; $x < $target; $x++) {
							if ($this->map[$x][$newMy] instanceof Dirt) {
								$this->map[$x][$newMy]->setToxicity(rand($this->level->toxicityMin, $this->level->toxicityMax));
								
								if ($dir == "R") {
									$newMy = $newMy + $speed;

									if ($newMy > $this->level->mapWidth) {
										$newMy = $newMy - $this->level->mapWidth;
									}
								} else {
									$newMy = $newMy - $speed;

									if ($newMy < 0) {
										$newMy = $newMy + $this->level->mapWidth;
									}
								}
							}
						}
					} 
				} else {
					// update the positioning of the meteors
					$this->meteor->setXY($newMx, $newMy);
				}
			}	
		}

		// decay the toxicity of any dirt squares
		$tcs = $this->getToxicCoords();

		foreach ($tcs as $xy) {
			$x = $xy[0];
			$y = $xy[1];

			$this->map[$x][$y]->tick();
		}
	}

	private function updateRain() {
		$raindrops = $this->getRainCoords();

		foreach ($raindrops as $raindrop) {
			$x = $raindrop[0];
			$y = $raindrop[1];

			// raindrops move straight down.
			$newX = $x + 1;
			$newY = $y;

			$wetness = $this->map[$x][$y]->getWetness();

			if ($this->map[$x][$y] instanceof Rock) {
				continue;
			} else if ($this->map[$x][$y] instanceof Sky) {
				$this->map[$x][$y]->setWetness(0);

				if ($this->player->onCoords($newX, $newY)) {
					//echo "rain is moving into player char at $newX, $newY\n";
					$this->player->setMoistureTicker($this->player->getMoistureTicks());
					$this->player->incrMoisture($wetness);
				} else if ($this->map[$newX][$newY] instanceof Dirt || $this->map[$newX][$newY] instanceof Sky) {
                    $this->map[$newX][$newY]->setWetness($wetness);
                }
			} else if ($this->map[$x][$y] instanceof Dirt) {
				$this->map[$x][$y]->setWetness(0);

				// if the player is in the newX, newY,
				// reset the moisture tick counter
				// increase moisture by 2
				// get all the coordinates of the character
				if ($this->player->onCoords($newX, $newY)) {
					// exposing variables
					//echo "Rain is moving into player char at $newX, $newY\n";
					$this->player->setMoistureTicker($this->player->getMoistureTicks());
					$this->player->incrMoisture($wetness);

					if ($this->player->getMoisture() >= ($this->player->getMoistureMax() * 2)) {
						echo "YOU GOT TOO WET AND DIED\n";

						$this->gameOver();
					} else if ($this->player->getMoisture() <= ($this->player->getMoistureMax() * -2)) {
						echo "YOU DRIED OUT AND DIED\n";

						$this->gameOver();
					}
				} else if ($this->map[$newX][$newY] instanceof Dirt) {
				    $this->map[$newX][$newY]->setWetness($wetness - 1);
                }

				// update dirts containing minerals
				$concentration = $this->map[$x][$y]->getConcentration();

				if ($concentration > 0) {
					// current space loses a mineral
					$this->map[$x][$y]->decrConcentration();

					// space below gets a mineral
                    if ($this->map[$newX][$newY] instanceof Dirt) {
					    $this->map[$newX][$newY]->incrConcentration();
                    }
				} 
			} 
		}
	}

	private function updatePlants() {
		// for every plant that doesn't currently have rain,
		// it loses 1 life.
		$plants = $this->getPlantCoords();

		// otherwise it replenishes to full
		foreach ($plants as $plant) {
			$x = $plant[0];
			$y = $plant[1];

			$wetness = $this->map[$x][$y]->getWetness();

			$this->map[$x][$y]->updatePlant($wetness);
		}
	}

	/**
	 * adds header of the specified length
	 * 
	 * with or without a title included
	 */
	public function titleHeader($l, $title = null) {
		$ret = "";

		if ($title) {
			$startTitle = $l - (strlen($title) + 1);

			for ($x = 0; $x < $startTitle; $x++) {
				if ($x == 0) {
					$ret .= json_decode('"\u250c"');
				} 

				$ret .= json_decode('"\u2501"');
			}

			$chars = str_split($title);
			
			for ($x = $startTitle; $x < $l - 1; $x++) {
				$ret .= $chars[$x - $startTitle];
			}

			$ret .= json_decode('"\u2501"');
			$ret .= json_decode('"\u2510"');
		} else {
			$ret .= json_decode('"\u250c"');

			for ($x = 1; $x < $l; $x++) {
				$ret .= json_decode('"\u2501"');
			}

			$ret .= json_decode('"\u2510"');
		}

		return $ret;
	}

    public function bottomBorder() {
        $ret = "";

        // bottom border for three screens
		$ret .= json_decode('"\u2514"');

        for ($y = 0; $y < $this->level->vpWidth; $y++) {
            $ret .= json_decode('"\u2501"');
        }

    	$ret .= json_decode('"\u2518"');
		$ret .= " ";

        return $ret;
    }

	public function moistureDisplay() {
		$m = $this->player->getMoisture();
		$mx = $this->player->getMoistureMax();

		if ($m >= 0) {
			return "$m/$mx";
		} else {
			return "$m/-$mx";
		}
	}

	public function getLevelScreen() {
		$ret = array(
			"LEVEL: " . $this->level->getName(),
			" ",
			"OBJECTIVES",
			" "
		);

		$objectives = $this->level->getObjectives();

		foreach ($objectives as $objective) {
			$tmp = "[";

			if ($objective->getCompleted()) {
				$tmp .= "X";
			} else {
				$tmp .= " ";
			}

			$tmp .= "] - " . $objective->getShortName();

			$ret[] = $tmp;
		}

		$ret[] = " ";

		return $ret;		
	}

	public function __toString() {
		// main display subroutine
		$ret = "Mapwidth: {$this->level->mapWidth} Viewport Width: " . $this->level->vpWidth . " Viewport Y: {$this->vpY} ViewType: {$this->viewType}\n";
		
		$ret .= "Player Health: " . $this->player->getLife() . " ";
		$ret .= "X: " . $this->player->getX() . " ";
		$ret .= "Y: " . $this->player->getY() . " ";
		$ret .= "Crystals: " . $this->player->getCrystals() . " ";
		$ret .= "TW%: " . $this->getToxicWastePercentage() . " ";
		$ret .= "Moisture: " . $this->moistureDisplay() . " ";
		
		$ret .= "\n";

		if ($this->threeScreen) {
			// top border for three screens)
			for ($x = 0; $x < 3; $x++) {
				switch ($x) {
					case 1:
						$txt = "mineral";
						break;
					case 2:
						$txt = "plant";
						break;
					case 0: 
						$txt = "h2o";
						break;
				}

				$ret .= $this->titleHeader($this->level->vpWidth, $txt);

				$ret .= " ";
			}

			$ret .= "\n";

            // iterate the height of the map
			for ($x = 0; $x < count($this->map); $x++) {
				// vertical left line
				$ret .= json_decode('"\u2502"');

                /** MULTILINE MESSAGING **/
                $curMsg = $this->mailbox->readCurrentMessage();
                $curChars = array();

                if (is_array($curMsg)) {
                    // calculate how many lines it is
                    $lines = count($curMsg);
                    $half = floor(($this->level->mapHeight - $lines) / 2);
                    
                    if ($x >= $half && $x < $half + $lines) {
                        // if we are within the viewport X
                        $curLine = $curMsg[$x - $half];
                        $curChars = str_split($curLine);
                    }
                }

                // iterate the width of the viewport
				// starting at vpX and going $vpWidth characters
				// when vpy hits 30, it should be 0
				for ($y = 0; $y < $this->level->vpWidth; $y++) {
					$ypos = $this->vpY + $y;

					if ($ypos >= $this->level->mapWidth) {
						$ypos = $ypos - $this->level->mapWidth;
					}

					// if the player has a position here, draw that
                    if (!empty($curChars) && $y <= 3) {
                        $ret .= " ";
                    } else if (!empty($curChars) && $y > 3) {
                        if (isset($curChars[$y - 4])) {
                            $ret .= $curChars[$y - 4];
                        } else {
                            $ret .= " ";
                        }
                    } else if ($this->player->onCoords($x, $ypos) && in_array(1, $this->charOnScreens)) {
						// if this is the x/y, get the head char
						if ($this->player->getX() == $x && $this->player->getY() == $ypos) {
							$ret .= Colors::fg_color(Config::$playerHeadColor, $this->player->getHeadChar());
						} else {
							$ret .= Colors::fg_color(Config::$playerTailColor, $this->player->getDefaultChar());
						}
					} else if ($this->map[$x][$ypos] instanceof Dirt) {
						$ret .= $this->map[$x][$ypos]->getView(1);
					} else {
						if ($this->meteor && $this->meteor instanceof Meteor) {
							if (array($x, $ypos) == $this->meteor->getXY()) {
								// head of the meteor
								$ret .= Colors::fg_color(Config::$meteorHeadColor, $this->meteor->getHeadChar());
							} else if (in_array(array($x, $ypos), $this->meteor->getTailCoords())) {
								// part of the tail
								$ret .= Colors::fg_color(Config::$meteorTailColor, $this->meteor->getTailChar());
							} else {
								$ret .= $this->map[$x][$ypos];
							}
						} else {
							$ret .= $this->map[$x][$ypos];
						}
					} 
				}

				// vertical right line
				$ret .= json_decode('"\u2502"');

				$ret .= " ";

				// second display start
				$ret .= json_decode('"\u2502"');

				// iterate the width of the viewport
				// starting at vpX and going $vpWidth characters
				// when vpy hits 30, it should be 0
				for ($y = 0; $y < $this->level->vpWidth; $y++) {
					$ypos = $this->vpY + $y;

					if ($ypos >= $this->level->mapWidth) {
						$ypos = $ypos - $this->level->mapWidth;
					}

					// if the player has a position here, draw that
					if ($this->player->onCoords($x, $ypos) && in_array(2, $this->charOnScreens)) {
						// if this is the x/y, get the head char
						if ($this->player->getX() == $x && $this->player->getY() == $ypos) {
							$ret .= Colors::fg_color(Config::$playerHeadColor, $this->player->getHeadChar());
						} else {
							$ret .= Colors::fg_color(Config::$playerTailColor, $this->player->getDefaultChar());
						}
					} else if ($this->map[$x][$ypos] instanceof Dirt) {
						$ret .= $this->map[$x][$ypos]->getView(2);
					} else {
						if ($this->meteor && $this->meteor instanceof Meteor) {
							if (array($x, $ypos) == $this->meteor->getXY()) {
								// head of the meteor
								$ret .= Colors::fg_color(Config::$meteorHeadColor, $this->meteor->getHeadChar());
							} else if (in_array(array($x, $ypos), $this->meteor->getTailCoords())) {
								// part of the tail
								$ret .= Colors::fg_color(Config::$meteorTailColor, $this->meteor->getTailChar());
							} else {
								$ret .= $this->map[$x][$ypos];
							}
						} else {
							$ret .= $this->map[$x][$ypos];
						}
					} 
				}

				// vertical right line
				$ret .= json_decode('"\u2502"');

				$ret .= " ";
				
				// third display start
				$ret .= json_decode('"\u2502"');

				// iterate the width of the viewport
				// starting at vpX and going $vpWidth characters
				// when vpy hits 30, it should be 0
				for ($y = 0; $y < $this->level->vpWidth; $y++) {
					$ypos = $this->vpY + $y;

					if ($ypos >= $this->level->mapWidth) {
						$ypos = $ypos - $this->level->mapWidth;
					}

                   if ($this->player->onCoords($x, $ypos) && in_array(3, $this->charOnScreens)) {
						// if this is the x/y, get the head char
						if ($this->player->getX() == $x && $this->player->getY() == $ypos) {
							$ret .= Colors::fg_color(Config::$playerHeadColor, $this->player->getHeadChar());
						} else {
							$ret .= Colors::fg_color(Config::$playerTailColor, $this->player->getDefaultChar());
						}
					} else if ($this->map[$x][$ypos] instanceof Dirt) {
						$ret .= $this->map[$x][$ypos]->getView(3);
					} else {
						if ($this->meteor && $this->meteor instanceof Meteor) {
							if (array($x, $ypos) == $this->meteor->getXY()) {
								// head of the meteor
								$ret .= Colors::fg_color(Config::$meteorHeadColor, $this->meteor->getHeadChar());
							} else if (in_array(array($x, $ypos), $this->meteor->getTailCoords())) {
								// part of the tail
								$ret .= Colors::fg_color(Config::$meteorTailColor, $this->meteor->getTailChar());
							} else {
								$ret .= $this->map[$x][$ypos];
							}
						} else {
							$ret .= $this->map[$x][$ypos];
						}
					} 
				}

				// vertical right line
				$ret .= json_decode('"\u2502"');

				$ret .= "\n";
			}

			// bottom border for three screens
			for ($x = 0; $x < 3; $x++) {
				$ret .= $this->bottomBorder();
			}

            // the little hang down thing for messages
			$ret .= "\n";
            $ret .= " " . json_decode('"\u2517"');

			// message area
			$curMessage = $this->mailbox->readCurrentMessage();

            if (!is_array($curMessage)) {
				if ($curMessage) {
			    	$ret .= " " . $curMessage;
				} else {
					$ret .= " EOF";
				}
            }
		} else {
			/**
			 * SINGLE SCREEN VIEW STARTS HERE!
			 */
			switch ($this->viewType) {
				case 2:
					$txt = "mineral";
					break;
				case 3:
					$txt = "plant";
					break;
				case 1: 
					$txt = "h2o";
					break;
			}

			$ret .= $this->titleHeader($this->level->vpWidth, $txt);

			$ret .= "\n";

			// all the map lines
			for ($x = 0; $x < count($this->map); $x++) {
				// vertical left line
				$ret .= json_decode('"\u2502"');

                /** MULTILINE MESSAGING **/
                $curMsg = $this->mailbox->readCurrentMessage();
                $curChars = array();
 
                if (is_array($curMsg)) {
                    // calculate how many lines it is
                    $lines = count($curMsg);
                    $half = floor(($this->level->mapHeight - $lines) / 2);
                    
                    if ($x >= $half && $x < $half + $lines) {
                        // if we are within the viewport X
                        $curLine = $curMsg[$x - $half];
                        $curChars = str_split($curLine);
                    }
                }

				// iterate the width of the viewport
				// starting at vpX and going $vpWidth characters
				// when vpy hits 30, it should be 0
				for ($y = 0; $y < $this->level->vpWidth; $y++) {
					$ypos = $this->vpY + $y;

					if ($ypos >= $this->level->mapWidth) {
						$ypos = $ypos - $this->level->mapWidth;
					}

                     // if the player has a position here, draw that
                     if (!empty($curChars) && $y <= 3) {
                        $ret .= " ";
                    } else if (!empty($curChars) && $y > 3) {
                        if (isset($curChars[$y - 4])) {
                            $ret .= $curChars[$y - 4];
                        } else {
                            $ret .= " ";
                        }
					} else if ($this->player->onCoords($x, $ypos) && in_array($this->viewType, $this->charOnScreens)) {
						// if this is the x/y, get the head char
						if ($this->player->getX() == $x && $this->player->getY() == $ypos) {
							$ret .= Colors::fg_color(Config::$playerHeadColor, $this->player->getHeadChar());
						} else {
							$ret .= Colors::fg_color(Config::$playerTailColor, $this->player->getDefaultChar());
						}
					} else if ($this->map[$x][$ypos] instanceof Dirt) {
						$ret .= $this->map[$x][$ypos]->getView($this->viewType);
					} else {
						if ($this->meteor && $this->meteor instanceof Meteor) {
							if (array($x, $ypos) == $this->meteor->getXY()) {
								// head of the meteor
								$ret .= Colors::fg_color(Config::$meteorHeadColor, $this->meteor->getHeadChar());
							} else if (in_array(array($x, $ypos), $this->meteor->getTailCoords())) {
								// part of the tail
								$ret .= Colors::fg_color(Config::$meteorTailColor, $this->meteor->getTailChar());
							} else {
								$ret .= $this->map[$x][$ypos];
							}
						} else {
							$ret .= $this->map[$x][$ypos];
						}
					} 
				}

				// vertical right line
				$ret .= json_decode('"\u2502"');

				$ret .= "\n";
			}

			// bottom of the viewport
			$ret .= $this->bottomBorder();

            // little hangdown thing for messages
			$ret .= "\n";         
            $ret .= " " . json_decode('"\u2517"');

			// message area
			$currentMessage = $this->mailbox->readCurrentMessage();

            if (!is_array($currentMessage)) {
				if ($currentMessage) {
			    	$ret .= " " . $currentMessage;
				} else {
					$ret .= " EOF";
				}
            } 
		}

		$ret .= "\n";

		return $ret;
	}
}

