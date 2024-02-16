<?php
class ThunderCycle {
    private $distance;
    private $thunder;
    private $lightning;
    private $countdownTicker;
    private $cycle;
    private $height;
    private $width;

    public function __construct($height, $width, $distance) {
        $this->distance = $distance;
        $this->height = $height;
        $this->width = $width;
        $this->countdownTicker = rand(1, 10);
        $this->cycle = "RANDOM";
        $this->thunder = null;
        $this->lightning = null;
    }

    public function tick() {
        switch ($this->cycle) {
            case "RANDOM":
                $this->countdownTicker--;

                if ($this->countdownTicker == 0) {
                    $this->cycle = "THUNDER";
                    $this->thunder = new Thunder();
                    $this->thunder->flash();
                    $this->countdownTicker = $this->distance;
                }

                break;
            case "THUNDER":
                $this->thunder = null;
                $this->cycle = "DISTANCE";

                break;
            case "DISTANCE":
                $this->countdownTicker--;

                if ($this->countdownTicker == 0) {
                    $this->cycle = "LIGHTNING";
                    $this->lightning = new Lightning(rand(0, $this->width - 1), $this->height, $this->width);
                }

                break;
            case "LIGHTNING":
                $this->lightning = null;
                $this->cycle = "FINISH";
                
                break;
            case "FINISH":
                $this->lightning = null;
                $this->thunder = null;

                break;
        }
    }

    public function isThunder() {
        if ($this->cycle == "THUNDER") {
            return true;
        }

        return false;
    }

    public function isLightning() {
        if ($this->cycle == "LIGHTNING") {
            return true;
        }

        return false;
    }

    public function isFinished() {
        if ($this->cycle == "FINISH") {
            return true;
        }

        return false;
    }

    public function reset($distance = false) {
        $this->cycle = "RANDOM";
        $this->countdownTicker = rand(1, 10);
        $this->thunder = null;
        $this->lightning = null;
        
        if ($distance) {
            $this->distance = $distance;
        }
    }

    public function getLightningCoords() {
        if (!is_null($this->lightning)) {
            return $this->lightning->getCoords();
        }

        return array();
    }

    public function getLightningChar($x, $y) {
        if (!is_null($this->lightning)) {
            return $this->lightning->getChar($x, $y);
        }

        return false;
    }

    public function __toString() {
        return "T: " . $this->countdownTicker . " Cycle: " . $this->cycle;
    }
}