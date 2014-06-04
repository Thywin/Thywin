<?php 
session_start();

$GLOBALS['config'] = array(
	'psql' => array(
		'host' => 'localhost',
		'username' => 'root',
		'password' => '',
		'dbname' => 'database'
	)
);

spl_autoload_register(function($class) {
	require_once 'classes/' . $class . '.php';
});

require_once 'functions/sanitize.php';