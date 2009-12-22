#ifndef _CS_FUNCTION_
#define _CS_FUNCTION_

#include <vector>
#include "variable.h"
#include "interactive.h"

class Function: public Interactive {
public:
						~Function(){};
	void 	 			Compute(){};
	void 				AddParam( const Variable& var );
	const Variable 		GetReturnValue() const;
protected:
	Variable 			m_ReturnValue;
	std::vector< Variable > 	m_vParamlist;
};
#endif //_CS_FUNCTION_
