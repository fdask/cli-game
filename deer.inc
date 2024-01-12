<?php
class Deer {
    public $y;
    
    public function __construct($y) {
        $this->setY($y);
    }

    public function setY($y) {
        $this->y = $y;
    }

    public function getY() {
        return $this->y;
    }
}
?>