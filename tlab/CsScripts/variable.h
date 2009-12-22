#ifndef _CS_VARIABLE_
#define _CS_VARIABLE_

#include <string>
#include "interactive.h"

class Variable: public Interactive {
public:
	enum E_VTYPE {
		E_NULL,
		E_INT,
		E_FLOAT,
		E_STRING,
	};

	Variable();
	Variable( int value );
	Variable( float value );
	Variable( const std::string& value );
	Variable( const Variable& var );
	virtual ~Variable();

	E_VTYPE 			GetType() const { return m_eType; }
	virtual void* 		GetNullValue() const;
	virtual float 		GetFloatValue() const;
	virtual int 		GetIntValue() const;
	virtual std::string GetStringValue() const;

	const Variable& 	operator=( const Variable& var );
	bool 				operator==( const Variable& var ) const;
	bool 				operator!=( const Variable& var ) const;

	bool 				operator<( const Variable& var ) const;
	bool 				operator>( const Variable& var ) const;
	bool 				operator<=( const Variable& var ) const;
	bool 				operator>=( const Variable& var ) const;

	void 				operator+=( const Variable& var );
	void 				operator-=( const Variable& var );

	friend Variable 	operator+( const Variable& lhs, const Variable& rhs );
	friend Variable 	operator-( const Variable& lhs, const Variable& rhs );
	friend Variable 	operator*( const Variable& lhs, const Variable& rhs );
	friend Variable 	operator/( const Variable& lhs, const Variable& rhs );
protected:
	void 				SetType( E_VTYPE t ) { m_eType = t; }
	E_VTYPE 			m_eType;
	void* 				m_pData;
};

//Variable operator+( const Variable& lhs, const Variable& rhs );
//Variable operator-( const Variable& lhs, const Variable& rhs );

#endif //_CS_VARIABLE_
