<?php
class Nibbit {
    use Treadable;

    private $turned;

    private $strength;

    private $x;
    private $y;

    public static $defaultChar = "N";
    
    public function __construct($x, $y, $strength = 1, $turned = false) {
        $this->x = $x;
        $this->y = $y;
        $this->turned = $turned;
        $this->treadable = false;
        $this->strength = $strength;
    }

    public function turn() {
        $this->turned = !$this->turned;
    }

    public function isTurned() {
        return $this->turned;
    }

    public function tick() {
        // do nothing so far
    }

    public function getXY() {
        return [$this->x, $this->y];
    }

    public function getField($mapWidth) {
        $x = $this->x;
        $y = $this->y;

        $ret = array([$x, $y]);

        if ($this->strength > 1) {
            $ret = array_merge($ret, array(
                [$x - 1, $y],
                [$x + 1, $y],
                [$x - 1, $y - 1],
                [$x, $y - 1],
                [$x + 1, $y - 1],
                [$x - 1, $y + 1],
                [$x, $y + 1],
                [$x + 1, $y + 1]
            ));
        } 

        if ($this->strength > 2) {
            $ret = array_merge($ret, array(
                [$x - 2, $y - 2],
                [$x - 2, $y - 1],
                [$x - 2, $y],
                [$x - 2, $y + 1],
                [$x - 2, $y + 2],
                [$x - 1, $y - 2],
                [$x - 1, $y + 2],
                [$x, $y - 2],
                [$x, $y + 2],
                [$x + 1, $y - 2],
                [$x + 1, $y + 2],
                [$x + 2, $y - 2],
                [$x + 2, $y - 1],
                [$x + 2, $y],
                [$x + 2, $y + 1],
                [$x + 2, $y + 2]
            ));
        }

        for ($i = 0; $i < count($ret); $i++) {
            if ($ret[$i][1] >= $mapWidth) {
                $ret[$i][1] = $ret[$i][1] - $mapWidth;
            } else if ($ret[$i][1] < 0) {
                $ret[$i][1] = $ret[$i][1] + $mapWidth;
            }
        }

        return $ret;
    }
}