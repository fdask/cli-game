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

	// every level has one!
	public $portal;

	// are we in the power menu?
	public $powerMenuOpen;

	public function __construct($filename = null) {
		if (!is_null($filename)) {
			$this->level = new Level($filename);
		} else {
			$this->level = new Level();
		}

		$this->mailbox = new Mailbox();
		$this->mailbox->addInstructions();
		
		$this->player = new Player($this->level->skyHeight, floor($this->level->vpWidth / 2));

		// during TESTING, add powers to the player
		$this->player->addPower(new Power("teleport", "Teleport"));
		$this->player->addPower(new Power("expand", "Expand"));
		$this->player->addPower(new Power("contract", "Contract"));
		$this->player->addPower(new Power("break", "Break"));

		$this->powerMenuOpen = false;

		$this->reset();
		$this->gameLoop();
	}

	public function reset() {
		// screen settings
		$this->viewType = 1;
		$this->vpY = 0;
		$this->threeScreen = false;
		$this->charOnScreens = array(1, 2, 3);

		$this->portal = null;

		$this->initialize();
	}

    public function checkPointsOfInterest() {
        // using the x, y of the player, 
        // lets figure out if any of the current 
        // points of interests apply
		if (is_array($this->level->pointsOfInterest) && !empty($this->level->pointsOfInterest)) {
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

		$this->parseMap();

		$dirtSize = count($this->getDirtCoords());

		if ($this->level->enableMinerals && $this->level->mineralsAtStart) {
			$mineralCount = 0;

			while (($mineralCount / $dirtSize) < ($this->level->maxMinerals / $dirtSize)) {
				$mineralCount++;
				$this->addMineral();
			}
		}

		// add in plants up to maxPlants
		if ($this->level->enablePlants && $this->level->plantsAtStart) {
			for ($x = 0; $x < $this->level->maxPlants; $x++) {
				$this->addPlant();
			}
		}

		// rocks are always added at start.
		if ($this->level->enableRocks) {
			// add in the rocks (up to maxRocks %)
			do {
				// nothing
			} while ($this->addRock());
		}

		if ($this->level->enableCrystals && $this->level->crystalsAtStart) {
			do {
				// nothing
			} while ($this->addCrystal());
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
				// start toxicity is a percent age of the soil.
				$twCount = 0;

				while (($twCount / $dirtSize) < ($this->level->startToxicity / $dirtSize)) {
					$twCount++;
					$this->addToxicWaste();
				}
			}
		}

		// add a portal to the map
		$this->addPortal();
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

	/** 
	 * takes map data from the level
	 * and applies it to the actual map
	 */
	private function parseMap() {
		$lmap = $this->level->getMap();

		if (!is_null($lmap)) {
			for ($x = 0; $x < count($lmap); $x++) {
				$splitY = str_split($lmap[$x]);

				for ($y = 0; $y < count($splitY); $y++) {
					$withSkyX = $x + ($this->level->skyHeight);

					switch ($splitY[$y]) {
						case 'D':
							// dirt
							$this->map[$withSkyX][$y] = new Dirt;

							break;
						case 'R':
							// rocks
							$this->map[$withSkyX][$y] = new Rock;

							break;
						case 'P':
							// exit portal
							$this->portal->setXY($withSkyX, $y);
							
							break;
						case 'T':
							// toxic waste
							$this->map[$withSkyX][$y] = new Dirt;
							$this->map[$withSkyX][$y]->setToxicity(rand($this->level->toxicityMin, $this->level->toxicityMax));

							break;
					}
				}
			}
		}
	}

	public function activatePowerTeleport() {
		// what do we do with the tail
		
		// select a random new dirt location
		$pCoords = $this->player->getCoords();

		// make sure its not on rocks
		$rCoords = $this->getRockCoords();

		$taken = array_merge($pCoords, $rCoords);

		do {
			$newX = rand($this->level->skyHeight, $this->level->mapHeight - 1);
			$newY = rand(0, $this->level->mapWidth - 1);
		} while (in_array([$newX, $newY], $taken));

		// transfer the player there
		//$this->player->addFrontCoord($newX, $newY);
		$this->player->setX($newX);
		$this->player->setY($newY);
		$this->player->coordPop();
		$this->player->coordPop();


		$newViewPortY = $newY - floor($this->level->vpWidth / 2);

		if ($newViewPortY < 0) {
			$newViewPortY = $this->level->mapWidth + $newViewPortY; 
		}

		$this->vpY = $newViewPortY;
	}

	public function activatePowerExpand() {
		// push everything one square away (except rocks, and flowers)
		$x = $this->player->getX();
		$y = $this->player->getY();

		for ($passCount = 0; $passCount < 3; $passCount++) {
			for ($loopX = $x - 3; $loopX <= $x + 3; $loopX++) {
				for ($loopY = $y - 3; $loopY <= $y + 3; $loopY++) {
					$dest = null;

					if 	(
						($loopX == $x - 3 && $loopY == $y - 3 && $passCount == 0) ||
						($loopX == $x - 3 && $loopY == $y - 2 && $passCount == 0) ||
						($loopX == $x - 3 && $loopY == $y - 1 && $passCount == 0) ||
						($loopX == $x - 2 && $loopY == $y - 3 && $passCount == 0) ||
						($loopX == $x - 1 && $loopY == $y - 3 && $passCount == 0) ||
						($loopX == $x - 2 && $loopY == $y - 2 && $passCount == 1) || 
						($loopX == $x - 2 && $loopY == $y - 1 && $passCount == 1) ||
						($loopX == $x - 1 && $loopY == $y - 2 && $passCount == 1) ||
						($loopX == $x - 1 && $loopY == $y - 1 && $passCount == 2)) {
							// top left corner (x - 1, y - 1)
							//echo "top left corner ($passCount)\n";
							$dest = [$loopX - 1, $loopY - 1];
					} else if (
						($loopX == $x - 3 && $loopY == $y && $passCount == 0) ||
						($loopX == $x - 2 && $loopY == $y && $passCount == 1) ||
						($loopX == $x - 1 && $loopY == $y && $passCount == 2)) {
							// top middle (x - 1, y)
							//echo "top middle ($passCount)\n";
							$dest = [$loopX - 1, $loopY];
					} else if (
						($loopX == $x - 3 && $loopY == $y + 1 && $passCount == 0) ||
						($loopX == $x - 3 && $loopY == $y + 2 && $passCount == 0) ||
						($loopX == $x - 3 && $loopY == $y + 3 && $passCount == 0) ||
						($loopX == $x - 3 && $loopY == $y + 2 && $passCount == 0) ||
						($loopX == $x - 2 && $loopY == $y + 3 && $passCount == 0) ||
						($loopX == $x - 1 && $loopY == $y + 3 && $passCount == 0) ||
						($loopX == $x - 2 && $loopY == $y + 1 && $passCount == 1) ||
						($loopX == $x - 2 && $loopY == $y + 2 && $passCount == 1) ||
						($loopX == $x - 1 && $loopY == $y + 1 && $passCount == 2) ||
						($loopX == $x - 1 && $loopY == $y + 2 && $passCount == 1)) {
							// top right (x - 1, y + 1)
							//echo "top right ($passCount)\n";
							$dest = [$loopX - 1, $loopY + 1];
					} else if (
						($loopX == $x && $loopY == $y - 3 && $passCount == 0) ||
						($loopX == $x && $loopY == $y - 2 && $passCount == 1) ||
						($loopX == $x && $loopY == $y - 1 && $passCount == 2)) {
							// center line (left) (x, y - 1)
							//echo "center (left) ($passCount)\n";
							$dest = [$loopX, $loopY - 1];
					} else if (
						($loopX == $x && $loopY == $y + 3 && $passCount == 0) ||
						($loopX == $x && $loopY == $y + 1 && $passCount == 2) ||
						($loopX == $x && $loopY == $y + 2 && $passCount == 1)) {
							// center line (right) (x, y + 1)
							//echo "center (right) ($passCount)\n";
							$dest = [$loopX, $loopY + 1];
					} else if (
						($loopX == $x + 1 && $loopY == $y - 3 && $passCount == 0) ||
						($loopX == $x + 2 && $loopY == $y - 3 && $passCount == 0) ||
						($loopX == $x + 3 && $loopY == $y - 3 && $passCount == 0) ||
						($loopX == $x + 3 && $loopY == $y - 2 && $passCount == 0) ||
						($loopX == $x + 3 && $loopY == $y - 1 && $passCount == 0) ||
						($loopX == $x + 1 && $loopY == $y - 2 && $passCount == 1) ||
						($loopX == $x + 1 && $loopY == $y - 1 && $passCount == 2) ||
						($loopX == $x + 2 && $loopY == $y - 2 && $passCount == 1) ||
						($loopX == $x + 2 && $loopY == $y - 1 && $passCount == 1)) {
							// bottom left corner (x + 1, y - 1)
							//echo "bottom left ($passCount)\n";
							$dest = [$loopX + 1, $loopY - 1];
					} else if (
						($loopX == $x + 3 && $loopY == $y && $passCount == 0) ||
						($loopX == $x + 1 && $loopY == $y && $passCount == 2) ||
						($loopX == $x + 2 && $loopY == $y && $passCount == 1)) {
							// bottom middle (x + 1, y)
							//echo "bottom middle ($passCount)\n";
							$dest = [$loopX + 1, $loopY];
					} else if (
						($loopX == $x + 3 && $loopY == $y + 1 && $passCount == 0) ||
						($loopX == $x + 3 && $loopY == $y + 2 && $passCount == 0) ||
						($loopX == $x + 3 && $loopY == $y + 3 && $passCount == 0) ||
						($loopX == $x + 2 && $loopY == $y + 3 && $passCount == 0) ||
						($loopX == $x + 1 && $loopY == $y + 3 && $passCount == 0) ||
						($loopX == $x + 1 && $loopY == $y + 1 && $passCount == 2) ||
						($loopX == $x + 1 && $loopY == $y + 2 && $passCount == 1) ||
						($loopX == $x + 2 && $loopY == $y + 1 && $passCount == 1) ||
						($loopX == $x + 2 && $loopY == $y + 2 && $passCount == 1)) {
							// bottom right corner (x + 1, y + 1)
							//echo "bottom right ($passCount)\n";
							$dest = [$loopX + 1, $loopY + 1];
					}

					if (!is_null($dest)) {
						// adjust the destination for any overflows/wraparounds
						$this->copyDirtContents($loopX, $loopY, $dest[0], $dest[1]);
						$this->clearDirt($loopX, $loopY);
					}
				}
			}
		}
	}

	public function activatePowerContract() {
		// draw everything in one tile
		$x = $this->player->getX();
		$y = $this->player->getY();

		for ($passCount = 0; $passCount < 2; $passCount++) {
			for ($loopX = $x - 3; $loopX <= $x + 3; $loopX++) {
				for ($loopY = $y - 3; $loopY <= $y + 3; $loopY++) {
					$dest = null;

					// if this is a square we want, set the dest!
					if (
						($loopX == $x - 1 && $loopY == $y - 2 && $passCount == 0) ||
						($loopX == $x - 2 && $loopY == $y - 2 && $passCount == 0) ||
						($loopX == $x - 2 && $loopY == $y - 1 && $passCount == 0) ||
						($loopX == $x - 1 && $loopY == $y - 3 && $passCount == 1) ||
						($loopX == $x - 2 && $loopY == $y - 3 && $passCount == 1) ||
						($loopX == $x - 3 && $loopY == $y - 3 && $passCount == 1) ||
						($loopX == $x - 3 && $loopY == $y - 2 && $passCount == 1) ||
						($loopX == $x - 3 && $loopY == $y - 1 && $passCount == 1)) {
							// top left corner
							$dest = [$loopX + 1, $loopY + 1];
					} else if (
						($loopX == $x - 2 && $loopY == $y && $passCount == 0) ||
						($loopX == $x - 3 && $loopY == $y && $passCount == 1)) {
							// top middle
							$dest = [$loopX + 1, $loopY];
					} else if (
						($loopX == $x - 2 && $loopY == $y + 1 && $passCount == 0) ||
						($loopX == $x - 2 && $loopY == $y + 2 && $passCount == 0) ||
						($loopX == $x - 1 && $loopY == $y + 2 && $passCount == 0) ||
						($loopX == $x - 3 && $loopY == $y + 1 && $passCount == 1) ||
						($loopX == $x - 3 && $loopY == $y + 2 && $passCount == 1) ||
						($loopX == $x - 3 && $loopY == $y + 3 && $passCount == 1) ||
						($loopX == $x - 2 && $loopY == $y + 3 && $passCount == 1) ||
						($loopX == $x - 1 && $loopY == $y + 3 && $passCount == 1)) {
							// top right
							$dest = [$loopX + 1, $loopY - 1];
					} else if (
						($loopX == $x && $loopY == $y - 2 && $passCount == 0) ||
						($loopX == $x && $loopY == $y - 3 && $passCount == 1)) {
							// center left
							$dest = [$loopX, $loopY + 1];
					} else if (
						($loopX == $x && $loopY == $y + 2 && $passCount == 0) ||
						($loopX == $x && $loopY == $y + 3 && $passCount == 1)) {
							// center right
							$dest = [$loopX, $loopY - 1];
					} else if (
						($loopX == $x + 1 && $loopY == $y - 2 && $passCount == 0) ||
						($loopX == $x + 2 && $loopY == $y - 2 && $passCount == 0) ||
						($loopX == $x + 2 && $loopY == $y - 1 && $passCount == 0) ||
						($loopX == $x + 1 && $loopY == $y - 3 && $passCount == 1) ||
						($loopX == $x + 2 && $loopY == $y - 3 && $passCount == 1) ||
						($loopX == $x + 3 && $loopY == $y - 3 && $passCount == 1) ||
						($loopX == $x + 3 && $loopY == $y - 2 && $passCount == 1) ||
						($loopX == $x + 3 && $loopY == $y - 1 && $passCount == 1)) {
							// bottom left
							$dest = [$loopX - 1, $loopY + 1];
					} else if (
						($loopX == $x + 2 && $loopY == $y && $passCount == 0) ||
						($loopX == $x + 3 && $loopY == $y && $passCount == 1)) {
							// bottom middle
							$dest = [$loopX - 1, $loopY];
					} else if (
						($loopX == $x + 2 && $loopY == $y + 1 && $passCount == 0) ||
						($loopX == $x + 2 && $loopY == $y + 2 && $passCount == 0) ||
						($loopX == $x + 1 && $loopY == $y + 2 && $passCount == 0) ||
						($loopX == $x + 3 && $loopY == $y + 1 && $passCount == 1) ||
						($loopX == $x + 3 && $loopY == $y + 2 && $passCount == 1) ||
						($loopX == $x + 3 && $loopY == $y + 3 && $passCount == 1) ||
						($loopX == $x + 2 && $loopY == $y + 3 && $passCount == 1) ||
						($loopX == $x + 1 && $loopY == $y + 3 && $passCount == 1)) {
							// bottom right
							$dest = [$loopX - 1, $loopY - 1];
					}
				
					// if we got a dest
					if (!is_null($dest)) {
						// adjust the destination for any overflows/wraparounds
						$this->copyDirtContents($loopX, $loopY, $dest[0], $dest[1]);
						$this->clearDirt($loopX, $loopY);
					}
				}
			}
		}
	}

	public function clearDirt($dirtX, $dirtY) {
		// whether or not we moved the squares, delete all the present values
		if (isset($this->map[$dirtX][$dirtY]) && $this->map[$dirtX][$dirtY] instanceof Dirt) {
			// minerals
			$this->map[$dirtX][$dirtY]->setConcentration(0);

			// wetness
			$this->map[$dirtX][$dirtY]->setWetness(0);

			// toxic waste
			$this->map[$dirtX][$dirtY]->setToxicity(0);

			// crystals
			$this->map[$dirtX][$dirtY]->setCrystal(false);

			return true;
		}

		return false;
	}

	public function copyDirtContents($srcX, $srcY, $destX, $destY) {
		$toxicCoords = $this->getToxicCoords();
		$rainCoords = $this->getDirtRainCoords();
		$mineralCoords = $this->getMineralCoords();
		$cCoords = $this->getCrystalCoords();

		if ($destX > $this->level->mapHeight - 1) {
			return false;
		}

		if ($destX < 0) {
			return false;
		}

		if ($destY >= $this->level->mapWidth) {
			$destY = $destY - $this->level->mapWidth;
		}

		if ($destY < 0) {
			$destY = $destY + $this->level->mapWidth;
		}

		// if the destination exists
		if (isset($this->map[$destX][$destY])) {
			// and is not a rock or sky
			if ($this->map[$destX][$destY] instanceof Dirt) {	
				//echo "Destination $destX,$destY exists and is dirt!\n";	
				// move the minerals there
				if (in_array([$srcX, $srcY], $mineralCoords)) {
					$oldConcentration = $this->map[$srcX][$srcY]->getConcentration();
					$newConcentration = $this->map[$destX][$destY]->getConcentration();

					// add to the existing minerals
					$this->map[$destX][$destY]->setConcentration($oldConcentration + $newConcentration);
				}

				// move the wetness there
				if (in_array([$srcX, $srcY], $rainCoords)) {
					$oldWetness = $this->map[$srcX][$srcY]->getWetness();
					$newWetness = $this->map[$destX][$destY]->getWetness();

					// add to the existing wetness
					$this->map[$destX][$destY]->setWetness($oldWetness + $newWetness);
				}
				
				// move the toxic waste there
				if (in_array([$srcX, $srcY], $toxicCoords)) {
					$oldToxicity = $this->map[$srcX][$srcY]->getToxicity();
					$newToxicity = $this->map[$destX][$destY]->getToxicity();

					// add to the existing toxicity
					$this->map[$destX][$destY]->setToxicity($oldToxicity + $newToxicity);
				}

				// move the crystals there
				if (in_array([$srcX, $srcY], $cCoords)) {
					$this->map[$destX][$destY]->setCrystal(true);
				}
			}
		} else {
			// echo "destination doesn't exist! ($destX, $destY)\n";
		}
	}

	public function activatePowerBreak() {
		// any rocks in radius are turned into minerals
		$rCoords = $this->getRockCoords();

		$x = $this->player->getX();
		$y = $this->player->getY();

		for ($loopX = $x - 3; $loopX <= $x + 3; $loopX++) {
			for ($loopY = $y - 3; $loopY <= $y + 3; $loopY++) {
				if ($loopY >= $this->level->mapWidth) {
					$loopY = $loopY - $this->level->mapWidth;
				}
		
				if ($loopY < 0) {
					$loopY = $loopY + $this->level->mapWidth;
				}
		
				if (in_array([$loopX, $loopY], $rCoords)) {
					$this->map[$loopX][$loopY] = new Dirt;
					$this->map[$loopX][$loopY]->setConcentration(9);
				}
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
				case ' ':
				case '\n':
					if ($this->powerMenuOpen) {
						// select the current active power
						$this->player->setActivePower($this->player->getMenuPower());

						// redraw the message
						$ps = $this->player->getPowersScreen();
						$this->mailbox->deleteCurrentMessage();
						$this->mailbox->addMessageToStart($ps);
					}

					$tick = false;

					break;
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
					$cm = $this->mailbox->readCurrentMessage();

					if ($cm[0] == $ss[0]) {
						$this->mailbox->deleteCurrentMessage();
					} else {
						$this->mailbox->addMessageToStart($ss);
					}

					$tick = false;

					break;
				case 'a': 
					// move the character left
				case 'A':
					if (!$this->powerMenuOpen) {
						// grow character to the left
						$newX = $this->player->getX();
						$newY = $this->player->getY() - 1;

						if ($newY < 0) {
							$newY = $this->level->mapWidth - 1;
						} 
						
						$tick = $this->playerMove($newX, $newY, false, $c == 'D');

						// move viewport left
						if ($tick) {
							if ($this->vpY - 1 < 0) {
								$this->vpY = $this->level->mapWidth - 1;
							} else {
								$this->vpY--;
							}
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
					if (!$this->powerMenuOpen) {
						// grow the character to the right
						$newX = $this->player->getX();
						$newY = $this->player->getY() + 1;

						if ($newY > $this->level->mapWidth - 1) {
							$newY = 0;
						} 
						
						$tick = $this->playerMove($newX, $newY, false, $c == 'D');

						if ($tick) {
							// scroll viewprot to the right
							if ($this->vpY + 1 >= $this->level->mapWidth) {
								$this->vpY = 0;
							} else {
								$this->vpY++;
							}
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
				case 'o':
					// activate selected power
					$p = $this->player->getPower();
					
					if ($p && $this->player->getCrystals() > 0) {
						// do we have enough crystals?
						$this->player->decrCrystals();

						// now do what the power tells us
						switch ($p->getType()) {
							case 'teleport':
								$this->activatePowerTeleport();

								break;
							case 'expand':
								$this->activatePowerExpand();

								break;
							case 'contract':
								$this->activatePowerContract();

								break;
							case 'break':
								$this->activatePowerBreak();

								break;
						}
					}

					$tick = false;

					break;
				case 'p':
					// select power
					$ps = $this->player->getPowersScreen();
					$cm = $this->mailbox->readCurrentMessage();

					if ($cm[1] == $ps[1]) {
						$this->mailbox->deleteCurrentMessage();
						$this->powerMenuOpen = false;
					} else {
						$this->mailbox->addMessageToStart($ps);
						$this->powerMenuOpen = true;
					}

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
					if ($this->powerMenuOpen) {
						$mp = $this->player->getMenuPower();

						if (($mp + 1) < count($this->player->getPowers())) {
							$this->player->setMenuPower($this->player->getMenuPower() + 1);

							// redraw the message
							$ps = $this->player->getPowersScreen();
							$this->mailbox->deleteCurrentMessage();
							$this->mailbox->addMessageToStart($ps);
						}

						$tick = false;
					} else {
						// grow character down
						$newX = $this->player->getX() + 1;
						$newY = $this->player->getY();

						// don't go below the bottom
						if ($newX > $this->level->mapHeight - 1) {
							$newX = $this->level->mapHeight - 1;
						} 
						
						$tick = $this->playerMove($newX, $newY, true, $c == 'S');
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
					if ($this->powerMenuOpen) {
						$this->player->setMenuPower($this->player->getMenuPower() - 1);

						if ($this->player->getMenuPower() < 0) {
							$this->player->setMenuPower(0);
						}

						// redraw the message
						$ps = $this->player->getPowersScreen();
						$this->mailbox->deleteCurrentMessage();
						$this->mailbox->addMessageToStart($ps);

						$tick = false;
					} else {
						$newX = $this->player->getX() - 1;
						$newY = $this->player->getY();

						// character cannot go into the sky
						if ($newX < $this->level->skyHeight) {
							$newX = $this->level->skyHeight;
						}

						$tick = $this->playerMove($newX, $newY, true, $c == 'W');
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

	public function playerMove($newX, $newY, $setX = false, $grow = false) {
		// character cannot pass onto itself
		if (!$this->player->onCoords($newX, $newY)) {
			// character cannot go through a rock
			if ($this->map[$newX][$newY] instanceof Rock) {
				return false;
			}

			// character has landed on the portal
			if (!is_null($this->portal) && $this->portal->getXY() == [$newX, $newY] && $this->portal->isVisible()) {
				// load the next map
				$nl = $this->level->getNextLevel();

				if (!is_null($nl)) {
					// load it
					$this->level = null;
					
					if ($nl == "generate" || !file_exists($nl)) {
						$this->level = new Level();
						$this->level->takeArray(levelGenerator::generate());
					} else {
						$this->level = new Level($nl);
					}
				
					$this->reset();
					$this->player->reset($this->level->playerStartX, $this->level->playerStartY);
				} else {
					// we are out of levels to play
					$this->gameOver();
				}
			}

			// set the X or the Y
			// (records a move in the player class)
			if ($setX) {
				$this->player->setX($newX);
			} else {
				$this->player->setY($newY);
			}

			// are we growing, or moving
			if (!$grow) {
				$this->player->coordPop();
			} else {
				$this->player->scoreGrown++;
			}

			// collect rain
			if ($this->level->enableRain) {
				$wetness = $this->map[$newX][$newY]->getWetness();

				if ($wetness > 0) {
					$this->player->incrMoisture($wetness);
					$this->map[$newX][$newY]->setWetness(0);
				}

				// did we get too dry or wet?
				$this->checkPlayerMoisture();
			}

			// collect minerals 
			if ($this->level->enableMinerals) {
				$concentration = $this->map[$newX][$newY]->getConcentration();

				if ($concentration > 0) {
					$this->player->setLife($concentration + $this->player->getLife());
					$this->player->scoreMinerals += $concentration;
					$this->map[$newX][$newY]->setConcentration(0);
				}
			}
			
			// collect toxicity
			if ($this->level->enableToxicity) {
				$toxicity = $this->map[$newX][$newY]->getToxicity();

				if ($toxicity > 0) {
					$this->player->setLife($this->player->getLife() - ($toxicity * $this->level->toxicityMultiplier));
					$this->player->scoreToxicWaste += $toxicity;
					$this->map[$newX][$newY]->setToxicity(0);
				}
			}
			
			// collect crystal
			if ($this->level->enableCrystals) {
				if ($this->map[$newX][$newY] instanceof Dirt && $this->map[$newX][$newY]->isCrystal()) {
					// remove the crystal, and add to players inventory
					$this->player->incrCrystals();
					$this->map[$newX][$newY]->setCrystal(false);
					$this->player->setLife($this->player->getLife() + $this->level->crystalHealthBonus);
				}
			}

			// upkeep tax (health decrement)
			if ($this->player->decrLife() < 0) {
				$this->gameOver();
			}
		} else {
			return false;
		}

		return true;
	}

	public function checkPlayerMoisture() {
		if ($this->level->enableMoisture) {
			if ($this->player->getMoisture() >= ($this->player->getMoistureMax() * 2)) {
				echo "YOU GOT TOO WET AND DIED\n";

				$this->gameOver();
			} else if ($this->player->getMoisture() <= ($this->player->getMoistureMax() * -2)) {
				echo "YOU DRIED OUT AND DIED\n";

				$this->gameOver();
			}
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
			$rows = $this->level->crystalDepth;
			$dirtSquares = $rows * $this->level->mapWidth;
			$crystalPercentage = (count($this->getCrystalCoords()) / $dirtSquares) * 100;

			// if we are below our minimum, add one in
			if ($crystalPercentage < $this->level->minCrystals) {			
				$this->addCrystal();
			} 
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

				if (!$objective->getCompleted()) {
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
								$this->player->addXP($objective->getRewardXP());
							}

							break;
					}        
				}
			}
		}

		// check if all objectives are complete
		if ($this->level->objectivesComplete() && !$this->level->getCompleted()) {
			$this->level->setCompleted();
			$this->mailbox->addMessage("Portal has been revealed!");
			$this->portal->setVisible();
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

		$this->map[$mX][$mY]->setConcentration(rand($this->level->mineralConcentrationMin, $this->level->mineralConcentrationMax));
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
		$playerCoords = $this->player->getCoords();
		$dirtCoords = $this->getDirtCoords();

		$taken = array_merge($rockCoords, $playerCoords);

		if (count($taken) >= count($dirtCoords)) {
			// all dirt is taken!
			return false;
		}
		
		if ((count($rockCoords) / count($dirtCoords)) * 100 >= $this->level->maxRocks) {
			return false;
		}

		do {
			$newX = rand($this->level->skyHeight, $this->level->mapHeight - 1);
			$newY = rand(0, $this->level->mapWidth - 1);
		} while (in_array([$newX, $newY], $taken));

		$rock = new Rock();

		$this->map[$newX][$newY] = $rock;

		return true;
	}

	public function addCrystal() {
        // look for a dirt square we can take
        $cCoords = $this->getCrystalCoords();
        $rockCoords = $this->getRockCoords();
		$player = $this->player->getCoords();
		$toxicCoords = $this->getToxicCoords();
		
		$rows = $this->level->crystalDepth;
		$dirtSquares = $rows * $this->level->mapWidth;

		$allCoords = array_merge($cCoords, $rockCoords, $player, $toxicCoords);

        if (count($allCoords) >= $dirtSquares) {
			// all dirt is taken!
            return false;
        }  

		$crystalPercentage = (count($cCoords) / $dirtSquares) * 100;

		if ($crystalPercentage >= $this->level->maxCrystals) {
			// we've passed our level limits
			return false;
		}

		do {
			$newX = rand($this->level->mapHeight - $rows, $this->level->mapHeight - 1);
			$newY = rand(0, $this->level->mapWidth - 1);
		} while(in_array([$newX, $newY], $allCoords));

		$this->map[$newX][$newY]->setCrystal(true);

		return true;
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

		$dirtCoords = $this->getDirtCoords();
		$rows = $this->level->mapHeight - $this->level->skyHeight;

		if (count($taken) >= $rows * $this->level->mapWidth) {
			return false;
		}

		do {
			$newX = rand($this->level->skyHeight, $this->level->mapHeight - 1);
			$newY = rand(0, $this->level->mapWidth - 1);
		} while (in_array([$newX, $newY], $taken));

		$this->map[$newX][$newY]->setToxicity(rand($this->level->toxicityMin, $this->level->toxicityMax));
	}

	public function addPortal() {
		if (is_null($this->portal)) {
			// add a portal object at random coords
			$taken = array_merge($this->getRockCoords(), $this->getCrystalCoords(), $this->player->getCoords());

			do {
				$portalX = rand($this->level->skyHeight, $this->level->mapHeight - 1);
				$portalY = rand(0, $this->level->mapWidth - 1);
			} while (in_array([$portalX, $portalY], $taken));
			
			$this->portal = new Portal($portalX, $portalY);
		}
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

	/**
	 * get an array of all rock locations (in the dirt)
	 */
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

	/**
	 * get an array of all dirt coords that have a crystal
	 */
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

	/**
	 * return coordinates of all dirt instances with a mineral concentration
	 */
    private function getMineralCoords() {
		$ret = array();

		// scan the dirt for existing minerals
		for ($x = 0; $x < $this->level->mapHeight; $x++) {
			for ($y = 0; $y < $this->level->mapWidth; $y++) {
				if ($this->map[$x][$y] instanceof Dirt && ($this->map[$x][$y]->hasMinerals())) {
					$ret[] = [$x, $y];
				}
			}
		}

		return $ret;	
	}

	/**
	 * return an array of all the dirt coords that have a plant
	 */
	private function getPlantCoords() {
		$ret = array();

		for ($x = 0; $x < $this->level->mapHeight; $x++) {
			for ($y = 0; $y < $this->level->mapWidth; $y++) {
				if ($this->map[$x][$y] instanceof Dirt && $this->map[$x][$y]->hasPlant()) {
					$ret[] = [$x, $y];
				}
			}
		}

		return $ret;
	}

	/**
	 * get an array of ALL coordinates that are wet (dirt and sky)
	 */
	private function getRainCoords() {
		$ret = array();

		for ($x = 0; $x < $this->level->mapHeight; $x++) {
			for ($y = 0; $y < $this->level->mapWidth; $y++) {
				if ($this->map[$x][$y] instanceof Rock) {
					continue;
				} else if ($this->map[$x][$y]->isWet()) {
					$ret[] = [$x, $y];
				}
			}
		}

		return $ret;
	}

	private function getDirtRainCoords() {
		$ret = array();

		// scan the skies for existing rain
		for ($x = $this->level->skyHeight; $x < $this->level->mapHeight; $x++) {
			for ($y = 0; $y < $this->level->mapWidth; $y++) {
				if ($this->map[$x][$y] instanceof Dirt) {
					if ($this->map[$x][$y]->isWet()) {
						$ret[] = [$x, $y];
					}
				}
			}
		}

		return $ret;
	}

	/**
	 * get an array of all sky coords that are wet
	 */
	private function getSkyRainCoords() {
		$ret = array();

		// scan the skies for existing rain
		for ($x = 0; $x < $this->level->mapHeight; $x++) {
			for ($y = 0; $y < $this->level->mapWidth; $y++) {
				if ($this->map[$x][$y] instanceof Sky) {
					if ($this->map[$x][$y]->isWet()) {
						$ret[] = [$x, $y];
					}
				}
			}
		}

		return $ret;
	}

	/**
	 * get an array of meteor coordinates (in the sky)
	 */
	private function getMeteorCoords() {
		if ($this->meteor && $this->meteor instanceof Meteor) {
			$xy = $this->meteor->getXY();

			return array_merge($this->meteor->getTailCoords(), $x[0], $x[1]);
		}
		
		return false;
	}

	/**
	 * add up the toxic level of every toxic dirt square
	 */
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

	/**
	 * how much of the dirt is made up of toxic waste?
	 */
	private function getToxicWastePercentage() {
		$tt = $this->getTotalToxicity();
		$rocks = $this->getRockCoords();
		$dirt = $this->getDirtCoords();

		$ret = round(($tt / (count($dirt) - count($rocks))) * 100, 1);

		return $ret;
	}

	/**
	 * get every dirt coordinate that is toxic
	 */
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

	/**
	 * if there is a meteor, update it's position/state
	 */
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

						if ($target > $this->level->mapHeight - 1) {
							$target = $this->level->mapHeight - 1;
						}

						for ($x = $this->level->skyHeight; $x < $target; $x++) {
							if ($this->map[$x][$newMy] instanceof Dirt) {
								$this->map[$x][$newMy]->setToxicity(rand($this->level->toxicityMin, $this->level->toxicityMax));
								
								if ($dir == "R") {
									$newMy = $newMy + $speed;

									if ($newMy >= $this->level->mapWidth) {
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

	/**
	 * hunt for tiles with wetness, and drop them down by 1.
	 * 
	 * if they hit a player, adjust players moisture
	 */
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
					if ($this->level->enableMoisture) {
						$this->player->setMoistureTicker($this->player->getMoistureTicks());
						$this->player->incrMoisture($wetness);
					}
				} else if ($this->map[$newX][$newY] instanceof Dirt || $this->map[$newX][$newY] instanceof Sky) {
                    $this->map[$newX][$newY]->setWetness($wetness);
                }
			} else if ($this->map[$x][$y] instanceof Dirt) {
				$this->map[$x][$y]->setWetness(0);

				// if the player is in the newX, newY,
				// reset the moisture tick counter
				// get all the coordinates of the character
				if ($this->player->onCoords($newX, $newY)) {				
					if ($this->level->enableMoisture) {
						$this->player->setMoistureTicker($this->player->getMoistureTicks());
						$this->player->incrMoisture($wetness);
						
						$this->checkPlayerMoisture();
					}
				} else if ($this->map[$newX][$newY] instanceof Dirt) {
				    $this->map[$newX][$newY]->setWetness($wetness - 1);
                }

				// update dirts containing minerals
				$concentration = $this->map[$x][$y]->getConcentration();

				if ($concentration > 0) {
					if ($this->level->newRainStyle) {
						// minerals are washed down
						$newConcentration = $concentration - 1;

						$this->map[$x][$y]->decrConcentration($newConcentration);

						// space below gets a mineral
						if ($this->map[$newX][$newY] instanceof Dirt) {
							$this->map[$newX][$newY]->incrConcentration($newConcentration);
						}
					} else {
						// old rain style
						$this->map[$x][$y]->decrConcentration();

						if ($this->map[$newX][$newY] instanceof Dirt) {
							$this->map[$newX][$newY]->incrConcentration();
						}
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
			" ",
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

		if ($this->level->enableCrystals) {
			$ret .= "Crystals: " . $this->player->getCrystals() . " ";
		}

		if ($this->level->enableToxicity) {
			$ret .= "TW%: " . $this->getToxicWastePercentage() . " ";
		}

		if ($this->level->enableMoisture) {
			$ret .= "Moisture: " . $this->moistureDisplay() . " ";
		}

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
			for ($x = 0; $x < $this->level->mapHeight; $x++) {
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
					} else if ($this->portal->getXY() == [$x, $ypos] && $this->portal->isVisible()) {
						// player is on the portal
						$ret .= $this->portal;
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

					if ($this->portal->getXY() == [$x, $ypos] && $this->portal->isVisible()) {
						// player is on the portal
						$ret .= $this->portal;
                   	} else if ($this->player->onCoords($x, $ypos) && in_array(2, $this->charOnScreens)) {
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
				
					if ($this->portal->getXY() == [$x, $ypos] && $this->portal->isVisible()) {
						// player is on the portal
						$ret .= $this->portal;
                   	} else if ($this->player->onCoords($x, $ypos) && in_array(3, $this->charOnScreens)) {
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
				case 1: 
					$txt = "h2o";
					break;
				case 2:
					$txt = "mineral";
					break;
				case 3:
					$txt = "plant";
					break;
			}

			$ret .= $this->titleHeader($this->level->vpWidth, $txt);

			$ret .= "\n";

			// all the map lines
			for ($x = 0; $x < $this->level->mapHeight; $x++) {
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
					} else if ($this->portal->getXY() == [$x, $ypos] && $this->portal->isVisible()) {
						// player is on the portal
						$ret .= $this->portal;
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

