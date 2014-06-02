<?php
require_once 'core/init.php';

?>

<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">

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
		<input type="text" name="search" id="search" placeholder="Start searching...">
		<input type="hidden" name="token" value="<?php echo Token::generate(); ?>">
		<button>Search</button>
	</form>
	
</div><!-- end search -->

</body>
</html>