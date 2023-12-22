<?php
// a message read when the given x coordinate is reached

class messageXY {
    use PointOfInterest;

    private $x;
    private $y;

    public function __construct($msg, $x, $y) {
        $this->setXY($x, $y);
        $this->setMessage($msg);
        $this->displayed = false;
    }
    public function getXY() {
        return [$this->x, $this->y];
    }

    public function setXY($x, $y) {
        $this->x = $x;
        $this->y = $y;
    }
}

class messageY {
    use PointOfInterest;

    private $y;

    public function __construct($msg, $value) {
        $this->setY($value);
        $this->setMessage($msg);
        $this->displayed = false;
    }
public function getY() {
        return $this->y;
    }

    public function setY($y) {
        $this->y = $y;
    }
}

class messageX {
    use PointOfInterest;

    private $x;

    public function __construct($msg, $value) {
        $this->setX($value);
        $this->setMessage($msg);
        $this->displayed = false;
    }

    public function getX() {
        return $this->x;
    }

    public function setX($x) {
        $this->x = $x;
    }
}

trait PointOfInterest {
    private $msg;
    private $displayed;

    public function getMessage() {
        return $this->msg;
    }

    public function setMessage($msg) {
        $this->msg = $msg;
    }

    public function setDisplayed() {
        $this->displayed = true;
    }

    public function getDisplayed() {
        return $this->displayed;
    }
}

// my little pills are yellow
// your little pills are green
// lets mix them all together
// and find out what they mean
?>