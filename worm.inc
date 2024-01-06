<?php
class Worm {
    // sometimes is nice, but not all the time
    public $x;
    public $y;
    public $visible;
    public $health;
    public $length;
    public $coords;

    public function __construct($x, $y) {
        $this->x = $x;
        $this->y = $y;
        $this->coords = array([$x, $y]);
    }

    public function setLength($l) {
        $this->length = $l;
    }

    public function getLength() {
        return $this->length;
    }

    public function setVisible($v) {
        $this->visible = $v;
    }

    public function getVisible() {
        return $this->visible;
    }

    public function setHealth($h) {
        $this->health = $health;
    }

    public function getHealth() {
        return $this->health;
    }

    public function getX() {
        return $this->x;
    }

    public function setX($x) {
        $this->x = $x;
    }

    public function getY() {
        return $this->y;
    }

    public function setY($y) {
        $this->y = $y;
    }

    public function setXY($x, $y) {
        $this->x = $x;
        $this->y = $y;
    }

    public function addCoord($x, $y) {
        $this->coords[] = [$x, $y];

        if (count($this->coords) > $this->length) {
            array_shift($this->coords);
        }
    }

    public function getCoords() {
        return $this->coords();
    }

    public function setCoords($coords) {
        $this->coords = $coords;
    }

    /**
     * check to see if this worm is on a specific coords
     */
    public function hasCoord($x, $y) {
        return in_array([$x, $y], $this->coords);
    }
    /**
     * every turn, tick down the worms health
     * 
     * eventually leading to it going invisible
     */
    public function tick() {
        if ($this->visible) {
            $this->health--;
            
            if ($this->health <= 0) {
                $this->visible = false;
            }
        }
    }
}
?>