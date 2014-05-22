<?php
require_once 'core/init.php';

$uris = DB::getInstance()->get('uris', array('uri_id', '>', 0));
$uri_count = $uris->count();

?>

<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Thywin</title>

	<link href='http://fonts.googleapis.com/css?family=Roboto:400,300' rel='stylesheet' type='text/css'>
	<link rel="stylesheet" href="css/style.css">
</head>
<body class="body-home">

<div class="search-home">
	
	<div class="logo">
		<img src="img/logo.svg" alt="Thywin">
	</div>
	
	<form action="search.php" method="get" class="searchform">
		<input type="text" name="search" id="seaarch" placeholder="Start searching...">
		<button>Search</button>
	</form>

	<p><?php echo $uri_count; ?> unique links</p>

</div><!-- end search -->

</body>
</html>