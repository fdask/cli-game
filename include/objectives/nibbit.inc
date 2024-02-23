<?php
namespace Objectives;

class Nibbit extends \Objective {
    public $allInState;
    
    public function __construct($allInState) {
        $this->allInState = $allInState;

        $this->shortName = "All nibbits " . (($this->allInState) ? "on" : "off");
    }
}