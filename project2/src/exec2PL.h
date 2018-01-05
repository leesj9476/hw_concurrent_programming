#ifndef __EXEC_2PL_H__
#define __EXEC_2PL_H__

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

#include <pthread.h>

#define READ  	  0
#define WRITE 	  1

// information about waiting thread
typedef struct WaitThreadInfo {
	int rw_flag;
	std::vector<int> id;

	WaitThreadInfo(int, int);
} WaitThreadInfo;

// record struct
typedef struct Records {
	long long val;

	// read and write lock
	pthread_rwlock_t rw_lock;

	// waiting thread(trx_id) info
	std::vector<WaitThreadInfo> lock_list;
	std::vector<int> lock_list_trx_id;

	// wait mutex and condition variable
	std::mutex wait_mutex;
	std::condition_variable wait_cond;

	Records();
} Records;

//The main class of 2PL execution.
class Exec2PL {
private:
	int N, R, E;
	Records *record;
	std::vector<std::thread> t;

public:
	Exec2PL(int, int, int);
	~Exec2PL();

	// init and execute 2PL trx
	void init2PL();
	void exec2PLTransactions();
};

#endif
