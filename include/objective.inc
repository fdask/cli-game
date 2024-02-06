<?php
class Objective {
    // boolean indicating if the objective is complete
    public $completed;

    public $rewardXP;

    public $shortName;

    /**
     * collect X of Y
     * - passed in as 
     * bridge two points
     * 
     */
    public $type;

    public $rewards;

    public function __construct() {
        $this->completed = false;
    }

    public function setCompleted() {
        $this->completed = true;
    }

    public function getCompleted() {
        return $this->completed;
    }

    public function getRewardXP() {
        return $this->rewardXP;
    }
    
    public function getShortName() {
        return $this->shortName;
    }
}