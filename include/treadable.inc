<?php
trait Treadable {
    public $treadable;

    public function setTreadable($val) {
        $this->treadable = $val;
    }

    public function getTreadable() {
        return $this->treadable;
    }

    public function isTreadable() {
        return $this->getTreadable();
    }
}