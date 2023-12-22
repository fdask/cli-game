<?php
class Map {
	private $map;
	private $term;

	// holds the player object
	public $player;

	// how much sky on the map
	public $skyHeight;

	// how deep is the dirt
	public $groundHeight;

	// how wide is the viewport
	public $vpWidth;

	// the starting y position of the viewport
	public $vpY;

	// how wide is the total map	
	public $mapWidth;

	// how high is the map (hehehe)
	public $mapHeight;

	// an array holding the integers 1-3 
	// to indicate whether char shows on
	// that screen
	public $charOnScreens;

	// which view we want to show the user
	// 1 - shows the rain/wetness
	// 2 - mineral view
	// 3 - plant view
	public $viewType;

	// how many raindrops allowed on the map at once
	public $maxRain;

	// how many mineral spaces to allow on the map at once
	public $maxMinerals;

	// how many plants to allow on the map at once
	public $maxPlants;

	// whether we have enabled three screen view, or not.   toggles
	private $threeScreen;

	public $mailbox;

	public function __construct($skyHeight, $groundHeight, $vpWidth, $mapWidth, $maxRain, $maxMinerals, $player) {
		$this->skyHeight = $skyHeight;
		$this->groundHeight = $groundHeight;
		$this->mapHeight = $groundHeight + $skyHeight;
		$this->vpWidth = $vpWidth;
		$this->mapWidth = $mapWidth;
		$this->maxRain = $maxRain;
		$this->maxMinerals = $maxMinerals;
		$this->vpY = 0;
		$this->threeScreen = false;
		$this->mailbox = new Mailbox();
		$this->mailbox->addInstructions();
		$this->charOnScreens = array(1, 2, 3);
		
		$this->player = $player;

		// magic numbers
		$this->maxRocks = 10;
		$this->maxPlants = 5;

		$this->initialize();
		$this->gameLoop();
	}

	public function initialize() {
		// default to the rain view
		$this->viewType = 1;

		// initialize the map array
		$this->initializeMap();

		// fill in the sky
		$this->fillMap(0, 0, $this->skyHeight, $this->mapWidth, "Sky");

		// add in random rains up to maxRain
		for ($x = 0; $x < $this->maxRain; $x++) {
			$this->addRain();
		}

		// fill in the dirt
		$this->fillMap($this->skyHeight, 0, $this->mapHeight, $this->mapWidth, "Dirt");		

		// add in random minerals up to maxMinerals
		for ($x = 0; $x < $this->maxMinerals; $x++) {
			$this->addMineral();
		}

		// add in plants up to maxPlants
		for ($x = 0; $x < $this->maxPlants; $x++) {
			$this->addPlant();
		}

		// add in the rocks
		for ($x = 0; $x < $this->maxRocks; $x++) {
			$this->addRock();
		}
	}

	private function initializeMap() {
		// fills the map array out with falses
		$this->map = array();

		for ($x = 0; $x < ($this->skyHeight + $this->groundHeight); $x++) {
			for ($y = 0; $y < $this->mapWidth; $y++) {
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

		$this->tick();

		// keypress handler
		while ($c = fread(STDIN, 1)) {
			$tick = true;

			switch ($c) {
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
				case 'z':
					// move viewport to the left
					if ($this->vpY - 1 < 0) {
						$this->vpY = $this->mapWidth - 1;
					} else {
						$this->vpY--;
					}

					$tick = false;

					break;
				case 'a': 
					// move the character left
				case 'A':
					// grow character to the left
					$newX = $this->player->getX();
					$newY = $this->player->getY() - 1;

					if ($newY < 0) {
						$newY = $this->mapWidth - 1;
					} 
					
					if (!$this->player->onCoords($newX, $newY)) {
						if ($this->map[$newX][$newY] instanceof Rock) {
							break;
						}

						$this->player->setY($newY);
						
						if ($c == 'a') {
							$this->player->coordPop();
						}

						$concentration = $this->map[$newX][$newY]->getConcentration();

						if ($concentration > 0) {
							$this->player->setLife($concentration + $this->player->getLife());
							$this->map[$newX][$newY]->setConcentration(0);
						}

						if ($this->player->decrLife() < 0) {
							$this->gameOver();
						}

						// move viewport to the left
						if ($this->vpY - 1 < 0) {
							$this->vpY = $this->mapWidth - 1;
						} else {
							$this->vpY--;
						}
					}

					break;
				case 'c':
					// move viewport to the right
					if ($this->vpY + 1 >= $this->mapWidth) {
						$this->vpY = 0;
					} else {
						$this->vpY++;
					}

					$tick = false;

					break;
				case 'd':
					// move the character to the right
				case 'D':
					// grow the character to the right
					$newX = $this->player->getX();
					$newY = $this->player->getY() + 1;

					if ($newY > $this->mapWidth - 1) {
						$newY = 0;
					} 
					
					if (!$this->player->onCoords($newX, $newY)) {
						if ($this->map[$newX][$newY] instanceof Rock) {
							break;
						} 

						$this->player->setY($newY);
						
						if ($c == 'd') {
							$this->player->coordPop();
						}

						$concentration = $this->map[$newX][$newY]->getConcentration();

						if ($concentration > 0) {
							$this->player->setLife($concentration + $this->player->getLife());
							$this->map[$newX][$newY]->setConcentration(0);
						}

						if ($this->player->decrLife() < 0) {
							$this->gameOver();
						}

						// move viewport to the right
						if ($this->vpY + 1 >= $this->mapWidth) {
							$this->vpY = 0;
						} else {
							$this->vpY++;
						}
					}
					
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
					// add a raindrop up to maxRain
					$this->addRain();
					$tick = false;
			
					break;
				
				case 's':
					// move character down
				case 'S':
					// grow character down
					$newX = $this->player->getX() + 1;
					$newY = $this->player->getY();

					if ($newX > ($this->skyHeight + $this->groundHeight) - 1) {
						$newX = $this->skyHeight + $this->groundHeight - 1;
					} 
					
					if (!$this->player->onCoords($newX, $newY)) {
						if ($this->map[$newX][$newY] instanceof Rock) {
							break;
						} 

						$this->player->setX($newX);

						if ($c == 's') {
							$this->player->coordPop();
						}

						$concentration = $this->map[$newX][$newY]->getConcentration();

						// add any life from minerals
						if ($concentration > 0) {
							$this->player->setLife($concentration + $this->player->getLife());
							$this->map[$newX][$newY]->setConcentration(0);
						}

						// turnly upkeep tax
						if ($this->player->decrLife() < 0) {
							$this->gameOver();
						}
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

					if ($newX < $this->skyHeight) {
						$newX = $this->skyHeight;
					}

					if (!$this->player->onCoords($newX, $newY)) {
						if ($this->map[$newX][$newY] instanceof Rock) {
							break;
						}

						$this->player->setX($newX);

						if ($c == 'w') {
							$this->player->coordPop();
						}

						$concentration = $this->map[$newX][$newY]->getConcentration();

						if ($concentration > 0) {
							$this->player->setLife($concentration + $this->player->getLife());
							$this->map[$newX][$newY]->setConcentration(0);
						}

						if ($this->player->decrLife() < 0) {
							$this->gameOver();
						}
					}

					break;
				case 'x':
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

	private function tick() {
		// redraw the screen
		echo $this;
		
		// we do this backwardes, otherwise stuff that moves into dirt from the sky,
		// will move twice in this tick
		$this->updateRain();
		$this->updatePlants();

		// will only add up to the maxes
		$this->addRain();
		$this->addMineral();
		$this->addPlant();
	}

	public function fillMap($x1, $y1, $x2, $y2, $fillObj) {
		for ($x = $x1; $x < $x2; $x++) {
			for ($y = $y1; $y < $y2; $y++) {
				$this->map[$x][$y] = new $fillObj;
			}
		}	
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

	private function addRain() {
		$existingRain = $this->getSkyRainCoords();
		$ys = array();

		// only allow rain up to maxRain
		if (count($existingRain) >= $this->maxRain) {
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

		$this->map[0][$rainY]->setWetness(rand(1, 3));
	}

	private function addMineral() {
		$existingMs = $this->getMineralCoords();
	
		// only allow up to maxMinerals
		if (count($existingMs) >= $this->maxMinerals) {
			return false;
		}

		// find a random unoccupied x,y value
		do {
			$mX = rand($this->skyHeight, $this->skyHeight + $this->groundHeight - 1);
			$mY = rand(0, $this->mapWidth - 1);
		} while (in_array([$mX, $mY], $existingMs));

		$this->map[$mX][$mY]->setConcentration(rand(1, 5));
	}

	private function addPlant() {
		// which dirt coords already have plants?
		$plantCoords = $this->getPlantCoords();

		if (count($plantCoords) >= $this->maxPlants) {
			return false;
		}

		$ys = array();

		foreach ($plantCoords as $plant) {
			$ys[] = $plant[1];
		}

		do {
			$newY = rand(0, $this->mapWidth - 1);
		} while (in_array($newY, $ys) || $this->map[$this->skyHeight][$newY] instanceof Rock);

		// create the new plan
		$plant = new Plant();

		$this->map[$this->skyHeight][$newY]->addPlant($plant);
	}

	public function getRockCoords() {
		$ret = array();

		for ($x = $this->skyHeight; $x < $this->mapHeight; $x++) {
			for ($y = 0; $y < $this->mapWidth; $y++) {
				if ($this->map[$x][$y] instanceof Rock) {
					$ret[] = array($x, $y);
				}
			}
		}

		return $ret;
	}

	public function addRock() {
		// look for a dirt square we can take
		$rockCoords = $this->getRockCoords();

		if (count($rockCoords) >= $this->maxRocks) {
			return false;
		}
		
		do {
			$newX = rand($this->skyHeight, $this->mapHeight - 1);
			$newY = rand(0, $this->vpWidth - 1);
		} while (in_array([$newX, $newY], $rockCoords));

		$rock = new Rock();

		$this->map[$newX][$newY] = $rock;
	}

	private function updateRain() {
		$raindrops = $this->getRainCoords();

		foreach ($raindrops as $raindrop) {
			$x = $raindrop[0];
			$y = $raindrop[1];

			$newX = $x + 1;
			$newY = $y;

			$wetness = $this->map[$x][$y]->getWetness();

			if ($this->map[$x][$y] instanceof Rock) {
				continue;
			} else if ($this->map[$x][$y] instanceof Sky) {
				$this->map[$x][$y]->setWetness(0);
                
				if ($this->map[$newX][$newY] instanceof Dirt || $this->map[$newX][$newY] instanceof Sky) {
                    $this->map[$newX][$newY]->setWetness($wetness);
                }
			} else if ($this->map[$x][$y] instanceof Dirt) {
				$this->map[$x][$y]->setWetness(0);

                if ($this->map[$newX][$newY] instanceof Dirt) {
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

	public function __toString() {
		// main display subroutine
		$ret = "Mapwidth: {$this->mapWidth} Viewport Width: {$this->vpWidth} Viewport Y: {$this->vpY} ViewType: {$this->viewType}\n";
		$ret .= "Player Health: " . $this->player->getLife() . " X: " . $this->player->getX() . " Y: " . $this->player->getY() . "\n";

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

				$ret .= $this->titleHeader($this->vpWidth, $txt);

				$ret .= " ";
			}

			$ret .= "\n";

			// iterate the height of the map
			for ($x = 0; $x < count($this->map); $x++) {
				// vertical left line
				$ret .= json_decode('"\u2502"');

				// iterate the width of the viewport
				// starting at vpX and going $vpWidth characters
				// when vpy hits 30, it should be 0
				for ($y = 0; $y < $this->vpWidth; $y++) {
					$ypos = $this->vpY + $y;

					if ($ypos >= $this->mapWidth) {
						$ypos = $ypos - $this->mapWidth;
					}

					// if the player has a position here, draw that
					if ($this->player->onCoords($x, $ypos) && in_array(1, $this->charOnScreens)) {
						$ret .= "*";
					} else if ($this->map[$x][$ypos] instanceof Dirt) {
						$ret .= $this->map[$x][$ypos]->getView(1);
					} else {
						$ret .= $this->map[$x][$ypos];
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
				for ($y = 0; $y < $this->vpWidth; $y++) {
					$ypos = $this->vpY + $y;

					if ($ypos >= $this->mapWidth) {
						$ypos = $ypos - $this->mapWidth;
					}

					// if the player has a position here, draw that
					if ($this->player->onCoords($x, $ypos) && in_array(2, $this->charOnScreens)) {
						$ret .= "*";
					} else if ($this->map[$x][$ypos] instanceof Dirt) {
						$ret .= $this->map[$x][$ypos]->getView(2);
					} else {
						$ret .= $this->map[$x][$ypos];
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
				for ($y = 0; $y < $this->vpWidth; $y++) {
					$ypos = $this->vpY + $y;

					if ($ypos >= $this->mapWidth) {
						$ypos = $ypos - $this->mapWidth;
					}

					// if the player has a position here, draw that
					if ($this->player->onCoords($x, $ypos) && in_array(3, $this->charOnScreens)) {
						$ret .= "*";
					} else if ($this->map[$x][$ypos] instanceof Dirt) {
						$ret .= $this->map[$x][$ypos]->getView(3);
					} else {
						$ret .= $this->map[$x][$ypos];
					}
				}

				// vertical right line
				$ret .= json_decode('"\u2502"');

				$ret .= "\n";
			}

			// bottom border for three screens
			for ($x = 0; $x < 3; $x++) {
				$ret .= json_decode('"\u2514"');

				for ($y = 0; $y < $this->vpWidth; $y++) {
					$ret .= json_decode('"\u2501"');
				}

				$ret .= json_decode('"\u2518"');
				$ret .= " ";
			}

			$ret .= "\n";

			// message area
			$ret .= " " . json_decode('"\u2517"') . " " . $this->mailbox->readCurrentMessage();
		} else {
			// single screen
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

			$ret .= $this->titleHeader($this->vpWidth, $txt);

			$ret .= "\n";

			// all the map lines
			for ($x = 0; $x < count($this->map); $x++) {
				// vertical left line
				$ret .= json_decode('"\u2502"');

				// iterate the width of the viewport
				// starting at vpX and going $vpWidth characters
				// when vpy hits 30, it should be 0
				for ($y = 0; $y < $this->vpWidth; $y++) {
					$ypos = $this->vpY + $y;

					if ($ypos >= $this->mapWidth) {
						$ypos = $ypos - $this->mapWidth;
					}

					// if the player has a position here, draw that
					if ($this->player->onCoords($x, $ypos) && in_array($this->viewType, $this->charOnScreens)) {
						$ret .= "*";
					} else if ($this->map[$x][$ypos] instanceof Dirt) {
						$ret .= $this->map[$x][$ypos]->getView($this->viewType);
					} else {
						$ret .= $this->map[$x][$ypos];
					}
				}

				// vertical right line
				$ret .= json_decode('"\u2502"');

				$ret .= "\n";
			}

			// bottom of the viewport
			$ret .= json_decode('"\u2514"');

			for ($y = 0; $y < $this->vpWidth; $y++) {
				$ret .= json_decode('"\u2501"');
			}

			$ret .= json_decode('"\u2518"');

			$ret .= "\n";

			// message area
			$ret .= " " . json_decode('"\u2517"') . " " . $this->mailbox->readCurrentMessage();
		}

		$ret .= "\n";

		return $ret;
	}
}

