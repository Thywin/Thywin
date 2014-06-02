<?php 
session_start();

$GLOBALS['config'] = array(
	'psql' => array(
		'host' => 'localhost',
		'username' => 'user',
		'password' => 'password',
		'dbname' => 'database'
	),
	'session' => array(
		'token_name' => 'token'
	)
);

spl_autoload_register(function($class) {
	require_once 'classes/' . $class . '.php';
});

require_once 'functions/sanitize.php';