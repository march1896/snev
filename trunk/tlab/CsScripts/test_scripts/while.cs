{
	$var = 1
	while ( $var < 50 ) {
		
		$var2 = 1
		while ( $var2 < 300 ) {
			&print($var, $var2 )
			$var2 = $var2 + 1
		}
		$var = $var + 1
	}
}
