<?php
namespace AStar;

class Obstacle {
    public $x;
    public $y;

    public function __construct($x, $y) {
        $this->x = $x;
        $this->y = $y;
    }

    public function getDest() {
        return false;
    }

    public function getOrig() {
        return false;
    }

    public function __toString() {
        return "X";
    }
}

class Node {
    public $x;
    public $y;
    public $g;
    public $h;
    public $f;
    public $orig;
    public $dest;
    public $parentX;
    public $parentY;
    public $isPath;

    public function __construct($x, $y) {
        $this->x = $x;
        $this->y = $y;
        $this->f = 0;
        $this->orig = false;
        $this->dest = false;
        $this->isPath = false;
    }

    public function setPath($p) {
        $this->isPath = $p;
    }

    public function isPath() {
        return $this->isPath;
    }

    public function getX() {
        return $this->x;
    }

    public function getY() {
        return $this->y;
    }

    public function getXY() {
        return [$this->x, $this->y];
    }

    public function getParentX() {
        return $this->parentX;
    }

    public function setParentX($x) {
        $this->parentX = $x;
    }

    public function getParentY() {
        return $this->parentY;
    }

    public function setParentY($y) {
        $this->parentY = $y;
    }

    public function setParentXY($x, $y) {
        $this->parentX = $x;
        $this->parentY = $y;
    }

    public function getParentXY() {
        return [$this->parentX, $this->parentY];
    }

    public function getF() {
        return $this->f;
    }

    public function setF($f) {
        $this->f = $f;
    }

    public function setG($g) {
        $this->g = $g;
    }

    public function getG() {
        return $this->g;
    }

    public function setH($h) {
        $this->h = $h;
    }

    public function getH() {
        return $this->h;
    }

    public function getOrig() {
        return $this->orig;
    }

    public function setOrig($o) {
        $this->orig = $o;
    }

    public function getDest() {
        return $this->dest;
    }

    public function setDest($d) {
        $this->dest = $d;
    }

    public function __toString() {
        if ($this->orig) {
            return "O";
        } else if ($this->dest) {
            return "D";
        } else if ($this->isPath()) {
            return "*";
        } else {
            return ".";
        }
    }
}

class Map {
    public $map;

    public function __construct($array) {
        $keys = array_keys($array);
        $min_array = min($keys);
        $max_array = max($keys);

        for ($x = $min_array; $x <= $max_array; $x++) {
            for ($y = 0; $y < count($array[$x]); $y++) {
                if ($array[$x][$y] == "X") {
                    $this->map[$x][$y] = new Obstacle($x, $y);
                } else {
                    $this->map[$x][$y] = new Node($x, $y);

                    if ($array[$x][$y] == "O") {
                        $this->map[$x][$y]->setOrig(true);
                    } else if ($array[$x][$y] == "D") {
                        $this->map[$x][$y]->setDest(true);
                    }
                }
            }
        }
    }

    public function fileToMap($filename) {
        $lines = file($filename);
        $this->map = array();

        // populate the map, and extract origin/dest
        for ($x = 0; $x < count($lines); $x++) {
            for ($y = 0; $y < strlen(trim($lines[0])); $y++) {
                $bits = str_split(trim($lines[$x]));

                if ($bits[$y] == "X") {
                    $this->map[$x][$y] = new Obstacle($x, $y);
                } else {
                    $this->map[$x][$y] = new Node($x, $y);

                    if ($bits[$y] == "O") {
                        $this->map[$x][$y]->setOrig(true);
                    } else if ($bits[$y] == "D") {
                        $this->map[$x][$y]->setDest(true);
                    }
                }
            }
        }
    }

    public function setPath($a) {
        foreach ($a as $xy) {
            $x = $xy[0];
            $y = $xy[1];

            $this->map[$x][$y]->setPath(true);
        }
    }

    public function getDest() {
        $keys = array_keys($this->map);
        $min_array = min($keys);
        $max_array = max($keys);

        for ($x = $min_array; $x <= $max_array; $x++) {
            for ($y = 0; $y < count($this->map[$x]); $y++) {
                if ($this->map[$x][$y]->getDest()) {
                    return $this->map[$x][$y];
                }
            }
        }
    }

    public function getDestXY() {
        $n = $this->getDest();
        
        return [$n->getX(), $n->getY()];
    }

    public function getOrig() {
        $keys = array_keys($this->map);
        $min_array = min($keys);
        $max_array = max($keys);

        for ($x = $min_array; $x <= $max_array; $x++) {
            for ($y = 0; $y < count($this->map[$x]); $y++) {
                if ($this->map[$x][$y]->getOrig()) {
                    return $this->map[$x][$y];
                }
            }
        }
    }

    public function getOrigXY() {
        $n = $this->getOrig();

        return [$n->getX(), $n->getY()];
    }

    public function getNeighbours($x, $y) {
        $keys = array_keys($this->map);
        $minHeight = min($keys);
        $maxHeight = max($keys);
        $minWidth = 0;
        $maxWidth = count($this->map[$minHeight]) - 1;
        
        $ret = array();
        
        // up
        $upX = $x - 1;
        $upY = $y;
        
        if ($upX >= $minHeight && $this->map[$upX][$upY] instanceof Node) {
            $ret[] = $this->map[$upX][$upY];
        }
        
        // down
        $downX = $x + 1;
        $downY = $y;
        
        if ($downX <= $maxHeight && $this->map[$downX][$downY] instanceof Node) {
            $ret[] = $this->map[$downX][$downY];
        }
        
        // left
        $leftX = $x;
        $leftY = $y - 1;
        
        if ($leftY >= 0 && $this->map[$leftX][$leftY] instanceof Node) {
            $ret[] = $this->map[$leftX][$leftY];
        }
        
        // right
        $rightX = $x;
        $rightY = $y + 1;
        
        if ($rightY <= $maxWidth && $this->map[$rightX][$rightY] instanceof Node) {
            $ret[] = $this->map[$rightX][$rightY];
        }
        
        return $ret;
    }

    public function distanceBetween($x1, $y1, $x2, $y2) {
        return abs($x1 - $x2) + abs($y1 - $y2);
    }

    public function __toString() {
        $ret = "";
        $keys = array_keys($this->map);
        $min_array = min($keys);
        $max_array = max($keys);

        for ($x = $min_array; $x <= $max_array; $x++) {
            for ($y = 0; $y < count($this->map[$x]); $y++) {
                $ret .= $this->map[$x][$y];
            }
            
            $ret .= "\n";
        }

        $ret .= "\n";

        return $ret;
    }
}

class PathFinding {
    public $open;
    public $closed;
    public $map;

    public function __construct(Map $map) {
        $this->open = array();
        $this->closed = array();
        $this->map = $map;
    }

    public static function findPath($map) {
        $m = new Map($map);

        $pf = new PathFinding($m);

        return $pf->pathFind();
    }

    public function openAdd(Node $n) {
        // add or update.
        $nX = $n->getX();
        $nY = $n->getY();

        if (count($this->open)) {
            for ($x = 0; $x < count($this->open); $x++) {
                $lX = $this->open[$x]->getX();
                $lY = $this->open[$x]->getY();

                if ($nX == $lX && $nY == $lY) {
                    $this->open[$x] = $n;

                    return;
                }
            }
        }

        $this->open[] = $n;

        return;
    }

    public function nodeInOpen(Node $n) {
        for ($x = 0; $x < count($this->open); $x++) {
            if ($this->open[$x]->getX() == $n->getX() && $this->open[$x]->getY() == $n->getY()) {
                return true;
            }
        }

        return false;
    }

    public function getOpenG(Node $n) {
        for ($x = 0; $x < count($this->open); $x++) {
            if ($this->open[$x]->getX() == $n->getX() && $this->open[$x]->getY() == $n->getY()) {
                return $this->open[$x]->getG();
            }
        }

        return false;
    }

    public function nodeInClosed(Node $n) {
        for ($x = 0; $x < count($this->closed); $x++) {
            if ($this->closed[$x]->getX() == $n->getX() && $this->closed[$x]->getY() == $n->getY()) {
                return true;
            }
        }

        return false;
    }

    public function getOpen() {
        return $this->open;
    }

    public function getClosed() {
        return $this->closed;
    }

    public function closedAdd($n) {
        $this->closed[] = $n;
    }

    public function popLowestF() {
        $curF = 9999999;

        // remove and return the node with the lowest F value
        for ($x = 0; $x < count($this->open); $x++) {
            $newF = $this->open[$x]->getF();

            if ($newF < $curF) {
                $curF = $newF;
                $ret = $x;
            }
        }

        // now remove the index $ret
        return array_splice($this->open, $ret, 1)[0];
    }

    public function getPath() {
        // starting from the end, lets trace back to the start
        $dest = $this->map->getDest();
        $orig = $this->map->getOrig();

        $x = $dest->getX();
        $y = $dest->getY();

        $ret = array();
        
        do {
            $ret[] = [$x, $y];

            $newX = $this->map->map[$x][$y]->getParentX();
            $newY = $this->map->map[$x][$y]->getParentY();

            $x = $newX;
            $y = $newY;
        } while ([$x, $y] != $orig->getXY());

        return $ret;
    }

    public function pathFind() {
        $ret = array();

        $dest = $this->map->getDest();

        // add the starting square to the open list
        $orig = $this->map->getOrig();
        
        $this->openAdd($orig);

        while (!empty($this->getOpen())) {           
            // look for the lowest F cost Node in the open list.
            // this becomes the current square
            $current = $this->popLowestF();
            
            // switch the current square to the closed list
            $this->closedAdd($current);

            // if the square is our destination, break out of the loop
            if ($current->getX() == $dest->getX() && $current->getY() == $dest->getY()) {
                return $this->getPath();
            }

            // get all the current squares neighbours
            $neighbours = $this->map->getNeighbours($current->getX(), $current->getY());

            for ($x = 0; $x < count($neighbours); $x++) {
                if ($this->nodeInClosed($neighbours[$x])) {
                    continue;
                }
                
                // create the f, g, and h values
                $n = $neighbours[$x];
                $n->setG($current->getG() + 1);
                $n->setH($this->map->distanceBetween($n->getX(), $n->getY(), $dest->getX(), $dest->getY()));
                $n->setF($n->getG() + $n->getH());
                $n->setParentXY($current->getX(), $current->getY());

                if ($this->nodeInOpen($n)) {
                    // if it's not on the open list, add it.
                    if ($n->getG() > $this->getOpenG($n)) {
                        // continue to the beginning of the loop
                        continue;
                    }
                } 
                
                // add the child to the open list
                $this->openAdd($n);
            }
        }

        // no path could be found
        return null;
    }

    public function printOpen() {
        $ret = "";

        for ($x = 0; $x < count($this->open); $x++) {
            $ret .= "(" . $this->open[$x]->getX() . ", " . $this->open[$x]->getY() . ") -> ";
            $ret .= "F: " . $this->open[$x]->getF() . " ";
            $ret .= "P(" . $this->open[$x]->getParentX() . ", " . $this->open[$x]->getParentY() . ")\n";
        }

        $ret .= "\n";

        return $ret;
    }
}