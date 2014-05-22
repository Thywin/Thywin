<?php 
session_start();

error_reporting(E_ALL);
ini_set('display_errors', 1);

$GLOBALS['config'] = array(
	'psql' => array(
		'host' => '80.113.19.15',
		'username' => 'postgres',
		'password' => 'hanicampna',
		'dbname' => 'thywin'
	)
);

spl_autoload_register(function($class) {
	require_once 'classes/' . $class . '.php';
});

require_once 'functions/sanitize.php';