#include <iostream>

#include "util.h"
#include "exec2PL.h"

using namespace std;

int main (int argc, char *argv[]) {
	// File name : run 
	// N : number of threads
	// R : number of records
	// E : end commit id(end of global execution order)
	if (argc != 4) {
		cerr << "<using> ./run N(threads #) R(records #) E(end commit id)";
		cerr << endl;
		return -1;
	}

	// Convert *argv[] arguments to integer N, R, E.
	// If *argv[] has non-numeric string, terminate program.
	int N, R, E;
	bool trans_argv_to_int = argvToInteger(N, R, E, argv);
	if (trans_argv_to_int == false) {
		cerr << "<Argument Error> Arguments must be integer" << endl;
		return -1;
	}

	if (N < 1) {
		cerr << "<Argument Error> N(thread number) >= 1" << endl;
		return -1;
	}

	if (R < 3) {
		cerr << "<Argument Error> R(record number) >= 3" << endl;
		return -1;
	}

	if (E < 0) {
		cerr << "<Argument Error> E(end commit id) >= 0" << endl;
		return -1;
	}

	// Execute trasactions using 2PL and delete container
	Exec2PL *container2PL = new Exec2PL(N, R, E);
	container2PL->exec2PLTransactions();
	delete container2PL;

	return 0;
}
