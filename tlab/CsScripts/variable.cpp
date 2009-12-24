#include "variable.h"

static int getInt( void *p ) {
	return *( static_cast<int*>( p ) );
}

static float getFloat( void *p ) {
	return *( static_cast<float*>( p ) );
}

static std::string getString( void *p ) {
	return *( static_cast< std::string* >( p ) );
}

Variable::Variable( int value ): m_eType( E_INT ) {
	m_pData = new int( value );
}

Variable::Variable( float value ): m_eType( E_FLOAT ) {
	m_pData = new float( value );
}

Variable::Variable( const std::string& value ): m_eType( E_STRING ) {
	m_pData = new std::string( value );
}

Variable::Variable(): m_eType( E_NULL ), m_pData( NULL ) {
}

Variable::Variable( const Variable& var ) {
	E_VTYPE type = var.GetType();
	SetType( type );

	if ( type == E_NULL ) {
		m_pData = NULL;
	}
	else if ( type == E_FLOAT ) {
		m_pData = new float( var.GetFloatValue() );
	}
	else if ( type == E_INT ) {
		m_pData = new int( var.GetIntValue() );
	}
	else if ( type == E_STRING ) {
		m_pData = new std::string( var.GetStringValue() );
	}
}

Variable::~Variable() {
	if ( m_pData != NULL ) {
		if ( m_eType == E_INT ) {
			int* p = static_cast< int* >( m_pData );
			delete p;
		}
		else if ( m_eType == E_FLOAT ) {
			float* p = static_cast< float* >( m_pData );
			delete p;
		}
		else if ( m_eType == E_STRING ) {
			std::string* p = static_cast< std::string* >( m_pData );
			delete p;
		}
		m_pData = NULL;
	}
}


bool Variable::Castable( E_VTYPE t ) const {
	if ( m_eType == t ) {
		// need no cast
		return true;
	}

	if ( m_eType == E_INT && t == E_FLOAT ) {
		return true;
	}

	return false;
}

bool Variable::Castto( E_VTYPE t ) {
	if ( !Castable( t ) ) {
		return false;
	}

	if ( t == m_eType ) {
		return true;
	}

	if ( m_eType == E_INT && t == E_FLOAT ) {
		int *p = static_cast< int* >( m_pData );
		float f = *p;
		delete p;
		m_pData = new float( f );
	}

	return false;
}

const Variable& Variable::operator=( const Variable& var ) {
	if ( m_pData ) {
		if ( m_eType == E_INT ) {
			int* p = static_cast< int* >( m_pData );
			delete p;
		}
		else if ( m_eType == E_FLOAT ) {
			float* p = static_cast< float* >( m_pData );
			delete p;
		}
		else if ( m_eType == E_STRING ) {
			std::string* p = static_cast< std::string* >( m_pData );
			delete p;
		}
		m_pData = NULL;
	}
	
	E_VTYPE type = var.GetType();
	SetType( type );
	if ( type == E_NULL ) {
		m_pData = NULL;
	}
	else if ( type == E_FLOAT ) {
		m_pData = new float( var.GetFloatValue() );
	}
	else if ( type == E_INT ) {
		m_pData = new int( var.GetIntValue() );
	}
	else if ( type == E_STRING ) {
		m_pData = new std::string( var.GetStringValue() );
	}

	return *this;
}

void Variable::operator+=( const Variable& var ) {
	if ( m_eType != var.GetType() ) {
		return;
	}

	if ( m_eType == E_NULL ) {
		m_pData = NULL;
	}
	else if ( m_eType == E_FLOAT ) {
		float *p = static_cast< float* >( m_pData );
		*p += var.GetFloatValue();
	}
	else if ( m_eType == E_INT ) {
		int* p = static_cast< int* >( m_pData );
		*p += var.GetIntValue();
	}
	else if ( m_eType == E_STRING ) {
		std::string *p = static_cast< std::string* >( m_pData );
		*p += var.GetStringValue();
	}

	return;
}

void Variable::operator-=( const Variable& var ) {
	if ( m_eType != var.GetType() ) {
		return;
	}

	if ( m_eType == E_NULL ) {
		m_pData = NULL;
	}
	else if ( m_eType == E_FLOAT ) {
		float *p = static_cast< float* >( m_pData );
		*p -= var.GetFloatValue();
	}
	else if ( m_eType == E_INT ) {
		int* p = static_cast< int* >( m_pData );
		*p -= var.GetIntValue();
	}
	else if ( m_eType == E_STRING ) {
		/*
			no operator -= defined for string type
		std::string *p = static_cast< std::string* >( m_pData );
		(*p) -= var.GetStringValue();
		*/
	}

	return;
}

bool Variable::operator==( const Variable& var ) const {
	if ( m_eType - var.GetType() == 1 ) {
		return GetFloatValue() == var.GetFloatValue();
	}
	else if ( m_eType - var.GetType() == -1 ) {
		return GetFloatValue() == var.GetFloatValue();
	}

	if ( m_eType != var.GetType() ) {
		return false;
	}

	switch ( m_eType ) {
		case E_NULL:
			return true;
			break;
		case E_INT:
			return GetIntValue() == var.GetIntValue();
			break;
		case E_FLOAT:
			return GetFloatValue() == var.GetFloatValue();
			break;
		case E_STRING:
			return GetStringValue() == var.GetStringValue();
			break;
	}
	return false;
}

bool Variable::operator!=( const Variable& var ) const {
	return !( *this == var );
}

bool Variable::operator<( const Variable& var ) const {
	if ( ( m_eType - var.GetType() == 1 ) &&  ( m_eType - var.GetType() == -1 ) ) {
		return GetFloatValue() < var.GetFloatValue();
	}

	if ( m_eType != var.GetType() ) {
		// Error
		return false;
	}

	switch ( m_eType ) {
		case E_NULL:
			return true;
			break;
		case E_INT:
			return GetIntValue() < var.GetIntValue();
			break;
		case E_FLOAT:
			return GetFloatValue() < var.GetFloatValue();
			break;
		case E_STRING:
			return GetStringValue() < var.GetStringValue();
			break;
	}
	return false;
}

bool Variable::operator>( const Variable& var ) const {
	if ( ( m_eType - var.GetType() == 1 ) &&  ( m_eType - var.GetType() == -1 ) ) {
		return GetFloatValue() > var.GetFloatValue();
	}

	if ( m_eType != var.GetType() ) {
		// Error
		return false;
	}

	switch ( m_eType ) {
		case E_NULL:
			return true;
			break;
		case E_INT:
			return GetIntValue() > var.GetIntValue();
			break;
		case E_FLOAT:
			return GetFloatValue() > var.GetFloatValue();
			break;
		case E_STRING:
			return GetStringValue() > var.GetStringValue();
			break;
	}
	return false;
}

bool Variable::operator<=( const Variable& var ) const {
	return !( *this > var );
}

bool Variable::operator>=( const Variable& var ) const {
	return !( *this < var );
}

void* Variable::GetNullValue() const {
	return NULL;
}

float Variable::GetFloatValue() const {
	if ( m_eType == E_FLOAT ) {
		return getFloat( m_pData );
	}
	else if ( m_eType == E_INT ) {
		return (float)( getInt( m_pData ) );
	}
	else {
		return 0.0;
	}
}

int Variable::GetIntValue() const {
	if ( m_eType == E_INT ) {
		return getInt( m_pData );
	}
	else if ( m_eType == E_FLOAT ) {
		return (int)( getInt( m_pData ) );
	}
	else {
		return 0;
	}
}

std::string Variable::GetStringValue() const {
	if ( m_eType == E_STRING ) {
		return getString( m_pData );
	}
	else {
		return std::string( "" );
	}
}

Variable operator+( const Variable& lhs, const Variable& rhs ) {
	Variable ret;

	if ( lhs.GetType() - rhs.GetType() == 1 || lhs.GetType() - rhs.GetType() == -1 ) {
		ret.SetType( Variable::E_FLOAT );
		ret.m_pData = new float( lhs.GetFloatValue() + rhs.GetFloatValue() );
	}
	else if ( lhs.GetType() == rhs.GetType() ) {
		ret.SetType( lhs.GetType() );
		switch ( lhs.GetType() ) {
			case Variable::E_NULL:
				ret.m_pData = NULL;
				break;
			case Variable::E_INT:
				ret.m_pData = new int( getInt( lhs.m_pData ) + getInt( rhs.m_pData ) );
				break;
			case Variable::E_FLOAT:
				ret.m_pData = new float( getFloat( lhs.m_pData) + getFloat( rhs.m_pData ) );
				break;
			case Variable::E_STRING:
				ret.m_pData = new std::string( getString( lhs.m_pData) + getString( rhs.m_pData ) );
				break;
		}
		return ret;
	}

	return ret;
	// Error Here;
}

Variable operator-( const Variable& lhs, const Variable& rhs ) {
	Variable ret;
	if ( lhs.GetType() - rhs.GetType() == 1 || lhs.GetType() - rhs.GetType() == -1 ) {
		ret.SetType( Variable::E_FLOAT );
		ret.m_pData = new float( lhs.GetFloatValue() - rhs.GetFloatValue() );
	}
	else if ( lhs.GetType() == rhs.GetType() ) {
		ret.SetType( lhs.GetType() );
		switch ( lhs.GetType() ) {
			case Variable::E_NULL:
				ret.m_pData = NULL;
				break;
			case Variable::E_INT:
				ret.m_pData = new int( getInt( lhs.m_pData ) - getInt( rhs.m_pData ) );
				break;
			case Variable::E_FLOAT:
				ret.m_pData = new float( getFloat( lhs.m_pData) - getFloat( rhs.m_pData ) );
				break;
			case Variable::E_STRING:
				// no opeator - defined for string type
				// ret.m_pData = new std::string( getString( lhs.m_pData) - getString( rhs.m_pData ) );
				break;
		}
		return ret;
	}

	return ret;
	// Error
}

Variable operator*( const Variable& lhs, const Variable& rhs ) {
	Variable ret;

	if ( lhs.GetType() - rhs.GetType() == 1 || lhs.GetType() - rhs.GetType() == -1 ) {
		ret.SetType( Variable::E_FLOAT );
		ret.m_pData = new float( lhs.GetFloatValue() * rhs.GetFloatValue() );
	}
	else if ( lhs.GetType() == rhs.GetType() ) {
		ret.SetType( lhs.GetType() );
		switch ( lhs.GetType() ) {
			case Variable::E_NULL:
				ret.m_pData = NULL;
				break;
			case Variable::E_INT:
				ret.m_pData = new int( getInt( lhs.m_pData ) * getInt( rhs.m_pData ) );
				break;
			case Variable::E_FLOAT:
				ret.m_pData = new float( getFloat( lhs.m_pData) * getFloat( rhs.m_pData ) );
				break;
			case Variable::E_STRING:
				break;
		}
		return ret;
	}

	// Error Here;
	return ret;
}

Variable operator/( const Variable& lhs, const Variable& rhs ) {
	Variable ret;

	if ( lhs.GetType() - rhs.GetType() == 1 || lhs.GetType() - rhs.GetType() == -1 ) {
		ret.SetType( Variable::E_FLOAT );
		ret.m_pData = new float( lhs.GetFloatValue() / rhs.GetFloatValue() );
	}
	else if ( lhs.GetType() == rhs.GetType() ) {
		ret.SetType( lhs.GetType() );
		switch ( lhs.GetType() ) {
			case Variable::E_NULL:
				ret.m_pData = NULL;
				break;
			case Variable::E_INT:
				ret.m_pData = new int( getInt( lhs.m_pData ) / getInt( rhs.m_pData ) );
				break;
			case Variable::E_FLOAT:
				ret.m_pData = new float( getFloat( lhs.m_pData) / getFloat( rhs.m_pData ) );
				break;
			case Variable::E_STRING:
				// no opeator - defined for string type
				// ret.m_pData = new std::string( getString( lhs.m_pData) - getString( rhs.m_pData ) );
				break;
		}
		return ret;
	}

	// Error
	return ret;
}

//***********************************************************************************************
#ifdef _CS_VARIABLE_TEST_
#include <cstdio>
int main() {
	Variable i1 = Variable( "Fuck you, " );
	Variable i2 = Variable( "Tangel" );

	Variable i3 = i1 + i2;

	printf( "%d %s\n", i3.GetType(), i3.GetStringValue().c_str() );

	getchar();
	return 0;
}
#endif //_CS_VARIABLE_TEST_
//  *********************************************************************************************/
