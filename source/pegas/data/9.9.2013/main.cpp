#include <iostream>
#include <set>
#include <ctime>
#include <cassert>
using namespace std;

static int __num_modify; /* number of insert and remove */
static int __num_search;

static int __data_diff_type; /* number of different values to store in the set */
static int __data_max_count; /* max count of a single value */

static intptr_t* data_count = NULL;

static void __reset_data() {
	int i = 0;

	for (i = 0; i < __data_diff_type; i ++) {
		data_count[i] = 0;
	}
}

static void __create_data() {
	if (data_count != NULL) 
		free(data_count);

	data_count = (intptr_t*)malloc(sizeof(intptr_t)*__data_diff_type);

	__reset_data();
}

static void __destroy_data() {
	free(data_count);
	data_count = NULL;
}

std::set<int> s;
std::multiset<int> ms;

static void set_bench_modify_randomly() {
	int i;

	printf("[number of modify: %d]\n", __num_modify);
	assert(s.empty());

	for (i = 0; i < __num_modify; i ++) {
		int x = rand() % __data_diff_type;
		bool found = s.find(x) != s.end();
		
		if (found == false) {
			s.insert(x);
			data_count[x] ++;
		} else {
			s.erase(x);
			data_count[x] --;
		}
	}

	s.clear();
}

static void set_bench_search_randomly() {
	int i;
	assert(s.empty());

	printf("[number of search: %d]\n", __num_search);

	for (i = 0; i < __data_diff_type; i ++) {
		/* insert all data one by one */
		s.insert(i);
	}
	/* now the set contains __data_diff_type * 2 of data, each elememt int [0, __data_diff_type)
	 * appears at least once */

	for (i = 0; i < __num_search; i ++) {
		int x = rand() % __data_diff_type;

		bool res = s.find(x) != s.end();
		assert(res == true);
	}

	s.clear();
}

void set_test_bench() {
	__data_diff_type = 100;
	__data_max_count = 1;
	__num_modify     = 100;
	__num_search     = 100;
	s.clear();
	printf("[data type: %d, single data max dup: %d]\n", __data_diff_type, __data_max_count);
	
	__create_data();
	set_bench_modify_randomly();
	__destroy_data();
	set_bench_search_randomly();

	__num_modify     = 100000;
	__num_search     = 100;
	__create_data();
	set_bench_modify_randomly();
	__destroy_data();

	__num_modify     = 100;
	__num_search     = 100000;
	set_bench_search_randomly();

	__data_diff_type = 5000;
	printf("[data type: %d, single data max dup: %d]\n", __data_diff_type, __data_max_count);
	__num_modify     = 100000;
	__num_search     = 100;
	__create_data();
	set_bench_modify_randomly();
	__destroy_data();

	__num_modify     = 100;
	__num_search     = 100000;
	set_bench_search_randomly();

	__data_diff_type = 100000;
	printf("[data type: %d, single data max dup: %d]\n", __data_diff_type, __data_max_count);
	__num_modify     = 100000;
	__num_search     = 100;

	__create_data();
	set_bench_modify_randomly();
	__destroy_data();

	__num_modify     = 100;
	__num_search     = 100000;
	set_bench_search_randomly();

	__data_diff_type = 100000;
	printf("[data type: %d, single data max dup: %d]\n", __data_diff_type, __data_max_count);
	__num_modify     = 1000000;
	__num_search     = 100;

	__create_data();
	set_bench_modify_randomly();
	__destroy_data();

	__num_modify     = 100;
	__num_search     = 1000000;
	set_bench_search_randomly();
}

static void mset_bench_modify_randomly() {
	int i;

	printf("[number of modify: %d]\n", __num_modify);
	assert(ms.empty());

	for (i = 0; i < __num_modify; i ++) {
		int x = rand() % __data_diff_type;
		int count = ms.count(x);
		
		if (count == 0) {
			ms.insert(x);
			data_count[x] ++;
		} else if (count == __data_max_count) {
			ms.erase(x);
			data_count[x] --;
		} else {
			int add = rand() % 2;
			if (add == 1) {
				ms.insert(x);
				data_count[x] ++;
			}
			else {
				ms.erase(x);
				data_count[x] --;
			}
		}
	}

	ms.clear();
}

static void mset_bench_search_randomly() {
	int i;
	assert(ms.empty());

	printf("[number of search: %d]\n", __num_search);

	for (i = 0; i < __data_diff_type; i ++) {
		/* insert all data one by one */
		ms.insert(i);
		int x = rand() % __data_diff_type;
		ms.insert(x);
	}
	/* now the mset contains __data_diff_type * 2 of data, each elememt int [0, __data_diff_type)
	 * appears at least once */

	for (i = 0; i < __num_search; i ++) {
		int x = rand() % __data_diff_type;

		bool res = ms.find(x) != ms.end();
		assert(res == true);
	}

	ms.clear();
}

void mset_test_bench() {
	__data_diff_type = 100;
	__data_max_count = 1;
	__num_modify     = 100;
	__num_search     = 100;
	ms.clear();
	printf("[data type: %d, single data max dup: %d]\n", __data_diff_type, __data_max_count);
	
	__create_data();
	mset_bench_modify_randomly();
	__destroy_data();
	mset_bench_search_randomly();

	__num_modify     = 100000;
	__num_search     = 100;
	__create_data();
	mset_bench_modify_randomly();
	__destroy_data();

	__num_modify     = 100;
	__num_search     = 100000;
	mset_bench_search_randomly();

	__data_diff_type = 5000;
	printf("[data type: %d, single data max dup: %d]\n", __data_diff_type, __data_max_count);
	__num_modify     = 100000;
	__num_search     = 100;
	__create_data();
	mset_bench_modify_randomly();
	__destroy_data();

	__num_modify     = 100;
	__num_search     = 100000;
	mset_bench_search_randomly();

	__data_diff_type = 100000;
	printf("[data type: %d, single data max dup: %d]\n", __data_diff_type, __data_max_count);
	__num_modify     = 100000;
	__num_search     = 100;

	__create_data();
	mset_bench_modify_randomly();
	__destroy_data();

	__num_modify     = 100;
	__num_search     = 100000;
	mset_bench_search_randomly();

	__data_diff_type = 100000;
	printf("[data type: %d, single data max dup: %d]\n", __data_diff_type, __data_max_count);
	__num_modify     = 1000000;
	__num_search     = 100;

	__create_data();
	mset_bench_modify_randomly();
	__destroy_data();

	__num_modify     = 100;
	__num_search     = 1000000;
	mset_bench_search_randomly();
}

int main() {
	clock_t start_c, end_c;
	start_c = clock();

	set_test_bench();

	end_c = clock();
	cout << (float)(end_c - start_c)/CLOCKS_PER_SEC << endl;

	start_c = clock();

	mset_test_bench();

	end_c = clock();
	cout << (float)(end_c - start_c)/CLOCKS_PER_SEC << endl;
}
