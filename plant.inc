<?php
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

	// a countdown
	private $ticker;

	// how high should the countdown be
	private $ticks;

	public function __construct() {
		$this->life = rand(5, 9);
		$this->maxLife = $this->life;
		$this->stage = 2;
		$this->ticks = 3;
		$this->ticker = $this->ticks;
	}

	public function tick() {
		if ($this->ticker == 0) {
			$this->decrLife();
			$this->ticker = $this->ticks;
		} else {
			$this->ticker--;
			echo "Decrementing ticker!\n";
		}
	}

	/*public function updatePlant($water) {
		// this is a plant internal tick()
		// check if we've had water recently
		if (is_null($water)) {
			echo "Calling tick!\n";
			$this->tick();

			if ($this->life <= 0) {
				// character is dead
				$this->stage = 4;
			} 
		}
	}*/

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
