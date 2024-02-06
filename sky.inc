<?php
class Sky {
	// things in this region of sky are added here
	use Wetable;
	use Treadable;

	public $defaultChar;

	public function __construct() {
		$this->setWetness(0);
		$this->defaultChar = "~";
		$this->setTreadable(false);
	}

	public function __toString() {
		if ($this->isWet()) {
			return Colors::fg_color(Config::$rainColor, "|");
		} else {
			return $this->defaultChar;	
		}
	}
}