<?php
class TitleScreen {
    public static function draw() {
        $ret = "my/ce/li/um\n";
        $ret .= "\n";
        $ret .= "1) Story Mode\n";
        $ret .= "2) Score Attack\n";
        $ret .= "\n";

        return $ret;
    }

    public static function run() {
        // wait for user input on game type
        $term = `stty -g`;
		system("stty -icanon -echo");

        // display the title screen
		echo self::draw();

		// keypress handler
		while ($c = fread(STDIN, 1)) {
            switch ($c) {
                case '1':
                case '2':
                    system("stty $term");

                    return $c;

                    break;
            }
        }
    }
}

