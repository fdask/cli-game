<?php
class Player {
	private $x;
	private $y;

	private $coords;

	private $life;

    private $crystals;

	// how wet we are
	private $moisture;

	// how many moves without water until we are LESS moist
	private $moistureTicks;

	// actual keep track of the ticker here
	private $moistureTicker;

	private $moistureMax;

	// what to show for the body of the player
    private $defaultChar;

	// what character for the head of the char
	private $headChar;

	// our experience points
	private $xp;

	// holds players powers
	private $powers;

	// which power is currently active
	private $activePower;
	private $menuPower;

	private $invulnerable;
	private $invulnerableDuration;

	// the players stats
	public $scoreWater;
	public $scoreMinerals;
	public $scoreToxicWaste;
	public $scoreMoves;
	public $scoreGrow;
	public $scoreShrink;
	public $scoreCrystals;
	public $scoreFossils;

	public function __construct($x, $y) {
		$this->coords[] = array($x, $y);

		$this->x = $x;
		$this->y = $y;
		$this->life = Config::$defaultPlayerHealth;
        $this->crystals = 0;
        $this->defaultChar = "*";
		$this->headChar = "o";
		
		$this->resetScore();

		// moisture stuff
		// a player can exist with +=$moistureMax.  
		// outside of those extremes, we either dry out, or waterlog.
		$this->setMoisture(0);
		$this->moistureMax = Config::$defaultMoistureMax;
		$this->moistureTicks = Config::$defaultMoistureTicks;
		$this->moistureTicker = $this->moistureTicks;

		// if our default state is invulnerable, crank up the duration
		$this->invulnerable = Config::$defaultPlayerInvulnerableState;
		$this->invulnerableDuration = $this->invulnerable ? 999999 : 0;

		$this->powers = array();
		$this->activePower = 0;
		$this->menuPower = 0;
	}

	public function reset($x, $y) {
		// reset moisture ticker
		$this->setMoisture(0);
		$this->coords = array([$x, $y]);
	}

	public function resetScore() {
		$this->scoreWater = 0;
		$this->scoreMinerals = 0;
		$this->scoreToxicWaste = 0;
		$this->scoreMoves = 0;
		$this->scoreGrown = 0;
		$this->scoreShrink = 0;
		$this->scoreCrystals = 0;		
		$this->scoreFossils = 0;
		$this->xp = 0;
	}

	public function calculateScore() {
		return ($this->scoreWater * Config::$defaultScoreWater) + 
			($this->scoreMinerals * Config::$defaultScoreMinerals) + 
			($this->scoreMoves * Config::$defaultScoreMoves) + 
			($this->scoreCrystals * Config::$defaultScoreCrystals) + 
			($this->scoreFossils * Config::$defaultScoreFossils);
	}

	// add a message with score, and details of
	public function getScoreScreen() {
		$scoreMsg = array(
			"Score: " . $this->calculateScore(),
			" ",
			"Water: " . $this->scoreWater,
			"Minerals: " . $this->scoreMinerals,
			"Toxic Waste: " . $this->scoreToxicWaste,
			"Crystals: " . $this->scoreCrystals,
			" ",
			"Moves: " . $this->scoreMoves,
			"Grown: " . $this->scoreGrown,
			"Shrunk: " . $this->scoreShrink,
			" ",
			"XP: " . $this->xp
		);

		return $scoreMsg;
	}

	public function addPower($p) {
		$this->powers[] = $p;
	}

	public function getPowers() {
		return $this->powers;
	}

	public function getPowersScreen() {
		$ret = array(
			" ",
			"Select Power:",
			" "
		);

		// if we have a power, show it.
		$ps = $this->getPowers();

		for ($x = 0; $x < count($ps); $x++) {
			$tmp = "  ";
			$p = $this->powers[$x];

			// if it's highlighted
			if ($this->getMenuPower() == $x) {
				$tmp .= "> ";
			} else {
				$tmp .= "  ";
			}

			if ($x == $this->activePower) {
				$tmp .= "[X] " . $ps[$x]->getName();
			} else {
				$tmp .= "[ ] " . $ps[$x]->getName();
			}
		
			$ret[] = $tmp;
		}
		
		$ret[] = " ";

		return $ret;
	}

	public function getMenuPower() {
		return $this->menuPower;
	}

	public function setMenuPower($p) {
		$this->menuPower = $p;
	}

	public function setActivePower($p) {
		$this->activePower = $p;
	}

	public function getActivePower() {
		return $this->activePower;
	}

	public function getPower() {
		if (!is_null($this->activePower) && isset($this->powers[$this->activePower])) {
			return $this->powers[$this->activePower];
		}

		return false;
	}

	public function isInvulnerable() {
		return $this->invulnerable;
	}

	public function setInvulnerable($v) {
		$this->invulnerable = $v;
	}

	public function setInvulnerableDuration($d) {
		$this->invulnerableDuration = $d;
	}

	public function getInvulnerableDuration() {
		return $this->invulnerableDuration;
	}
	
	public function decrInvulnerableDuration($d = null) {
		if (is_null($d)) {
			$this->invulnerableDuration--;
		} else {
			$this->invulerableDuration = $this->invulnerableDuration - $d;
		}

		return $this->invulnerableDuration;
	}

	// every move a player makes, run some code
	public function tick() {
		if ($this->moistureTicker > 0) {
			$this->moistureTicker--;
		} else {
			// reset the moisture ticker
			$this->moistureTicker = $this->moistureTicks;

			$this->decrMoisture();
		}

		$this->scoreMoves++;
	}

	public function incrMoisture($inc = null) {
		if (is_null($inc)) {
			$this->moisture++;
			$this->waterScore++;
		} else {
			$this->moisture = $this->moisture + $inc;
			$this->scoreWater = $this->scoreWater + $inc;
		}

		return $this->moisture;
	}

	public function setMoistureTicker($val) {
		$this->moistureTicker = $val;
	}
	
	public function decrMoisture() {
		return $this->moisture--;
	}

	public function getMoisture() {
		return $this->moisture;
	}

	public function getMoistureTicks() {
		return $this->moistureTicks;
	}

	public function setMoisture($m) {
		$this->moisture = $m;
	}

	public function getHeadChar() {
		return $this->headChar;
	}
	
    public function getDefaultChar() {
        return $this->defaultChar;
    }

    public function getCrystals() {
        return $this->crystals;
    }

	public function getMoistureMax() {
		return $this->moistureMax;
	}

    public function incrCrystals() {
        $this->crystals++;
		$this->scoreCrystals++;
    }

    public function decrCrystals() {
        if ($this->crystals > 0) {
            $this->crystals--;

            return true;
        }

        return false;
    }

	public function getSize() {
		return count($this->coords);
	}

	public function getLife() {
		return $this->life;
	}

	public function setLife($life) {
		$this->life = $life;
	}

	/**
	 * decrement the players life
	 */
	public function decrLife($amt = null) {
		if (is_null($amt)) {
			// for every 5 long, we had an additional life penalty
			$additional = ceil(count($this->coords) / 5);

			$this->life = $this->life - $additional;

			return $this->life;
		} else {
			$this->life = $this->life - $amt;

			return $this->life;
		}
	}

	// the player object expands
	public function addCoord($x, $y) {
		if (!in_array([$x, $y], $this->coords)) {
			$this->coords[] = array($x, $y);
		}

		$this->scoreGrow++;
	}

	// put the head where the tail is
	public function reverseCoords() {
		// reverse the array
		$this->coords = array_reverse($this->coords);

		$xy = $this->coords[0];

		// set the x/y to the new head
		
		$this->x = $xy[0];
		$this->y = $xy[1];
	}

	public function addFrontCoord($x, $y) {
		array_unshift($this->coords, array($x, $y));
	}

	public function getFrontCoord() {
		return $this->coords[0];
	}

	public function coordPop() {
		return array_pop($this->coords);
	}
	
	public function setX($x) {
		$this->x = $x;

		// add the coords to our list
		$this->addFrontCoord($x, $this->getY());
	}

	public function getX() {
		return $this->x;
	}

	public function getY() {
		return $this->y;
	}

	public function setY($y) {
		$this->y = $y;

		// add the coords to our list
		$this->addFrontCoord($this->getX(), $y);
	}

	public function setXY($x, $y) {
		$this->x = $x;
		$this->y = $y;

		$this->addFrontCoord($x, $y);
	}

	/**
	 * returns the coords property (array)
	 */
	public function getCoords() {
		return $this->coords;
	}

	public function onCoords($x, $y) {
		if (in_array([$x, $y], $this->coords)) {
			return true;
		}

		return false;
	}

	public function getXP() {
		return $this->xp;
	}

	public function addXP($add) {
		$this->xp = $this->xp + $add;
	}
	
	public function setXP($xp) {
		$this->xp = $xp;
	}
}