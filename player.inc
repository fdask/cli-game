<?php
class Player {
	private $x;
	private $y;

	private $coords;

	private $life;

    private $crystals;

	// what to show for the body of the player
    private $defaultChar;

	// what character for the head of the char
	private $headChar;

	public function __construct($x, $y) {
		$this->coords[] = array($x, $y);

		$this->x = $x;
		$this->y = $y;
		$this->life = 1000;
        $this->crystals = 0;
        $this->defaultChar = "*";
		$this->headChar = "o";
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

    public function incrCrystals() {
        $this->crystals++;
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
	public function decrLife() {
		// for every 5 long, we had an additional life penalty
		$additional = ceil(count($this->coords) / 5);

		$this->life = $this->life - $additional;

		return $this->life;
	}

	// the player object expands
	public function addCoord($x, $y) {
		if (!in_array([$x, $y], $this->coords)) {
			$this->coords[] = array($x, $y);
		}
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

	public function getCoords() {
		return $this->coords;
	}

	public function onCoords($x, $y) {
		if (in_array([$x, $y], $this->coords)) {
			return true;
		}

		return false;
	}
}