<?php

class Meteor {
    // the direction the meteor is travelling
    public $direction;
    
    // the amount of toxicity left behind on impact
    public $weight;

    // how many tiles left/right the meteor comes down
    public $speed;

    public $ticks;
    public $ticker;
 
    // where the head of the meteor is
    private $x;
    private $y;

    private $headChar;
    private $tailLeftChar;
    private $tailRightChar;

    // the trail the meteor leaves as it travels
    private $tailCoords;

    public function __construct($dir, $speed) {
        $this->direction = $dir;
        $this->weight = rand(5, 9);
        $this->speed = $speed;
        $this->headChar = json_decode('"\u00a4"');
        $this->tailLeftChar = json_decode('"\u2517"');
        $this->tailRightChar = json_decode('"\u251b"');
        $this->ticker = 3;
        $this->tailCoords = array();
    }

    public function tick() {
        if ($this->ticker > 0) {
            $this->ticker--;

            return true;
        } else {
            // time to go!  reset the ticker
            $this->ticker = $this->ticks;

            return false;
        }
    }

    public function getX() {
        return $this->x;
    }

    public function getY() {
        return $this->y;
    }
    
    public function getXY() {
        return [$this->getX(), $this->getY()];
    }

    // because we move diagonally, we update both xy at once
    // every move, update the tail coords
    public function setXY($x, $y) {
        $this->tailCoords[] = [$x, $y];
        
        $this->x = $x;
        $this->y = $y;
    }

    public function getTailCoords() {
        return $this->tailCoords;
    }

    public function getHeadChar() {
        return $this->headChar;
    }

    public function getTailChar() {
        if ($this->direction = "L") {
            return $this->tailLeftChar;
        } else if ($this->direction = "R") {
            return $this->tailRightChar;
        }
    }

    public function getDirection() {
        return $this->direction;
    }

    public function setDirection($d) {
        $this->direction = $d;
    }

    public function getSpeed() {
        return $this->speed;
    }
}