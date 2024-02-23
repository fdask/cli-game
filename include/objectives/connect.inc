<?php
namespace Objectives;

class Connect extends \Objective {
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