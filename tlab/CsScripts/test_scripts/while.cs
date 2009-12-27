{
	$var = 1
	while ( $var < 5 ) {
		
		$var2 = 1
		while ( $var2 < 10 ) {
			if ( $var2 == 4 ) {
				break
			}
			&print($var, $var2 )
			$var2 = $var2 + 1
		}
		if ( $var == 3 ) {
			break
		}
		else {
			&print( "k" )
		}
		$var = $var + 1
	}
}
