<?php
class CollectObjective extends Objective {
    public $what;
    public $amount;
    
    public function __construct($what, $amount) {
        $this->amount = $amount;
        $this->what = $what;

        $this->shortName = $this->amount . " " . ucfirst($this->what);
    }
}
