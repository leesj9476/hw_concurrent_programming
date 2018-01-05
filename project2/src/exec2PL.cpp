#include <thread>
#include <vector>
#include <algorithm>

#include <pthread.h>

#include "exec2PL.h"
#include "transaction.h"
#include "util.h"

using namespace std;

// store trx's i, j, k info
extern vector<TrxInfo> trx_info;

WaitThreadInfo::WaitThreadInfo(int rw_flag_, int id_)
	: rw_flag(rw_flag_) {
		
	id.emplace_back(id_);		
}

Records::Records() 
	: val(100), rw_lock(PTHREAD_RWLOCK_INITIALIZER) {
}

Exec2PL::Exec2PL(int N_, int R_, int E_)
	: N(N_), R(R_), E(E_), record(NULL) {}

Exec2PL::~Exec2PL() {
	for (int i = 0; i < R; i++)
		pthread_rwlock_destroy(&record[i].rw_lock);

	delete[] record;
}

// Initiate records and reserve vector size
void Exec2PL::init2PL() {
	record = new Records[R];

	// set vector<thread> initial size to N
	t.reserve(N);

	// set trx_info size to N and fill it
	trx_info.reserve(N);
	for (int i = 0; i < N; i++)
		trx_info.emplace_back();
}

// make trx thread and join them
void Exec2PL::exec2PLTransactions() {
	init2PL();
	randInit(R);

	// exec trasactions
	for (int i = 0; i < N; i++) {
		t.emplace_back(trx, i, record, E);
	}

	// wait all thread is done
	for (int i = 0; i < N; i++) {
		t[i].join();
	}
}
