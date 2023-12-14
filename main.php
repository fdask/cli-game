#!/usr/bin/php
<?php
// define some system variables
$skyHeight = 2;
$groundHeight = 2;
$mapWidth = 10;
$maxRain = 3;

$map = new Map($skyHeight, $groundHeight, $mapWidth, $maxRain);
echo $map;

class Map {
	private $map;
	private $term;

	// how much sky on the map
	public $skyHeight;

	// how deep is the dirt
	public $groundHeight;

	// how wide is the map
	public $mapWidth;

	// how high is the map
	public $mapHeight;

	// how many raindrops allowed on the map at once
	public $maxRain;

	public function __construct($skyHeight, $groundHeight, $mapWidth, $maxRain) {
		$this->skyHeight = $skyHeight;
		$this->groundHeight = $groundHeight;
		$this->mapHeight = $groundHeight + $skyHeight;
		$this->mapWidth = $mapWidth;
		$this->maxRain = $maxRain;

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

		while ($c = fread(STDIN, 1)) {
			switch ($c) {
				case 'q':
					// quit the game
					system("stty $this->term");

					exit;
				case 'r':
					$this->addRain();
			
					break;
				default:
					// do nothing here
			}

			$this->tick();
		}
	}

	private function tick() {
		echo $this;

		$this->updateRain();
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
		echo "Filling rectangle ($x1, $y1) to ($x2, $y2) with $fillObj\n";
		for ($x = $x1; $x < $x2; $x++) {
			for ($y = $y1; $y < $y2; $y++) {
				$this->map[$x][$y] = new $fillObj;
			}
		}	
	}

	private function getRainCoords() {
		$ret = array();

		// scan the skies for existing rain
		for ($x = 0; $x < count($this->map); $x++) {
			for ($y = 0; $y < count($this->map[0]); $y++) {
				if ($this->map[$x][$y] instanceof Sky) {
					$con = $this->map[$x][$y]->getContains();

					if ($con instanceof Rain) {
						$ret[] = [$x, $y];
					}
				}
			}
		}

		return $ret;
	}

	private function addRain() {
		$existingRain = $this->getRainCoords();
		$ys = array();

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

		$this->map[0][$rainY]->setContains(new Rain());
	}

	private function updateRain() {
		$raindrops = $this->getRainCoords();

		foreach ($raindrops as $raindrop) {
			// if the next space down is still air, move
			$newX = $raindrop[0] + 1;
			$newY = $raindrop[1];

			echo "Testing if the new rain x is less than mapheight ($newX < " . $this->mapHeight . ")\n";
			if ($newX < $this->mapHeight) {
				// move the rain one down 
				$this->map[$newX][$newY]->setContains($this->map[$raindrop[0]][$raindrop[1]]->getContains());
			} 

			$this->map[$raindrop[0]][$raindrop[1]]->setContains(null);
		}
	}

	public function __toString() {
		$ret = "";

		$ret .= json_decode('"\u250c"');

		for ($x = 0; $x < count($this->map[0]); $x++) {
			$ret .= json_decode('"\u2501"');
		}

		$ret .= json_decode('"\u2510"') . "\n";

		for ($x = 0; $x < count($this->map); $x++) {
			$ret .= json_decode('"\u2502"');

			for ($y = 0; $y < count($this->map[0]); $y++) {
				$ret .= $this->map[$x][$y];
			}

			$ret .= json_decode('"\u2502"');
			$ret .= "\n";
		}

		$ret .= json_decode('"\u2514"');

		for ($x = 0; $x < count($this->map[0]); $x++) {
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

	public function __toString() {
		return "|";
	}
}

class Dirt {
	public $contains;

	public function __construct() {
		$contains = null;
	}

	public function getContains() {
		return $this->contains;
	}

	public function setContains($obj) {
		$this->contains = $obj;
	}

	public function __toString() {
		if (!is_null($this->contains)) {
			return $this->contains->__toString();
		}

		return ".";
	}
}

class Sky {
	// things in this region of sky are added here
	public $contains;

	public function __construct() {
		$contains = null;
	}

	// return 
	public function getContains() {
		return $this->contains;
	}

	public function setContains($obj) {
		$this->contains = $obj;
	}

	public function __toString() {
		if (!is_null($this->contains)) {
			return $this->contains->__toString();
		}

		return "~";
	}
}
