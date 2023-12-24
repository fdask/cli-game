<?php
class Dirt {
	use Wetable;

	/*  integer representing how much moisture is in the dirt
	*/
	// integer representing the concentration of minerals
	private $concentration;

	// the single ascii character dirt shows up on the map as
	public $defaultChar;

	// if we are topsoil and have a plant, here it is
	public $plant;

	// boolean indicate if we have a crystal here or not
    public $crystal;

	// int level of toxicity
	public $toxicity;

	public function __construct($rocked = false) {
		$this->setWetness(0);
		$this->setConcentration(0);
		$this->defaultChar = ".";
		$this->plant = null;
        $this->crystal = false;
		$this->toxicity = 0;
	}

	public function isToxic() {
		return $this->toxicity;
	}

	public function setToxicity($t) {
		return $this->toxicity = $t;
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

	public function updatePlant() {
		if (!empty($this->plant) && $this->isWet()) {
			$this->plant->fullLife();
		} else if (!empty($this->plant)) {
			if ($this->plant->getLife() == 0) {
				$maxLife = $this->plant->maxLife;

				// kill the plant
				$this->plant = null;

				// add a mineral where the dead plant was
				$this->setConcentration($this->getConcentration() + $maxLife);
			} else {
				$this->plant->tick();
			}
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

	public function hasMinerals() {
		return $this->concentration > 0;
	}

	public function getView($viewType = 1) {
		// wetness view
		if ($viewType == 1) {
			$wetness = $this->getWetness();

			if ($wetness > 0) { 
				if ($wetness > 9) {
					return "9";
				} else {
					return "$wetness";
				}
			} else {
				return $this->defaultChar;
			}
		} else if ($viewType == 2) {
			// mineral view
			$concentration = $this->getConcentration();

			if ($concentration > 0) {
				if ($concentration > 9) {
					return "9";
				} else {
					return "$concentration";
				}
			} else {
				return $this->defaultChar;
			}
		} else if ($viewType == 3) {
			// plant matter (plants, crystals, toxicity) view
			if ($this->isToxic()) {
				// magic number
				return "T";
			} else if (!empty($this->plant)) {
				return "{$this->plant}";
			} else if ($this->isCrystal()) {
				// magic number
                return "C";
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

	public function incrConcentration() {
		$this->concentration++;
	}

	public function decrConcentration() {
		if ($this->concentration > 0) {
			$this->concentration--;
		}
	}

	public function __toString() {
		return $this->getView(1);
	}
}