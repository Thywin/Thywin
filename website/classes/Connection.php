<?php 

class Connection {

	protected $socket;

	private $buffer = 4096;

	/**
	 * Create a socket and start the connection with the server based on the HOST and the PORT of the server.
	 * Return the currect Connection object.
	 */
	public function __construct($host, $port) {
		$this->socket = socket_create(AF_INET, SOCK_STREAM, 0) or die("Could not create socket\n");
		
		socket_connect($this->socket, $host, $port) or die("Could not connect to server\n");  

		return $this;
	}

	/** 
	 * Write a message to the server.
	 */
	public function write($input) {
		$message = $input . chr(4);

		socket_write($this->socket, $message, strlen($message)) or die("Could not send data to server\n");
	}

	/** 
	 * Read a message from the server until the '\0'.
	 */
	public function read() {
		return socket_read($this->socket, $this->buffer, PHP_BINARY_READ);
	}	

    /** 
     * Close the connection with the server.
     */ 
	public function closeConnection() {
		return socket_close($this->socket);
	}
	
}