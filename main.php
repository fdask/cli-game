#!/usr/bin/php
<?php
// define some system variables
$skyHeight = 5;
$groundHeight = 5;
$vpWidth = 1;
$mapWidth = 1;
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

	public function __construct($skyHeight, $groundHeight, $vpWidth, $mapWidth, $maxRain, $maxMinerals) {
		$this->skyHeight = $skyHeight;
		$this->groundHeight = $groundHeight;
		$this->mapHeight = $groundHeight + $skyHeight;
		$this->vpWidth = $vpWidth;
		$this->mapWidth = $mapWidth;
		$this->maxRain = $maxRain;
		$this->maxMinerals = $maxMinerals;
		$this->vpY = 0;

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
				case 'q':
					// quit the game
					system("stty $this->term");

					exit;
				case 'r':
					// add a raindrop up to maxRain
					$this->addRain();
					$tick = false;
			
					break;
				case 'x':
					// debug the map here
					$tick = false;

					system("stty echo");
					$x = readline("X: ");
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

	/*
	private function getDirtRainCoords() {
		$ret = array();

		// scan the skies for existing rain
		for ($x = 0; $x < count($this->map); $x++) {
			for ($y = 0; $y < count($this->map[0]); $y++) {
				if ($this->map[$x][$y] instanceof Dirt) {
					$containedObjs = $this->map[$x][$y]->getContains();

					foreach ($containedObjs as $containedObj) {
						if ($containedObj instanceof Rain) {
							$ret[] = [$x, $y];
						}

						break;
					}
				}
			}
		}

		return $ret;
	}
	*/

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

	/* every tick
	private function updateDirt() {
		echo "in the updateDirt() method\n";
		// we go through and move anything that needs moving

		// water loses 1, and trickles down
		// if a water happens, the mineral leaves 1, and takes the rest down
		$wetnesses = $this->getDirtRainCoords();

		// decrease 1 from the rain, and pass it on to the next guy
		echo "We found " . count($wetnesses) . " wet squares (i.e. dirt containing water)\n";

		foreach ($wetnesses as $wetness) {
			$x = $wetness[0];
			$y = $wetness[1];
			echo "found wetness at $x, $y\n";
			$d = $this->map[$x][$y];

			var_dump($d->getContains());
			// everything we want to keep is here
			$newContains = array();

			foreach ($d->getContains() as $containedObj) {
				echo "in the contains loop\n";
				if ($containedObj instanceof Rain) {
					$containedObj->decrSize();

					if ($containedObj->getSize() > 0 && ($wetness[0] + 1 < $this->mapHeight)) {
						// if the tile below us is a dirt,
						// move the water there
						echo "Moving water down 1\n";
						$this->map[$wetness[0] + 1][$wetness[1]]->addContains($containedObj);
					}
				} else {
					echo "Object is not rain.\n";
					var_dump($containedObj);
					// add the non rain object back to the contains
					$newContains[] = $containedObj;
				}
			}		

			$this->map[$wetness[0]][$wetness[1]]->setContains($newContains);
		}

		echo "Outside the wetness loop\n";
	}
	*/

	public function __toString() {
		// main display subroutine
		$ret = "Mapwidth: {$this->mapWidth} Viewport Width: {$this->vpWidth} Viewport Y: {$this->vpY} ViewType: {$this->viewType}\n";

		// top left corner
		$ret .= json_decode('"\u250c"');

		// horitonztal top line
		for ($x = 0; $x < $this->vpWidth; $x++) {
			$ret .= json_decode('"\u2501"');
		}

		// top right corner
		$ret .= json_decode('"\u2510"') . "\n";

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

		for ($x = 0; $x < $this->vpWidth; $x++) {
			$ret .= json_decode('"\u2501"');
		}

		$ret .= json_decode('"\u2518"') . "\n";
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

	public function __construct() {
		$this->wetness = 0;
		$this->concentration = 0;
		$this->defaultChar = ".";
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
		}
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
