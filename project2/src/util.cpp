#include <cctype>
#include <cstdlib>
#include <random>
#include <set>
#include <vector>

#include "util.h"

using namespace std;

// Set mersenne twister engine for random number
random_device seed;
mt19937 engine(seed());
uniform_int_distribution<> *rand_int;

int record_num;

// Convert three arguments to integer each.
bool argvToInteger(int &N, int &R, int &E, char **argv) {
	// Find if char* argv has non-numeric element.
	// If has it, return false.
	for (int i = 1; i < 4; i++) {
		int j = 0;
		while (argv[i][j] != '\0') {
			if (!isdigit(argv[i][j]))
				return false;

			j++;
		}
	}

	// Convert char* to integer
	N = atoi(argv[1]);
	R = atoi(argv[2]);
	E = atoi(argv[3]);

	return true;
}

// Set random number range
void randInit(int R) {
	rand_int = new uniform_int_distribution<> (0, R-1);
	record_num = R;
}

// Return random number
int randNum() {
	return (*rand_int)(engine);
}

// deallocate random engine
void randDelete() {
	delete rand_int;
}

// get different i, j, k randomly
void get_ijk(int &i, int &j, int &k) {
	i = randNum();
	
	do {
		j = randNum();
	} while (j == i);

	do {
		k = randNum();
	} while (i == k || j == k);
}

// union id vectors before trx_id to set
void unionVectorsToSet(int trx_id, set<int> &wait_set, 
						Records *record, vector<TrxInfo> &trx_info) {

	// if trx_info[trx_id].i, j or k == INFO_NULL(-1), 
	// trx wasn't progress that record yet
	if (trx_info[trx_id].i != INFO_NULL) {
		for (auto &val_i : record[trx_info[trx_id].i].lock_list_trx_id) {
			if (val_i == trx_id)
				break;
	
			wait_set.insert(val_i);
		}
	}

	if (trx_info[trx_id].j != INFO_NULL) {
		for (auto &val_j : record[trx_info[trx_id].j].lock_list_trx_id) {
			if (val_j == trx_id)
				break;
	
			wait_set.insert(val_j);
		}
	}

	if (trx_info[trx_id].k != INFO_NULL) {
		for (auto &val_k : record[trx_info[trx_id].k].lock_list_trx_id) {
			if (val_k == trx_id)
				break;

			wait_set.insert(val_k);
		}
	}
}
