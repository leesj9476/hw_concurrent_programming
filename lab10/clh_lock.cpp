#include <iostream>
#include <thread>
#include <vector>

using namespace std;

#define NUM_THREAD	8
#define NUM_WORK	1000000

int cnt_global = 0;
int gap[128];

typedef struct clh_lock {
	int flag;
	int gap[128];

	clh_lock() {flag = 1;}
} clh_lock;

clh_lock *tail = NULL;

clh_lock *lock(clh_lock *cur) {
	clh_lock *pre = __sync_lock_test_and_set(&tail, cur);

	while (pre->flag == 1)
		this_thread::yield();

	return pre;
}

void unlock(clh_lock *cur, clh_lock *pre) {
	cur->flag = 0;
	delete pre;
}

void work() {
	clh_lock *cur;
	clh_lock *pre;
	for (int i = 0; i < NUM_WORK; i++) {
		cur = new clh_lock();
		pre = lock(cur);
		cnt_global++;
		unlock(cur, pre);
	}
}

int main (void) {
	vector<thread> t;

	tail = new clh_lock();
	tail->flag = 0;

	t.reserve(NUM_THREAD);
	for (int i = 0; i < NUM_THREAD; i++)
		t.emplace_back(work);
	
	for (int i = 0; i < NUM_THREAD; i++)
		t[i].join();

	cout << "cnt_global: " << cnt_global << endl;

	return 0;
}
