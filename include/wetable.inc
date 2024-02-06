<?php
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
?>