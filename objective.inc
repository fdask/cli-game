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

class CollectObjective extends Objective {
    public $what;
    public $amount;
    
    public function __construct($what, $amount) {
        $this->amount = $amount;
        $this->what = $what;

        $this->shortName = $this->amount . " " . ucfirst($this->what);
    }
}

class ConnectObjective extends Objective {
    public $x1;
    public $x2;
    public $y1;
    public $y2;

    public function __construct($x1, $y1, $x2, $y2) {
        $this->x1 = $x1;
        $this->y1 = $y1;
        $this->x2 = $x2;
        $this->y2 = $y2;

        $this->shortName = "$x1, $y1 to $x2, $y2";
    }
}