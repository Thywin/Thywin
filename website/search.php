<?php 
require_once 'core/init.php';

$results = 0;
$search = '';

if (isset($_GET['search'])) {
	$search = $_GET['search'];
}

if ($search != '') {
	$uris = DB::getInstance()->query("SELECT * FROM uris WHERE uri = ?", array( $search ));
	$results = $uris->results();
	$uris_count = $uris->count();
}

?>
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Thywin</title>

	<link href='http://fonts.googleapis.com/css?family=Roboto:400,300' rel='stylesheet' type='text/css'>
	<link rel="stylesheet" href="css/style.css">
</head>
<body>

<div class="search">
	
	<div class="logo">
		<a href="index.php"><img src="img/logo.svg" alt="Thywin"></a>
	</div>
	
	<form action="search.php" method="get" class="searchform">
		<input type="text" name="search" id="search" placeholder="Start searching..." value="<?php echo $search; ?>">
		<button>Search</button>
	</form>

</div><!-- end search -->

<div class="results">
	
	<?php if ($search == '' || $uris_count == 0) : ?>
	
		<h2>Oeps, no results found!</h2>

	<?php else : ?>

		<ul>
			
			<?php foreach ($results as $uri) : ?>
				
				<li><a href="http://<?php echo $uri->uri; ?>"><?php echo $uri->uri; ?> | Relevance: <?php echo $uri->relevance; ?></a></li>

			<?php endforeach; ?>

		</ul>

	<?php endif; ?>

</div>
	
</body>
</html>