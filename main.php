#!/usr/bin/php
<?php
// define some system variables
include 'dirt.inc';
include 'mailbox.inc';
include 'rock.inc';
include 'map.inc';
include 'player.inc';
include 'sky.inc';
include 'plant.inc';

$skyHeight = 10;
$groundHeight = 15;
$vpWidth = 40;
$mapWidth = 50;
$maxRain = 8;
$maxMinerals = 15;

$p = new Player($skyHeight, 20);
$map = new Map($skyHeight, $groundHeight, $vpWidth, $mapWidth, $maxRain, $maxMinerals, $p);
echo $map;

trait Alphabet {
	public function newFunction() {
		echo "hello";
	}
}

trait Wetable {
	public $wetness;

	public function setWetness($w) {
		$this->wetness = $w;
	}
	
	public function getWetness() {
		return $this->wetness;
	}

	public function incrWetness() {
		return $this->wetness++;
	}

	public function decrWetness() {
		if ($this->wetness > 0) {
			return $this->wetness--;
		}

		return $this->wetness;
	}

	public function isWet() {
		return $this->wetness > 0;
	}
}