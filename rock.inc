<?php
/**
 * impassable, rock
 */
class Rock {
	use Treadable;

	public function __construct() {
		$this->setTreadable(false);
	}

	public function __toString() {
		return json_decode('"\u2599"');
	}
}