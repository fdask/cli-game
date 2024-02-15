<?php
class Flood {
    // this much water will be released per tile
    public $strength;
    
    // how long the water pools on the tiles before descent
    public $duration;

    // current duration
    private $durationTicker;

    // the width of the flood plain
    public $width;

    // where the floodplain starts
    public $startY;

    public function __construct($width, $startY) {
        $this->strength = Config::$defaultFloodStrength;
        $this->duration = $this->getRandomDuration();
        $this->durationTicker = 0;
        
        $this->width = $width;     
        $this->startY = $startY;
    }

    public function tick() {
        $this->durationTicker++;
    }

    public function isOver() {
        return $this->durationTicker >= $this->duration;
    }

    public function getRandomDuration() {
        return rand(Config::$defaultFloodDurationMin, Config::$defaultFloodDurationMax);
    }

    public static function getRandomWidthPercentage() {
        return rand(Config::$defaultFloodWidthMin, Config::$defaultFloodWidthMax);
    }

    public function __toString() {
        return Colors::bg_color(Config::$floodColor, ($this->strength < 9) ? $this->strength : 9);
    }
}