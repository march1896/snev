{
	$var1 = 5
	$var2 = 5

	if ( $var1 == 5 ) {
		if ( $var2 == 5 ) {
			&print( "$var1 == 10 && $var2 == 15" )
			
			$i = 0
			while ( $i < $var1 ) {
				$j = 0
				while ( $j < $var2 ) {
					if ( $i == 3 && ($j == 3) ) {
						&print( "hello", "tangel" )
					}
					else 
					{
						&print( $i, $j)
					}
					$j = $j + 1
				}
				$i = $i + 1
			}
		}
	}
	else {
	}
}
