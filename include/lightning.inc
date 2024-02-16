<?php
class Lightning {
    private $startY;
    private $chars;
    private $coords;
    
    public function __construct($startY, $height, $width) {
        $this->coords = array();

        $dir = rand(0, 1) ? 'L' : 'R';
        $lastChr = "";
        $chr = "";
        $yPos = $startY;

        for ($x = 0; $x < $height; $x++) {
            $lastChr = $chr;

            $chr = $this->getRandChar();

            if ($chr == "/") {
                $dir = "L";
                $yPos = $yPos - 1;
            } else if ($chr == "\\") {
                $dir = "R";
                $yPos = $yPos + 1;
            } else if ($chr == "_") {
                // stay on the same line
                $x = $x - 1;
                
                if ($dir == "L") {
                    $yPos = $yPos - 1;             
                } else if ($dir == "R") {
                    $yPos = $yPos + 1;
                }
            }

            if ($yPos < 0) {
                $yPos = $width + $yPos;
            }

            if ($yPos >= $width) {
                $yPos = $yPos - $width;
            }
    
            $coords[$x][$yPos] = $chr;
        }

        $this->coords = $coords;
    }

    private function getRandChar() {
        switch (rand(0, 3)) {
            case 0:
                $chr = "\\";

                break;
            case 1:
                $chr = "/";
                
                break;
            case 2:
                $chr = "|";

                break;
            case 3:
                $chr = "_";

                break;
        }

        return $chr;
    }

    public function getCoords() {
        $ret = array();

        foreach ($this->coords as $x => $value) {
            foreach ($value as $y => $chr) {
                $ret[] = [$x, $y];
            }
        }

        return $ret;
    }

    public function getChar($x, $y) {
        return $this->coords[$x][$y];
    }
}

//$x = new Lightning(15, 15, 10, 80);