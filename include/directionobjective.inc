<?php
class DirectionObjective extends Objective {
    private $direction;

    public function __construct($direction) {
        $this->direction = $direction;

        $this->shortName = "Face $direction";
    }

    public function getDirection() {
        return $this->direction;
    }
}