<?php 

/**
 * Escape a string and convert double and single quotes.
 */
function escape($string) 
{
	return htmlentities($string, ENT_QUOTES, 'UTF-8');
}