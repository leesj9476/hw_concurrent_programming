#include <chrono>

#include "waitfreesnapshot.h"

// timeout (sec)
const int timeout = 60;

using namespace std;

// thread timeout flag
bool stop = false;

// wait-free snapshot wrapper
WaitFreeSnapshot *WFSnapshot;

// MRSW register
StampedSnap::StampedSnap()
	: stamp(0), value(0), snap(NULL) {}

StampedSnap::StampedSnap(int value_, int size_)
	: stamp(0), value(value_), snap(NULL) {

	snap = new int[size_];
}

StampedSnap::StampedSnap(int stamp_, int value_, int *snap_)
	: stamp(stamp_), value(value_) {

	snap = snap_;
}

StampedSnap::~StampedSnap() {
}


WaitFreeSnapshot::WaitFreeSnapshot(int size_, int init_value_) {
	// MRSW register array
	table = new StampedSnap[size_];
	size = size_;
	for (int i=0; i < size; i++) {
		table[i].value = init_value_;
		table[i].snap  = new int[size];
	}
}

WaitFreeSnapshot::~WaitFreeSnapshot() {
}

// return copy of i'th MRSW register
StampedSnap* WaitFreeSnapshot::collect() {
	StampedSnap *copy = new StampedSnap[size];
	for (int i = 0; i < size; i++) {
		copy[i].snap  = new int[size];

		copy[i].stamp = table[i].stamp;
		copy[i].value = table[i].value;
		copy[i].snap  = table[i].snap;
	}	

	return copy;
}

// scan current MRSW registers and update value
void WaitFreeSnapshot::update(int id, int value) {
	int* snap = scan();

	if (snap) {
		StampedSnap old_value = table[id];
		StampedSnap new_value = StampedSnap(old_value.stamp + 1, value, snap);
	
		delete[] table[id].snap;
		table[id] = new_value;
	}
}

// capture MRSW registers and return that
int * WaitFreeSnapshot::scan() {
	StampedSnap* old_copy = NULL;
	StampedSnap* new_copy = NULL;

	// check if it is checked once
	vector<bool> moved(size);

	// condigion flag
	bool jump_flag = false;

	// get register's copy
	old_copy = collect();
	while (!stop) {
		if (new_copy != NULL)
			delete[] new_copy;

		// get register's copy once again
		new_copy = collect();
		for (int i = 0; i < size; i++) {
			if (old_copy[i].stamp != new_copy[i].stamp) {
				// if checked twice at same i, return i's snapshot
				if (moved[i]) {
					delete[] new_copy;
					return old_copy[i].snap;
				}
				// if checked once at i, exchange old_copy to new_copy
				// and continue while statement
				else {
					moved[i] = true;
					old_copy = new_copy;
					jump_flag = true;
					break;
				}
			}
		}

		// if old_coyp == new_copy
		// return copy of new_copy's values
		if (jump_flag == false) {
			int *result = new int[size];
			for (int i = 0; i < size; i++)
				result[i] = new_copy[i].value;
	
			delete[] old_copy;

			return result;
		}

		// set flag to default value to reenter to while
		jump_flag = false;
	}

	return NULL;
}

ExecWaitFreeSnapshot::ExecWaitFreeSnapshot(int N_)
	: N(N_) {
	
	// reserve vector space to size N and initiate
	t.reserve(N + 1);
	WFSnapshot = new WaitFreeSnapshot(N, 0);
}

ExecWaitFreeSnapshot::~ExecWaitFreeSnapshot() {
	delete WFSnapshot;
}

long long ExecWaitFreeSnapshot::execUpdate() {
	// start timer and update threads and join them
	t.emplace_back(timer);
	for (int i = 0; i < N; i++) {
		t.emplace_back(update, i);
	}

	for (int i = 0; i < N + 1; i++) {
		if (t[i].joinable())
			t[i].join();
	}

	// calculate all update_num and return it
	long long updateCnt = 0;
	StampedSnap *result = WFSnapshot->collect();
	for (int i = 0; i < N; i++)
		updateCnt += result[i].stamp;

	delete[] result;

	return updateCnt;
}

void update(int id) {
	long long update_value = 1;

	// 1 min loop
	while (!stop) {
		WFSnapshot->update(id, update_value);
		update_value++;
	}
}

// sleep for timeout (sec) and set stop flag to true
void timer() {
	chrono::seconds sec(timeout);
	this_thread::sleep_for(sec);
	stop = true;
}
