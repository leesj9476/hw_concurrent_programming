#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__

#include <vector>

#include "exec2PL.h"

#define INFO_NULL	-1

// store i, j, k info through trx progress
typedef struct TrxInfo {
	int i;
	int j;
	int k;

	int ipos;
	int jpos;
	int kpos;

	TrxInfo();

	void resetTrxInfo();
} TrxInfo;

// used for deadlock detection
typedef struct WaitNode {
	int trx_id;
	bool is_root;
	std::vector<WaitNode> wait_trxs;

	WaitNode(int);
	
	void setRoot();
} WaitNode;

// add thread info to lock_list and lock_list_trx_id
bool addReadThreadInfo(int, int, Records *);
bool addWriteThreadInfo(int, int, Records *);

int removeThreadInfo(int, int, Records *);

// deadlock check
bool deadlockCheck(int, WaitNode &, Records *);

// wake up sleep thread by calling notify_all()
void wakeupSleepThread(int, int, Records *, int);

// transaction main function
void trx(int, Records *, int);

#endif
