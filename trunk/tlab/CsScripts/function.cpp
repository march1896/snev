#include "function.h"

using namespace CSSPT;
void Function::AddParam( const Variable& var ) {
	m_vParamlist.push_back( var );
}

void Function::ClearParam() {
	m_vParamlist.clear();
}

const Variable Function::GetReturnValue() const {
	return m_ReturnValue;
}
