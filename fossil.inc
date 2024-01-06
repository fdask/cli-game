<?php
class Fossil {
    public $quality;

    public function __construct($q = null) {
        if (!is_null($q)) {
            $this->quality = $q;
        } else {
            $this->quality = rand(Config::fossilQualityMin, Config::fossilQualityMax);
        }
    }

    public function __toString() {
        return "%";
    }

    public function getQuality() {
        return $this->quality;
    }

    public function setQuality($q) {
        $this->quality = $q;
    }
}
?>