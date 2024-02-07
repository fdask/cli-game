<?php
class WormHole {
    public $src;
    public $dest;
    public $twoway;
    public static $defaultChar = "O";

    public function __construct($x1, $y1, $x2, $y2, $tw = false) {
        $this->src = [$x1, $y1];
        $this->dest = [$x2, $y2];
        $this->twoway = $tw;
    }

    public function getSrc() {
        return $this->src;
    }

    public function getDest() {
        return $this->dest;
    }

    public function getTwoWay() {
        return $this->twoway;
    }

    public function isTwoWay() {
        return $this->getTwoWay();
    }

    public function setTwoWay($tw) {
        $this->twoway = $tw;
    }

    public function checkCoords($x, $y) {
        if ($this->src == [$x, $y] || $this->dest == [$x, $y]) {
            return true;
        }

        return false;
    }
}