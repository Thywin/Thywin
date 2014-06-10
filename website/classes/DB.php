<?php 

class DB
{
	private static $_instance = null;
	private $_pdo;
	private $_query;
	private $_error = false;
	private $_results;
	private $_count = 0;

	/**
     * Create a DB connection
	 */
	private function __construct() 
	{
		try
		{
			$this->_pdo = new PDO('pgsql:dbname=' . Config::get('psql/dbname') . ';host=' . Config::get('psql/host'), Config::get('psql/username'), Config::get('psql/password'));
		}
		catch(PDOException $e)
		{
			die($e->getMessage());
		}
	}	

	/**
     * Get a instance of the Database with an active connection
	 */
	public static function getInstance() 
	{
		if (!isset(self::$_instance)) 
		{
			self::$_instance = new DB();
		}

		return self::$_instance;
	}

	/**
     * Create a query based on a SQL statement and matching values for the question marks
	 */
	public function query($sql, $params = array()) 
	{
		$this->_error = false;

		if ($this->_query = $this->_pdo->prepare($sql))
		{
			$x = 1;

			if (count($params))
			{
				foreach ($params as $param) 
				{
					$this->_query->bindValue($x, $param);	
					$x++;
				}

				if ($this->_query->execute())
				{
					var_dump($this->_query);

					$this->_results = $this->_query->fetchAll(PDO::FETCH_OBJ);
					$this->_count = $this->_query->rowCount();
				}
				else
				{
					$this->_error = true;
				}
			}
		}

		return $this;
	}

	/**
     * Perform a query on the database based on an action, a table and a where condition
	 */
	public function action($action, $table, $where = array())
	{
		if (count($where) === 3)
		{
			$operators = array('=', '>', '<', '>=', '<=');

			$field 		= $where[0];
			$operator 	= $where[1];
			$value 		= $where[2];

			if (in_array($operator, $operators))
			{
				$sql = "{$action} FROM {$table} WHERE {$field} {$operator} ?";

				if (!$this->query($sql, array($value))->error())
				{
					return $this;
				}
			}
		}

		return false;
	}

	/**
     * Get data from a certain table with a where condition
	 */
	public function get($table, $where)
	{
		return $this->action('SELECT *', $table, $where);
	}

	/**
     * Return errors;
	 */
	public function error()
	{
		return $this->_error;
	}

	/**
     * Return the row count;
	 */
	public function count() {
		return $this->_count;
	}

	/**
     * Return the query results;
	 */
	public function results() {
		return $this->_results;
	}
} 