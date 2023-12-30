<?php
class Portal {
    public $x;

    public $y;

    public $visible;

    public function __construct($x, $y) {
        $this->x = $x;
        $this->y = $y;
        $this->visible = false;
    }

    public function setVisible() {
        $this->visible = true;
    }

    public function isVisible() {
        return $this->visible;
    }

    public function __toString() {
        if ($this->visible) {
            return "@";
        }
    }

    public function setXY($x, $y) {
        $this->x = $x;
        $this->y = $y;
    }

    public function getXY() {
        return [$this->x, $this->y];
    }
}