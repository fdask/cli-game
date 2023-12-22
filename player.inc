<?php
class Player {
	private $x;
	private $y;

	private $coords;

	private $life;

	public function __construct($x, $y) {
		$this->coords[] = array($x, $y);

		$this->x = $x;
		$this->y = $y;
		$this->life = 100;
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

	public function decrLife() {
		$this->life = $this->life - count($this->coords);

		return $this->life;
	}

	// the player object expands
	public function addCoord($x, $y) {
		if (!in_array([$x, $y], $this->coords)) {
			$this->coords[] = array($x, $y);
		}
	}

	public function addFrontCoord($x, $y) {
		array_unshift($this->coords, array($x, $y));
	}

	public function coordPop() {
		array_pop($this->coords);
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