<?php
class Plant {
	/**
	 * 	the lifecycle of a plant goes from
	 *  2 - plant
	 *  3 - enlightened
	 *  4 - dead plant
	 */
	public $stage;

	// turns without water we can live
	public $life;
	public $maxLife;

	public $wisdom;

	// a countdown
	private $ticker;

	// how high should the countdown be
	private $ticks;

	public function __construct($ticks) {
		$this->life = rand(5, 9);
		$this->maxLife = $this->life;
		$this->stage = 2;
		$this->wisdom = 0;
		$this->ticks = $ticks;
		$this->ticker = $this->ticks;
	}

	public function setTicks($ticks) {
		$this->ticks = $ticks;
	}

	public function tick() {
		if ($this->ticker == 0) {
			$this->decrLife();
			$this->ticker = $this->ticks;
		} else {
			$this->ticker--;
		}
	}

	public function decrLife($l = null) {
		if (is_null($l)) {
			$l = 1;
		}

		$this->life = $this->life - $l;

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

	public function water() {
		$this->wisdom++;
		$this->life = $this->maxLife;
	}

	public function __toString() {
		if ($this->stage == 2) {
			if ($this->wisdom > 2) {
				if ($this->life < 10) {
					return Colors::bg_color(Config::$enlightenedPlantColor, "{$this->life}");
				} else {
					return Colors::bg_color(Config::$enlightenedPlantColor, "9");
				}
			} else {
				if ($this->life < 10) {
					return Colors::fg_color(Config::$plantColor, "{$this->life}");
				} else {
					return Colors::fg_color(Config::$plantColor, "9");
				}
			}
		} else if ($this->stage == 4) {
			return "X";
		}
	}
}
