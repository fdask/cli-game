<?php
/**
 * impassable, rock
 */
class Rock {
	use Traversible;

	public function __construct() {
		$this->setTraversible(false);
	}

	public function __toString() {
		return json_decode('"\u2599"');
	}
}