<?php
class Dirt {
	use Wetable;
	use Treadable;

	// integer representing the concentration of minerals
	private $concentration;

	// the single ascii character dirt shows up on the map as
	public $defaultChar;

	// if we are topsoil and have a plant, here it is
	public $plant;

	// boolean indicate if we have a crystal here or not
    public $crystal;

	// how many turns between toxicity decrementing
	public $toxicTicks;

	// toxic ticker
	public $tticker;

	// int level of toxicity
	public $toxicity;

	// seeptick
	private $seepTicker;
	public $seepTicks;

	public function __construct($seepTicker) {
		$this->setWetness(0);
		$this->setConcentration(0);
		$this->defaultChar = ".";
		$this->plant = null;
        $this->crystal = false;
		$this->toxicity = 0;
		$this->seepTicker = $seepTicker;
		$this->seepTicks = $this->seepTicker;
		$this->setTreadable(true);
	}

	public function isToxic() {
		return $this->toxicity;
	}

	public function setToxicity($t) {
		return $this->toxicity = $t;
	}

	public function getToxicity() {
		return $this->toxicity;
	}

	public function setToxicTicks($tt) {
		$this->toxicTicks = $tt;
		$this->tticker = $this->toxicTicks;
	}

	public function resetSeep() {
		$this->seepTicks = $this->seepTicker;
	}

	public function tick() {
		// seepage
		if ($this->seepTicks > 0) {
			$this->seepTicks--;
		}

		if ($this->tticker > 0) {
			$this->tticker--;
		} else {
			$this->decrToxicity();
			$this->tticker = $this->toxicTicks;
		}

		$this->sweepDeadPlant();			
	}

	public function decrToxicity() {
		if ($this->toxicity > 0) {
			$this->toxicity--;
		}
	}

    public function isCrystal() {
        return $this->crystal;
    }

    public function setCrystal($bool) {
        $this->crystal = $bool;
    }

	public function sweepDeadPlant() {
		if (!is_null($this->plant) && $this->plant->getLife() <= 0) {
			$maxLife = $this->plant->getMaxLife();

			// kill the plant
			$this->plant = null;

			// add a mineral where the dead plant was
			$this->setConcentration($this->getConcentration() + $maxLife);
		}
	}

	public function updatePlant() {
		if (!empty($this->plant) && $this->isWet()) {
			$this->plant->water();
		} else if (!empty($this->plant)) {
			$this->plant->tick();
		} 
	}

	public function addPlant($plant) {
		if (is_null($this->plant)) {
			$this->plant = $plant;

			return true;
		}

		return false;
	}

	public function removePlant() {
		$this->plant = null;
	}

	public function hasPlant() {
		if (!is_null($this->plant)) {
			return true;
		}

		return false;
	}

	public function hasMinerals() {
		return $this->concentration > 0;
	}

	public function getView($viewType = 1) {
		// wetness view
		if ($viewType == 1) {
			$wetness = $this->getWetness();

			if ($wetness > 0) { 
				if ($wetness > 9) {
					return Colors::fg_color(Config::$rainColor, "9");
				} else {
					return Colors::fg_color(Config::$rainColor, "$wetness");
				}
			} else {
				return $this->defaultChar;
			}
		} else if ($viewType == 2) {
			// mineral view
			$concentration = $this->getConcentration();

			if ($concentration > 0) {
				if ($concentration > 9) {
					return Colors::fg_color(Config::$strongMineralColor, "9");
				} else {
					if ($concentration > 6) {
						return Colors::fg_color(Config::$strongMineralColor, "$concentration");
					} else {
						return Colors::fg_color(Config::$mineralColor, "$concentration");
					}
				}
			} else {
				return $this->defaultChar;
			}
		} else if ($viewType == 3) {
			// plant matter (plants, crystals, toxicity) view
			if ($this->isToxic()) {
				return Colors::fg_color(Config::$toxicWasteColor, "T");
			} else if (!empty($this->plant)) {
				return "{$this->plant}";
			} else if ($this->isCrystal()) {
                return Colors::fg_color(Config::$crystalColor, "C");
            } else {
				return $this->defaultChar;
			}
		}
	}

	public function setConcentration($c) {
		$this->concentration = $c;
	}

	public function getConcentration() {
		return $this->concentration;
	}

	public function incrConcentration($c = null) {
		if (!is_null($c)) {
			$this->concentration = $this->concentration + $c;
		} else {
			$this->concentration++;
		}
	}

	public function decrConcentration($c = null) {
		if ($this->concentration > 0) {
			if (!is_null($c)) {
				$this->concentration = $this->concentration - $c;
			} else {
				$this->concentration--;
			}
		}
	}

	public function __toString() {
		return $this->getView(1);
	}
}