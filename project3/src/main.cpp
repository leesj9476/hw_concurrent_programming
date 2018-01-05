#include <iostream>
#include <thread>
#include <cctype>

#include "waitfreesnapshot.h"

using namespace std;

int main (int argc, char *argv[]) {
	// program execution must be "./run <thread number>"
	if (argc != 2) {
		cerr << "<using> ./run N(threads #)" << endl;
		return -1;
	}

	// check if argument is number
	int i = 0;
	while (argv[1][i] != '\0') {
		if (!isdigit(argv[1][0])){
			cerr << "<Argument Error> Arguments must be interger" << endl;
			return -1;
		}

		i++;
	}

	// translate argument to integer and check if 1 <= <thread number> <= 32
	int N = atoi(argv[1]);
	if (N < 1 || N > 32) {
		cerr << "<Argument Error> 1 <= N(thread number) <= 32" << endl;
		return -1;
	}

	// execute wait free snapshot update
	long long updateNum = 0;
	ExecWaitFreeSnapshot wait_free_snapshot(N);
	updateNum = wait_free_snapshot.execUpdate();

	// print sum of update number of all threads
	cout << "update : " << updateNum << endl;

	return 0;
}
