#include "runtime.h"
#include "variable.h"
#include "function.h"
#include <map>
#include <vector>

/******************************************************************************
  implementation for functiontable class 
  ****************************************************************************/
FunctionTable::FunctionTable() {
}

FunctionTable::~FunctionTable() {
	m_mTable.clear();
}

void FunctionTable::RegisterFunction( const std::string& funcName, Function func ) {
	m_mTable.insert( make_pair( funcName, func ) );
}

void FunctionTable::UnregisterFunction( const std::string& funcName ) {
	std::map< std::string, Function >::iterator itr;

	itr = m_mTable.find( funcName );
	if ( itr != m_mTable.end() ) {
		// find the function
		m_mTable.erase( itr );
	}

	return;
}

Function* FunctionTable::GetFunction( const std::string& funcName ) {
	std::map< std::string, Function >::iterator itr;

	itr = m_mTable.find( funcName );
	if ( itr != m_mTable.end() ) {
		return &itr->second;
	}

	return NULL;
}

/******************************************************************************
  implementation for variable table
  ****************************************************************************/

VariableTable::VariableTable() {
}

VariableTable::~VariableTable() {
	m_mTable.clear();
}

void VariableTable::RegisterVariable( const std::string& variableName, const Variable& var ) {
	m_mTable.insert( make_pair( variableName, var ) );
}

void VariableTable::UnregisterVariable( const std::string& variableName ) {
	std::map< std::string, Variable >::iterator itr;
	itr = m_mTable.find( variableName );

	if ( itr != m_mTable.end() ) {
		m_mTable.erase( itr );
	}

	return;
}

Variable* VariableTable::GetVariable( const std::string& variableName ) {
	std::map< std::string, Variable >::iterator itr;
	itr = m_mTable.find( variableName );

	if ( itr != m_mTable.end() ) {
		return &itr->second;
	}

	return NULL;
}

/******************************************************************************
  implementation for runtime
  ****************************************************************************/
Runtime::Runtime()
{
	Initialize();
}

Runtime::~Runtime() {
	if ( m_pFuncTable ) {
		delete m_pFuncTable;
		m_pFuncTable = NULL;
	}

	std::vector< VariableTable* >::iterator itr = m_vVtList.begin();
	for ( ; itr != m_vVtList.end(); itr ++ ) {
		if ( *(itr) ) {
			delete *(itr);
			*(itr) = NULL;
		}
	}

	m_vVtList.clear();
}

void Runtime::Initialize() {
	InitFunctionTable();
	PushVarTable();
}

void Runtime::PushVarTable() {
	VariableTable* 	vt = new VariableTable();
	m_vVtList.push_back( vt );
}

void Runtime::PopVarTable() {
	VariableTable* vt = m_vVtList.back();
	delete vt;
	m_vVtList.pop_back();
}

Variable* Runtime::GetVariable( const std::string& varName ) {
	std::vector< VariableTable* >::iterator vt_itr = m_vVtList.end();

	for ( ; vt_itr != m_vVtList.begin(); vt_itr -- ) {
		// check for toppest variabletable first, then go down 
		Variable* pv = (*vt_itr)->GetVariable( varName );
		if ( pv != NULL ) {
			return pv;
		}
	}

	return NULL;
}

Variable* Runtime::AddVariable( const std::string& varName, Variable var ) {
	VariableTable* vt = m_vVtList.back();
	vt->RegisterVariable( varName, var );
	return GetVariable( varName );
}

void Runtime::InitFunctionTable() {
	// TODO:
	return;
}
