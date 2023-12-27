<?php
class Sky {
	// things in this region of sky are added here
	use Wetable;

	public $defaultChar;

	public function __construct() {
		$this->setWetness(0);
		$this->defaultChar = "~";
	}

	public function __toString() {
		if ($this->isWet()) {
			return Colors::fg_color('blue', "|");
		} else {
			return $this->defaultChar;	
		}
	}
}