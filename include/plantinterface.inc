<?php
interface PlantInterface {
    public function getLife();
    public function getMaxLife();
    public function decrLife($amount);
    public function tick();
    public function water();
    public function __toString();
}