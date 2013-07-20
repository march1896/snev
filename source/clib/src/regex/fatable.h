#ifndef _FA_TABLE_H_
#define _FA_TABLE_H_

struct s_dfa;
void dfa_to_table( const p_dfa pa, int** weight_list, int *weight_size, int** state_table, int *state_size );
void table_print( const p_dfa pa );

#endif //_FA_TABLE_H_
