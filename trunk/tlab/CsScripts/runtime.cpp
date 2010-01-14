#include "runtime.h"
#include "variable.h"
#include "function.h"
#include <map>
#include <vector>
#include <cstdio>

using namespace CSSPT;
#include "function.items"
/******************************************************************************
  implementation for functiontable class 
  ****************************************************************************/
FunctionTable::FunctionTable() {
}

FunctionTable::~FunctionTable() {
	std::map< std::string, Function* >::iterator itr = m_mTable.begin();
	for (; itr != m_mTable.end(); itr ++ ) {
		delete itr->second;
	}
	m_mTable.clear();
}

void FunctionTable::RegisterFunction( const std::string& funcName, Function *func ) {
	m_mTable.insert( make_pair( funcName, func ) );
}

void FunctionTable::UnregisterFunction( const std::string& funcName ) {
	std::map< std::string, Function* >::iterator itr;

	itr = m_mTable.find( funcName );
	if ( itr != m_mTable.end() ) {
		// find the function
		delete itr->second;
		m_mTable.erase( itr );
	}

	return;
}

Function* FunctionTable::GetFunction( const std::string& funcName ) {
	std::map< std::string, Function* >::iterator itr;

	itr = m_mTable.find( funcName );
	if ( itr != m_mTable.end() ) {
		return itr->second;
	}

	return NULL;
}

/******************************************************************************
  implementation for variable table
  ****************************************************************************/

VariableTable::VariableTable() {
	m_mTable.clear();
}

VariableTable::~VariableTable() {
	std::map< std::string, Variable* >::iterator itr;
	for ( itr = m_mTable.begin(); itr != m_mTable.end(); itr ++ ) {
		if ( itr->second ) {
			delete itr->second;
			itr->second = NULL;
		}
	}
	m_mTable.clear();
}

void VariableTable::RegisterVariable( const std::string& variableName, const Variable& var ) {
	m_mTable.insert( std::make_pair( variableName, new Variable( var ) ) );
}

void VariableTable::UnregisterVariable( const std::string& variableName ) {
	std::map< std::string, Variable* >::iterator itr;
	itr = m_mTable.find( variableName );

	if ( itr != m_mTable.end() ) {
		delete itr->second;
		m_mTable.erase( itr );
	}

	return;
}

Variable* VariableTable::GetVariable( const std::string& variableName ) {
	std::map< std::string, Variable* >::iterator itr;

	itr = m_mTable.find( variableName );

	if ( itr != m_mTable.end() ) {
		return itr->second;
	}

	return NULL;
}

/******************************************************************************
  implementation for runtime
  ****************************************************************************/
Runtime::Runtime() :
	m_pFuncTable( NULL ),
	m_bInited( false )
{
	m_vVtList.clear();
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
	if ( m_bInited ) {
		return;
	}
	
	m_bInited = true;
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

	for ( vt_itr-- ; vt_itr != m_vVtList.begin(); vt_itr -- ) {
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
	m_pFuncTable = new FunctionTable();

	m_pFuncTable->RegisterFunction( "print", new CsPrint() );
	m_pFuncTable->RegisterFunction( "add", new Add() );
	return;
}
