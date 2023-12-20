#!/usr/bin/php
<?php
// define some system variables
$skyHeight = 10;
$groundHeight = 15;
$vpWidth = 40;
$mapWidth = 50;
$maxRain = 3;
$maxMinerals = 15;

$map = new Map($skyHeight, $groundHeight, $vpWidth, $mapWidth, $maxRain, $maxMinerals);
echo $map;

class Map {
	private $map;
	private $term;

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

	// how high is the map
	public $mapHeight;

	// which view we want to show the user
	// 1 - shows the rain/wetness
	// 2 - mineral view
	public $viewType;

	// how many raindrops allowed on the map at once
	public $maxRain;

	// how many mineral spaces to allow on the map at once
	public $maxMinerals;

	// how many plants to allow on the map at once
	public $maxPlants;

	private $threeScreen;

	public function __construct($skyHeight, $groundHeight, $vpWidth, $mapWidth, $maxRain, $maxMinerals) {
		$this->skyHeight = $skyHeight;
		$this->groundHeight = $groundHeight;
		$this->mapHeight = $groundHeight + $skyHeight;
		$this->vpWidth = $vpWidth;
		$this->mapWidth = $mapWidth;
		$this->maxRain = $maxRain;
		$this->maxMinerals = $maxMinerals;
		$this->vpY = 0;
		$this->threeScreen = false;

		// magic number
		$this->maxPlants = 5;

		// default to the rain view
		$this->viewType = 1;

		// initialize the map array
		$this->initializeMap();

		// fill in the sky
		$this->fillMap(0, 0, $this->skyHeight, $this->mapWidth, "Sky");

		// fill in the dirt
		$this->fillMap($this->skyHeight, 0, $this->mapHeight, $this->mapWidth, "Dirt");
		
		$this->gameLoop();
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
				case 'a':
					// move viewport to the left
					if ($this->vpY - 1 < 0) {
						$this->vpY = $this->mapWidth - 1;
					} else {
						$this->vpY--;
					}

					$tick = false;

					break;
				case 'd':
					// move viewport to the right
					if ($this->vpY + 1 >= $this->mapWidth) {
						$this->vpY = 0;
					} else {
						$this->vpY++;
					}

					$tick = false;

					break;
				case 'm':
					// add a mineral
					$this->addMineral();
					$tick = false;

					break;
				case 'n':
					// new.  re-initializes the map

					// default to the rain view
					$this->viewType = 1;

					// initialize the map array
					$this->initializeMap();

					// fill in the sky
					$this->fillMap(0, 0, $this->skyHeight, $this->mapWidth, "Sky");

					// fill in the dirt
					$this->fillMap($this->skyHeight, 0, $this->mapHeight, $this->mapWidth, "Dirt");
			
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
				case 't':
					// toggle three screen
					$this->threeScreen ? $this->threeScreen = false : $this->threeScreen = true;

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
		// we do this backwardes, otherwise stuff that moves into dirt from the sky,
		// will move twice in this tick
		$this->updateRain();
		echo "Calling updatePlants from tick\n";
		$this->updatePlants();

		echo $this;
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

	private function fillMap($x1, $y1, $x2, $y2, $fillObj) {
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
					if ($this->map[$x][$y]->wetness > 0) {
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

		$this->map[0][$rainY]->wetness = rand(1, 3);
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

		$this->map[$mX][$mY]->concentration = rand(1, 5);
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
			$newY = rand(0, $this->mapWidth);
		} while (in_array($newY, $ys));

		// create the new plan
		$plant = new Plant();

		$this->map[$this->skyHeight][$newY]->addPlant($plant);
	}

	private function updateRain() {
		$raindrops = $this->getRainCoords();

		foreach ($raindrops as $raindrop) {
			$x = $raindrop[0];
			$y = $raindrop[1];

			$newX = $x + 1;
			$newY = $y;

			$wetness = $this->map[$x][$y]->wetness;

			if ($this->map[$x][$y] instanceof Sky) {
				$this->map[$x][$y]->wetness = 0;
				$this->map[$newX][$newY]->wetness = $wetness;
			} else if ($this->map[$x][$y] instanceof Dirt) {
				$this->map[$x][$y]->wetness = 0;
				$this->map[$newX][$newY]->wetness = $wetness - 1;

				if ($this->map[$x][$y]->concentration > 0) {
					$this->map[$x][$y]->concentration--;
					$this->map[$newX][$newY]->concentration++;
				} 
			} 
		}
	}

	private function updatePlants() {
		echo "in the updatePlants in same class (map)";
		// for every plant that doesn't currently have rain,
		// it loses 1 life.
		$plants = $this->getPlantCoords();

		// otherwise it replenishes to full
		foreach ($plants as $plant) {
			$x = $plant[0];
			$y = $plant[1];

			$wetness = $this->map[$x][$y]->getWetness();

			echo "Calling update plant ($x, $y) - $wetness\n";
			$this->map[$x][$y]->updatePlant($wetness);
		}
	}

	private function getMineralCoords() {
		$ret = array();

		// scan the dirt for existing minerals
		for ($x = 0; $x < count($this->map); $x++) {
			for ($y = 0; $y < count($this->map[0]); $y++) {
				if ($this->map[$x][$y] instanceof Dirt && ($this->map[$x][$y]->concentration > 0)) {
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
				if ($this->map[$x][$y]->wetness > 0) {
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

	public function __toString() {
		// main display subroutine
		$ret = "Mapwidth: {$this->mapWidth} Viewport Width: {$this->vpWidth} Viewport Y: {$this->vpY} ViewType: {$this->viewType}\n";

		if ($this->threeScreen) {
			for ($x = 0; $x < 3; $x++) {
				// top left corner
				$ret .= json_decode('"\u250c"');

				// horitonztal top line
				for ($y = 0; $y < $this->vpWidth; $y++) {
					$ret .= json_decode('"\u2501"');
				}

				// top right corner
				$ret .= json_decode('"\u2510"');

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

					if ($this->map[$x][$ypos] instanceof Dirt) {
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

					if ($this->map[$x][$ypos] instanceof Dirt) {
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

					if ($this->map[$x][$ypos] instanceof Dirt) {
						$ret .= $this->map[$x][$ypos]->getView(3);
					} else {
						$ret .= $this->map[$x][$ypos];
					}
				}

				// vertical right line
				$ret .= json_decode('"\u2502"');

				$ret .= "\n";
			}

			for ($x = 0; $x < 3; $x++) {
				$ret .= json_decode('"\u2514"');

				for ($y = 0; $y < $this->vpWidth; $y++) {
					$ret .= json_decode('"\u2501"');
				}

				$ret .= json_decode('"\u2518"');
				$ret .= " ";
			}
		} else {
			// single screen
			
			// upper left corner
			$ret .= json_decode('"\u250c"');

			// horitonztal top line
			for ($y = 0; $y < $this->vpWidth; $y++) {
				$ret .= json_decode('"\u2501"');
			}

			// top right corner
			$ret .= json_decode('"\u2510"') . "\n";

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

					if ($this->map[$x][$ypos] instanceof Dirt) {
						$ret .= $this->map[$x][$ypos]->getView($this->viewType);
					} else {
						$ret .= $this->map[$x][$ypos];
					}
				}

				// vertical right line
				$ret .= json_decode('"\u2502"');

				$ret .= "\n";
			}

			$ret .= json_decode('"\u2514"');

			for ($y = 0; $y < $this->vpWidth; $y++) {
				$ret .= json_decode('"\u2501"');
			}

			$ret .= json_decode('"\u2518"');
			$ret .= " ";
		}

		$ret .= "\n";

		return $ret;
	}
}

class Dirt {
	// integer representing how much moisture is in the dirt
	public $wetness;

	// integer representing the concentration of minerals
	public $concentration;

	// the single ascii character dirt shows up on the map as
	public $defaultChar;

	// if we are topsoil and have a plant, here it is
	public $plant;

	public function __construct() {
		$this->wetness = 0;
		$this->concentration = 0;
		$this->defaultChar = ".";
		$this->plant = null;
	}

	public function updatePlant() {
		echo "in the dirt updatePlant\n";

		if (!empty($this->plant) && $this->wetness) {
			echo "we have wetness, full life!\n";
			$this->plant->fullLife();
		} else if (!empty($this->plant)) {
			if ($this->plant->getLife() == 0) {
				$maxLife = $this->plant->maxLife;

				// kill the plant
				$this->plant = null;

				// add a mineral
				$this->concentration += $maxLife;
			} else {
				echo "decrementing plants life\n";
				$this->plant->decrLife();
			}
		} else {
			echo "We have no plants\n";
			print_r($this->plant);
		}
	}

	public function addPlant($plant) {
		if (empty($this->plant)) {
			$this->plant = $plant;

			return true;
		}

		return false;
	}

	public function removePlant() {
		$this->plant = null;
	}

	public function hasPlant() {
		if (!empty($this->plant)) {
			return true;
		}

		return false;
	}
	public function hasRain() {
		return $this->wetness > 0;
	}

	public function hasMinerals() {
		return $this->concentration > 0;
	}

	public function getView($viewType = 1) {
		// wetness view
		if ($viewType == 1) {
			if ($this->wetness > 0) { 
				if ($this->wetness > 9) {
					return "9";
				} else {
					return "{$this->wetness}";
				}
			} else {
				return $this->defaultChar;
			}
		} else if ($viewType == 2) {
			// mineral view
			if ($this->concentration > 0) {
				if ($this->concentration > 9) {
					return "9";
				} else {
					return "{$this->concentration}";
				}
			} else {
				return $this->defaultChar;
			}
		} else if ($viewType == 3) {
			// plant matter view
			if (!empty($this->plant)) {
				return "{$this->plant}";
			} else {
				return $this->defaultChar;
			}
		}
	}

	public function setWetness($wetness) {
		$this->wetness = $wetness;
	}

	public function getWetness() {
		return $this->wetness;
	}

	public function setConcentration($concentration) {
		$this->concentration = $concentration;
	}

	public function __toString() {
		return $this->getView(1);
	}
}

class Sky {
	// things in this region of sky are added here
	public $wetness;

	public $defaultChar;

	public function __construct() {
		$this->wetness = 0;
		$this->defaultChar = "~";
	}

	public function __toString() {
		if ($this->wetness > 0) {
			return "|";
		} else {
			return "~";	
		}
	}
}

class Plant {
	/**
	 * 	the lifecycle of a plant goes from
	 *  2 - plant
	 *  4 - dead plant
	 */
	public $stage;

	// turns without water we can live
	public $life;
	public $maxLife;

	public function __construct() {
		$this->life = rand(5, 9);
		$this->maxLife = $this->life;
		$this->stage = 2;
	}

	public function updatePlant($water) {
		// this is a plant internal tick()
		// check if we've had water recently
		if (is_null($water)) {
			$this->life = $this->life - 1;

			if ($this->life <= 0) {
				// character is dead
				$this->stage = 4;
			} 
		}
	}

	public function decrLife() {
		$this->life = $this->life - 1;

		if ($this->life <= 0) {
			$this->stage = 4;
		}
	}

	public function getStage() {
		return $this->stage;
	}

	public function getLife() {
		return $this->life;
	}

	public function fullLife() {
		$this->life = $this->maxLife;
	}

	public function __toString() {
		if ($this->stage == 2) {
			if ($this->life < 10) {
				return "{$this->life}";
			} else {
				return "9";
			}
		} else if ($this->stage == 4) {
			return "X";
		}
	}
}
