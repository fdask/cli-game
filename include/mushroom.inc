<?php
class Mushroom implements PlantInterface {
    public $life;
    public $maxLife;
    public $ticks;
    public $ticker;

    public function __construct() {
        $this->life = 10;
        $this->maxLife = 10;

        $this->ticks = 5;
        $this->ticker = 5;
    }

    public function getLife() {
        return $this->life;
    }

    public function getMaxLife() {
        return $this->maxLife;
    }

    public function decrLife($amt = null) {
        if (is_null($amt)) {
            $this->life--;
        } else {
            $this->life = $this->life - $amt;
        }
    }
    
    public function tick() {
        $this->ticker--;

        if ($this->ticker <= 0) {
            $this->life--;

            $this->ticker = $this->ticks;
        }
    }

    public function water() {
        // water has no effect on a mushroom
    }

    public function __toString() {
        return Colors::fg_color(Config::$mushroomColor, "M");
    }
}