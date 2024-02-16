<?php
class Thunder {
    private $lit;

    public function __construct() {
        $this->lit = false;
    }

    public function isLit() {
        return $this->lit;
    }

    public function flash() {
        $this->lit = true;
    }
    
    public function tick() {
        if ($this->lit) {
            $this->lit = false;
        } else {
            $this->lit = true;
        }
    }
}