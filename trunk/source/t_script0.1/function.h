#ifndef _CS_FUNCTION_
#define _CS_FUNCTION_

#include <vector>
#include "variable.h"
#include "interactive.h"

namespace CSSPT {
	
class Function: public Interactive {
public:
	virtual				~Function(){ m_vParamlist.clear(); };
	virtual void 	 	Compute() = 0;
	void 				AddParam( const Variable& var );
	void				ClearParam();
	const Variable 		GetReturnValue() const;
protected:
	Variable 			m_ReturnValue;
	std::vector< Variable > 	m_vParamlist;
};

};
#endif //_CS_FUNCTION_
