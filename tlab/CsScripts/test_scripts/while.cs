{
	$var = 1
	while ( $var < 5 ) {
		
		$var2 = 1
		while ( $var2 < 10 ) {
			&print($var, $var2 )
			$var2 = $var2 + 1
		}
		if ( $var == 3 ) {
			&print( "breaked" )
			break
		}
		else {
			&print( "fuck" )
		}
		$var = $var + 1
	}
}
