<?php 

class Config 
{
	/**
     * Get a value out of global config array from init.php
	 */
	public static function get($path = null) 
	{
		if ($path) 
		{
			$config = $GLOBALS['config'];
			$path = explode('/', $path);

			foreach ($path as $bit) 
			{
				if (isset($config[$bit])) 
				{
					$config = $config[$bit];
				}
			}

			return $config;
		}

		return false;
	}
}