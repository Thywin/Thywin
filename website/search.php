<?php 
require_once 'core/init.php';

$results = 0;
$search = '';
$searchResults = array();

if (Input::exists()) {
	$search = Input::get('search');
	if (!empty($search)) {
		$host    = "80.113.19.16";
		$port    = 7501;
		
		$connection = new Connection($host, $port);
		$connection->write($search);

		$read = $connection->read();
		$searchResults =  explode(chr(3), $read);
	}
}

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
<body>

<div class="search">
	
	<div class="logo">
		<a href="index.php"><img src="img/logo.svg" alt="Thywin"></a>
	</div>
	
	<form action="search.php" method="get" class="searchform">
		<input type="text" name="search" id="search" placeholder="Start searching..." value="<?php echo Input::get('search'); ?>">
		<button>Search</button>
	</form>

</div><!-- end search -->

<div class="results">

	<?php if ($search == '') : ?>
	
		<h2>Oops, no results found!</h2>	

	<?php else : ?>

		<h2>Results on: "<?php echo $search; ?>" <?php echo floor(count($searchResults) / 2); ?> results found!</h2>

		<ul>

			<?php for($i = 1; $i < count($searchResults); $i += 2) :
				
				echo '<li><a href="' . $searchResults[$i] . '">' . $searchResults[$i] . '</a></li>';
			
			endfor ?>

		</ul> 

	<?php endif; ?>

</div>
	
</body>
</html>