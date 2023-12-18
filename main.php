#!/usr/bin/php
<?php
// define some system variables
$skyHeight = 5;
$groundHeight = 5;
$vpWidth = 10;
$mapWidth = 30;
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
				case 'q':
					// quit the game
					system("stty $this->term");

					exit;
				case 'r':
					// add a raindrop up to maxRain
					$this->addRain();
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

	private function tick() {
		// we do this backwardes, otherwise stuff that moves into dirt from the sky,
		// will move twice in this tick
		$this->updateDirt();
		$this->updateSkyRain();

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
		//echo "Filling rectangle ($x1, $y1) to ($x2, $y2) with $fillObj\n";
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
					$containedObjs = $this->map[$x][$y]->getContains();

					foreach ($containedObjs as $containedObj) {
						if ($containedObj instanceof Rain) {
							$ret[] = [$x, $y];
						}
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

		$this->map[0][$rainY]->addContains(new Rain(rand(1, 3)));
	}

	private function addMineral() {
		$existingMs = $this->getDirtMineralCoords();
	
		// only allow up to maxMinerals
		if (count($existingMs) >= $this->maxMinerals) {
			return false;
		}

		// find a random unoccupied x,y value
		do {
			$mX = rand($this->skyHeight, $this->skyHeight + $this->groundHeight - 1);
			$mY = rand(0, $this->mapWidth - 1);
		} while (in_array([$mX, $mY], $existingMs));

		echo "Adding a mineral to $mX,$mY\n";
		$this->map[$mX][$mY]->addContains(new Mineral(rand(1, 5)));
	}

	// moves all the rain down a square in the skies (or ignores i)
	private function updateSkyRain() {
		$raindrops = $this->getSkyRainCoords();

		foreach ($raindrops as $raindrop) {
			// if the next space down is still air, move
			$newX = $raindrop[0] + 1;
			$newY = $raindrop[1];

			// move all the rains one down
			$newContains = array();

			foreach ($this->map[$raindrop[0]][$raindrop[1]]->getContains() as $containedObj) {
				if ($containedObj instanceof Rain && $newX < $this->mapHeight) {
					$this->map[$newX][$newY]->addContains($containedObj);
				} else {
					$newContains[] = $containedObj;
				}
			}

			$this->map[$raindrop[0]][$raindrop[1]]->setContains($newContains);
		} 
	}

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

	private function getDirtMineralCoords() {
		$ret = array();

		// scan the dirt for existing minerals
		for ($x = 0; $x < count($this->map); $x++) {
			for ($y = 0; $y < count($this->map[0]); $y++) {
				if ($this->map[$x][$y] instanceof Dirt) {
					$containedObjs = $this->map[$x][$y]->getContains();

					foreach ($containedObjs as $containedObj) {
						if ($containedObj instanceof Mineral) {
							$ret[] = [$x, $y];
						}

						break;
					}
				}
			}
		}

		return $ret;	
	}

	private function updateDirt() {
		$wetness = $this->getDirtRainCoords();

		// decrease 1 from the rain, and pass it on to the next guy
		foreach ($wetness as $wetness) {
			$d = $this->map[$wetness[0]][$wetness[1]];

			$newContains = array();
			foreach ($d->getContains() as $containedObj) {
				if ($containedObj instanceof Rain) {
					$containedObj->decrSize();

					if ($containedObj->getSize() > 0 && ($wetness[0] + 1 < $this->mapHeight)) {
						// if the tile below us is a dirt,
						// move the water there
						$this->map[$wetness[0] + 1][$wetness[1]]->addContains($containedObj);
					}
				} else {
					$newContains[] = $containedObj;
				}
			}		

			$this->map[$wetness[0]][$wetness[1]]->setContains($newContains);
		}
	}

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

class Rain {
	// how much rain in this drop
	public $size;

	public function __construct($size = 1) {
		$this->size = $size;	
	}

	public function decrSize() {
		$this->size--;
	}

	public function getSize() {
		return $this->size;
	}

	public function __toString() {
		return "|";
	}
}

class Mineral {
	public $size;

	public function __construct($size = 1) {
		$this->size = $size;	
	}
	
	public function getSize() {
		return $this->size;
	}

	public function __toString() {
		return "M";
	}
}

class Dirt {
	public $defaultChar;

	public $contains;

	public function __construct() {
		$this->contains = array();
		$this->defaultChar = ".";
	}

	public function getContains() {
		return $this->contains;
	}

	public function addContains($obj) {
		$this->contains[] = $obj;
	}

	public function setContains($obj) {
		$this->contains = $obj;
	}

	public function getView($viewType = 1) {
		// wetness view
		if ($viewType == 1) {
			if (count($this->contains)) {
				$wetness = 0;

				foreach ($this->contains as $containedObj) {
					if ($containedObj instanceof Rain) {
						$wetness += $containedObj->getSize();
					}
				}

				if ($wetness > 0) {
					return "$wetness";
				}

				return $this->defaultChar;
			}

			return $this->defaultChar;	
		} else if ($viewType == 2) {
			// mineral view
			if (count($this->contains)) {
				$concentration = 0;

				foreach ($this->contains as $containedObj) {
					if ($containedObj instanceof Mineral) {
						$concentration += $containedObj->getSize();
					}
				}

				if ($concentration > 0) {
					return "$concentration";
				}

				return $this->defaultChar;
			}

			return $this->defaultChar;	
		}
	}
	public function __toString() {
		return $this->getView(1);
	}
}

class Sky {
	// things in this region of sky are added here
	public $contains;

	public function __construct() {
		$this->contains = array();
	}

	// return 
	public function getContains() {
		return $this->contains;
	}

	public function setContains($obj) {
		$this->contains = $obj;
	}

	public function addContains($obj) {
		$this->contains[] = $obj;
	}

	public function __toString() {
		if (count($this->contains)) {
			foreach ($this->contains as $containedObj) {
				// we return immediately, but eventually want to add code
				// to set precedent to the different contained objects
				// and print accordingly
				return $containedObj->__toString();
			}
		}

		return "~";
	}
}
