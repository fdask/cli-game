<?php
class Compass {
    private $vpWidth;
    private $mapWidth;
    private $string;
    
    public function __construct($vpWidth, $mapWidth) {
        $this->vpWidth = $vpWidth;
        $this->mapWidth = $mapWidth;

        // we have to get north to north on mapWidth;
        $q = floor($this->mapWidth / 4);
        $south = $q * 2;
        $north = 0;
        $east = $q;
        $west = $q * 3;
        
        $this->string = array();

        for ($y = 0; $y < $this->mapWidth; $y++) {
            switch ($y) {
                case $north:
                    $this->string[] = "N";
                    break;
                case $east:
                    $this->string[] = "E";
                    break;
                case $south:
                    $this->string[] = "S";
                    break;
                case $west:
                    $this->string[] = "W";
                    break;
                default:
                    $this->string[] = "-";
            }
        }
    }

    public function turnClockwise() {
        // take a char from the left, add to the right
        $chr = array_shift($this->string);
        $this->string[] = $chr;
    }

    public function turnCounterClockwise() {
        // take a char from the right, add to the left
        $chr = array_pop($this->string);
        array_unshift($this->string, $chr);   
    }

    public function getString() {
        return implode("", $this->string);
    }

    public function getChar($yPos) {
        return $this->string[$yPos];
    }

    public function getDirection($yPos) {
        $leftCount = 0;
        $rightCount = 0;

        $y = $yPos;

        while (1) {
            if ($y < 0) {
                $y = $this->mapWidth + $y;
            }

            if (in_array($this->string[$y], array('N', 'E', 'W', 'S'))) {
                $dirLeft = $this->string[$y];

                break;
            }

            $y = $y - 1;
            $leftCount++;
        }

        $y = $yPos;

        while (1) {
            if ($y >= $this->mapWidth) {
                $y = $y - $this->mapWidth;
            }

            if (in_array($this->string[$y], array('N', 'E', 'W', 'S'))) {
                $dirRight = $this->string[$y];

                break;
            }

            $y = $y + 1;
            $rightCount++;
        }

        if ($rightCount < $leftCount || $rightCount == $leftCount) {
            return $dirRight;
        } else {
            return $dirLeft;
        }
    }
}
