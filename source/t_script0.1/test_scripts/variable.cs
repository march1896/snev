{
	$str = "Hello_"
	$str = $str + "Tangel"
	$str = "Fuck_" + $str
	&print( $str )

	$str = -3 * ( 8 * ( 10 - 2 * 4 ) )
	&print( $str )

	$str = $var = 0.5 / ( 5 + 10 / 10 - 1 )
	&print( $var, $str )

	$str = "Hello"

	if ( $str == "Hello" ) {
		&print( "same" )
	}
	else {
		&print( "not the same" )
	}

	$str2 = "Tangel"

	if ( $str != "Hello" ) {
		&print( "str and str2 are the same" )
	}
	else {
		&print( "str and str2 are not the same" )
	}
}



