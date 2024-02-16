<?php
class Storm {
    // how long the storm takes to pass
    private $size;

    // how long the storm has been on a particular distance
    private $sizeTicker;

    private $thunderCycle;

    // how far away the storm is
    private $distance;
    private $startDistance;
    
    // true, coming towards.  false, moving away.
    private $direction;

    // boolean.  is the storm over?
    private $finished;

    public function __construct($height, $width) {
        // magic number
        $this->size = rand(1, 10);
        $this->sizeTicker = $this->size;
        $this->startDistance = rand(1, 10);
        $this->distance = $this->startDistance;
        $this->finished = false;
        
        // add a new thundercycle
        $this->thunderCycle = new ThunderCycle($height, $width, $this->distance);
    }

    public function tick() {
        // when we complete all thunder and lightning cycles
        if (!$this->thunderCycle->isFinished()) {
            $this->thunderCycle->tick();
        } else {
            $this->sizeTicker--;
            
            if ($this->sizeTicker == 0) {
                if (!$this->direction) {
                    $this->distance--;

                    if ($this->distance == 0) {
                        $this->changeDirection();
                    }
                } else {
                    $this->distance++;

                    if ($this->distance == $this->startDistance) {
                        $this->finished = true;
                    }
                }

                $this->thunderCycle->reset($this->distance);
                $this->sizeTicker = $this->size;
            }
        }
    }

    private function changeDirection() {
        $this->direction = (!$this->direction) ? true : false;
    }

    public function isStormActive() {
        return !$this->finished;
    }

    public function isFinished() {
        return $this->finished;
    }

    public function isThunder() {
        return $this->thunderCycle->isThunder();
    }

    public function isLightning() {
        return $this->thunderCycle->isLightning();
    }

    public function getLightningCoords() {
        return $this->thunderCycle->getLightningCoords();
    }

    public function getLightningChar($x, $y) {
        return $this->thunderCycle->getLightningChar($x, $y);
    }

    public function __toString() {
        return "| Direction: " . ((!$this->direction) ? "towards" : "away") . " Distance: " . $this->distance . " Thunder Cycles: " . $this->sizeTicker . "/" . $this->size . " " . $this->thunderCycle;
    }
}