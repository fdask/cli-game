<?php
class Mailbox {
	private $messages;
	private $currentMsg;

	function __construct($welcomeMsg = null) {
		$this->currentMsg = 0;
		$this->messages = array();

		if ($welcomeMsg) {
			$this->addMessage($welcomeMsg);
		}
	}

	function clearMessages() {
		$this->messages = array();
	}

	/**
	 * add a message to the mailbox
	 */
	function addMessage($msg) {
		$this->messages[] = $msg;

		return true;
	}

	function addMessageToStart($msg) {
		array_unshift($this->messages, $msg);
	}

	/**
	 * returns the current message
	 * 
	 * then advances the currentMsg pointer
	 */
	function readCurrentMessage() {
		if (isset($this->messages[$this->currentMsg])) {
			return $this->messages[$this->currentMsg];
		} else {
			return "";
		}
	}

	/**
	 * advance the message pointer
	 */
	function nextMessage() {
		if ($this->currentMsg < count($this->messages) - 1) {
			$this->currentMsg++;

			return true;
		}

		return false;
	}

	/**
	 * decrement the message pointer
	 */
	function prevMessage() {
		if ($this->currentMsg > 0) {
			$this->currentMsg--;

			return true;
		}

		return false;
	}

	/**
	 * remove whatever message we are on
	 */
	function deleteCurrentMessage() {
		if (count($this->messages)) {
			array_splice($this->messages, $this->currentMsg, 1);

			if ($this->currentMsg > count($this->messages)) {
				$this->currentMsg = count($this->messages) - 1;
			}
		}

		return true;
	}

	/**
	 * dumps a bunch of instructions into the message queue
	 */
	public function addInstructions() {
    	$this->addMessage("wasd to move (press m)");
		$this->addMessage("'m' to move to the next message");
		$this->addMessage("the rain is falling down");
		$this->addMessage("and minerals are washed around");
		$this->addMessage("can't go through a rock");
		$this->addMessage("try and not be found");
		$this->addMessage("");
		$this->addMessage("use 1 2 or 3 keys to change view");
		$this->addMessage("h2o, mineral, or plant view");
		$this->addMessage("press 't' to toggle all three");
		$this->addMessage("'h' to show or hide you");
		$this->addMessage("on the currently selected view");
		$this->addMessage("");
		$this->addMessage("'q' to quit, 'n' for new game");
		$this->addMessage("'z' and 'c' for scroll viewport");
		$this->addMessage("collect minerals to maintain health");
	}

    public function showKeyboardMsg() {
        $this->addMessageToStart(array(
            " ",
            "123 to change view",
            "1: h2o (washes minerals down)",
            "2: mineral (collect for health)",
            "3: plant (turns to minerals)",
            " ",
            "wasd to move",
			" ",
			"h - hide character",
			"m - next message",
            "q - quit game",
			"n - newgame",
            " "
        ));
    }
}