{
	$i = 1
	$j = 1

	&print( $i )
	&print( $j )
	while ( $i < 100 ) {
		$k = $i + $j
		$j = $i
		$i = $k
		&print( $i )
	}
}
