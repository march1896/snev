#ifndef _CS_RUNTIME_
#define _CS_RUNTIME_
#include "function.h"
#include "variable.h"
#include <map>

namespace CSSPT {

class FunctionTable {
public:
						~FunctionTable();
	void 				RegisterFunction( const std::string& funcName, Function* func );
	void 				UnregisterFunction( const std::string& funcName );

	Function* 			GetFunction( const std::string& funcName );

public:
	friend class 		Runtime;

private:
						FunctionTable();
	std::map< std::string, Function* > m_mTable;
};

class VariableTable {
public:
						~VariableTable();
	void 				RegisterVariable( const std::string& variableName, const Variable& var );
	void 				UnregisterVariable( const std::string& variableName );

	Variable* 			GetVariable( const std::string& variableName );

public:
	friend class 		Runtime;

private:
						VariableTable();
	std::map< std::string, Variable* > m_mTable;
};


class Runtime {
public:
						Runtime();
						~Runtime();

	void 				Initialize();

	// init all functions use in the runtime environment
	void				InitFunctionTable();
	FunctionTable* 		GetFunctionTable() { return m_pFuncTable; }
	Function* 			GetFunction( const std::string& funcName ) { return m_pFuncTable->GetFunction( funcName ); }

	void 				PushVarTable();
	void 				PopVarTable();
	Variable* 			GetVariable( const std::string& varName );
	Variable* 			AddVariable( const std::string& varName, Variable var );

private:
	FunctionTable* 		m_pFuncTable;
	std::vector< VariableTable* > m_vVtList;
	bool 				m_bInited;
};
	
};
#endif // _CS_RUNTIME_
