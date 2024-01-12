<?php
class Deer {
    public $y;
    public $toxicity;
    
    public function __construct($y) {
        $this->setY($y);
        $this->toxicity = 0;
    }

    public function setY($y) {
        $this->y = $y;
    }

    public function getY() {
        return $this->y;
    }

    public function getToxicity() {
        return $this->toxicity;
    }

    public function setToxicity($t) {
        $this->toxicity = $t;
    }

    public function addToxicity($t) {
        $this->toxicity = $this->toxicity + $t;

        return $this->toxicity;
    }

    /**
     * indicate deers toxicity with different colors
     */
    public function __toString() {
        $t = $this->getToxicity();

        if ($t < Config::$defaultDeerToxicAt) {
            // not toxic
            return Colors::fg_color(Config::$deerColor, "D");
        }

        // toxic
        return Colors::bg_color(Config::$deerColor, "D");
    }
}
?>