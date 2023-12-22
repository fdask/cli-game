<?php
/**
 * impassable, rock
 */
class Rock {
	public function __construct() {

	}

	public function __toString() {
		return json_decode('"\u2599"');
	}
}