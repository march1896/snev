#include "function.h"

void Function::AddParam( const Variable& var ) {
	m_vParamlist.push_back( var );
}

const Variable Function::GetReturnValue() const {
	return m_ReturnValue;
}
