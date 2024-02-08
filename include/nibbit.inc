<?php
class Nibbit {
    use Treadable;

    private $turned;

    private $x;
    private $y;

    public static $defaultChar = "N";
    
    public function __construct($x, $y, $turned = false) {
        $this->x = $x;
        $this->y = $y;
        $this->turned = $turned;
        $this->treadable = false;
    }

    public function turn() {
        $this->turned = !$this->turned;
    }

    public function isTurned() {
        return $this->turned;
    }

    public function tick() {
        // do nothing so far
    }

    public function getXY() {
        return [$this->x, $this->y];
    }
}