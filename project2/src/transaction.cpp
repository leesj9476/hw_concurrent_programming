#include <mutex>
#include <iostream>
#include <fstream>
#include <set>
#include <string>

#include <pthread.h>

#include "exec2PL.h"
#include "transaction.h"
#include "util.h"

using namespace std;

// global mutex
mutex g_mutex;

// commit id
int g_exec_order = 0;

TrxInfo::TrxInfo()
	: i(INFO_NULL), j(INFO_NULL), k(INFO_NULL), 
		ipos(INFO_NULL), jpos(INFO_NULL), kpos(INFO_NULL) {}

// reset i, j, k waiting info to INFO_NULL(-1)
void TrxInfo::resetTrxInfo() {
	i = INFO_NULL;
	j = INFO_NULL;
	k = INFO_NULL;

	ipos = INFO_NULL;
	jpos = INFO_NULL;
	kpos = INFO_NULL;
}

// store trx pos in record waiting list
vector<TrxInfo> trx_info;
	
WaitNode::WaitNode(int trx_id_)
	: trx_id(trx_id_), is_root(false) {}

void WaitNode::setRoot() {
	is_root = true;
}

// add read info(trx_id) to lock_list struct and return true if wait or false
bool addReadThreadInfo(int trx_id, int record_id, Records *record) {
	Records &r = record[record_id];
	if (r.lock_list.empty() == true || r.lock_list.back().rw_flag == WRITE)
		// add info to last of lock_list
		r.lock_list.emplace_back(READ, trx_id);
	else
		// add info to last of lock_list(already exist struct)
		r.lock_list.back().id.push_back(trx_id);
	
	// store all trx_id
	r.lock_list_trx_id.push_back(trx_id);

	// wait check
	if (r.lock_list.size() == 1)
		return false;
	else
		return true;
}

// add write info(trx_id) to lock_list struct and return true if wait or false
bool addWriteThreadInfo(int trx_id, int record_id, Records *record) {
	Records &r = record[record_id];

	// add info to last of lock_list
	r.lock_list.emplace_back(WRITE, trx_id);

	// store all trx_id
	r.lock_list_trx_id.push_back(trx_id);

	// wait check
	if (r.lock_list.size() == 1)
		return false;
	else
		return true;
}

// trx_id: trx(called this deadlock check function) id
// so we need to find trx_id through searching waiting threads(using DFS)
// -> if circular(find trx_id), deadlock has occured and need to be recorvered
bool deadlockCheck(int trx_id, WaitNode &parent, Records *record) {
	// if circular situation, return true
	if (parent.is_root == false && trx_id == parent.trx_id)
		return true;

	// find all waiting trx id
	set<int> wait_trxs;
	unionVectorsToSet(parent.trx_id, wait_trxs, record, trx_info); 

	// if no circular situation, return false
	if (wait_trxs.empty() == true)
		return false;

	// add childs
	for (auto &x : wait_trxs) {
		parent.wait_trxs.emplace_back(x);
	}

	// deadlock check using DFS
	bool deadlock_check_result = false;
	for (auto &x : parent.wait_trxs) {
		deadlock_check_result = deadlockCheck(trx_id, x, record);

		// if find circular, stop navigating
		if (deadlock_check_result == true)
			break;
	}
	
	return deadlock_check_result;
}

// wakeup waiting threads
void wakeupSleepThread(int record_id, int rw, Records *record, int deleted_idx) {
	Records &r = record[record_id];

	// if rw == WRITE -> regardless of next READ/WRITE, we can start next trx
	// if rw == READ -> we can start next trx only when all READ trx is done
	if (deleted_idx == 0 &&
		r.lock_list.empty() == false &&
			((rw == WRITE) || 
			(rw == READ && r.lock_list[0].rw_flag == WRITE))) {

		// wakeup call
		r.wait_cond.notify_all();
	}
}

int removeThreadInfo(int trx_id, int record_id, Records *record) {
	Records &r = record[record_id];

	int i;
	vector<int>::iterator iter;

	// reset trx info about i, j, k
	trx_info[trx_id].resetTrxInfo();

	// find and erase it from lock_list
	for (i = 0; i < r.lock_list.size(); i++) {
		for (iter = r.lock_list[i].id.begin(); iter != r.lock_list[i].id.end(); iter++) {
			if (*iter == trx_id)
				break;
		}

		if (iter != r.lock_list[i].id.end()) {
			if (r.lock_list[i].id.size() == 1) {
				// if id has only trx_id, erase that element
				r.lock_list.erase(r.lock_list.begin() + i);

				// adjust record pos
				for (auto &x : trx_info) {
					if (x.i == record_id && x.ipos > 1)
						x.ipos--;
					else if (x.j == record_id && x.jpos > 1)
						x.jpos--;
					else if (x.k == record_id && x.kpos > 1)
						x.kpos--;
				}
			}
			else {
				// delete from READ object, then do not need to wakeup call
				r.lock_list[i].id.erase(iter);
				i = -1;
			}

			break;
		}
	}

	// find and erase it from lock_list_trx_id vector
	for (iter = r.lock_list_trx_id.begin(); iter != r.lock_list_trx_id.end(); iter++) {
		if (*iter == trx_id)
			break;
	}

	if (iter != r.lock_list_trx_id.end())
		r.lock_list_trx_id.erase(iter);

	// if i == 0 wakeup waiting threads
	return i;
}

// execute transaction
// read record_i
// record_j <- record_j + record_i + 1
// record_k <- record_k - record_i
void trx(int trx_id, Records *record, int E) {
	// make and open thread log file
	string filename = "output/thread" + to_string(trx_id + 1) + ".txt";
	ofstream log_file(filename, ios::out | ios::trunc);

	bool is_deadlock = false;
	int i, j, k;
	while (g_exec_order < E) {
		// get random i, j, k and reset trx_info to INFO_NULL(-1)
		if (is_deadlock == false)
			get_ijk(i, j, k);
	
		long long record_i;
		bool wait = false;
		is_deadlock = false;
	
		////////////////////// record i //////////////////////
		g_mutex.lock();

		// add thread info to record.lock_list
		wait = addReadThreadInfo(trx_id, i, record);
		trx_info[trx_id].i = i;
		trx_info[trx_id].ipos = record[i].lock_list.size();

		// if need to wait, sleep until notify_all() called
		if (wait) {
			// about i, wait graph cannot make cycle, 
			// so don't occur deadlock situation -> deadlock check... X

			// wait condition
			g_mutex.unlock();
			{
			unique_lock<mutex> lk_i(record[i].wait_mutex);
			record[i].wait_cond.wait(lk_i, [trx_id] { 
				return trx_info[trx_id].ipos == 1;
			});
			}
			g_mutex.lock();
		}

		// get read lock
		pthread_rwlock_rdlock(&record[i].rw_lock);
		g_mutex.unlock();
		record_i = record[i].val;

		////////////////////// record j //////////////////////
		g_mutex.lock();

		// add thread info to record.lock_list
		wait = addWriteThreadInfo(trx_id, j, record);
		trx_info[trx_id].j = j;
		trx_info[trx_id].jpos = record[j].lock_list.size();

		// if need to wait, sleep until notify_all() called
		if (wait) {
			// deadlock check. if deadlock occur, move all thread info to back
			// and restart trx
			WaitNode root(trx_id);
			root.setRoot();
			is_deadlock = deadlockCheck(trx_id, root, record);
			if (is_deadlock) {
				// remove thread info from lock_list and lock_list_trx_id
				int lock_list_idx_i = removeThreadInfo(trx_id, i, record);
				removeThreadInfo(trx_id, j, record);


				// wake up other waiting thread and restart
				wakeupSleepThread(i, READ, record, lock_list_idx_i);

				// unlock read lock
				pthread_rwlock_unlock(&record[i].rw_lock);

				g_mutex.unlock();

				continue;
			}

			// wait condition
			g_mutex.unlock();
			while (trx_info[trx_id].jpos != 1);
			/*{
			unique_lock<mutex> lk_j(record[j].wait_mutex);
			record[j].wait_cond.wait(lk_j, [trx_id] { 
				return trx_info[trx_id].jpos == 1;
			});
			}*/
			g_mutex.lock();
		}

		// get write lock and do trx action
		pthread_rwlock_wrlock(&record[j].rw_lock);
		g_mutex.unlock();
		record[j].val += record_i + 1;
	
		////////////////////// record k //////////////////////
		g_mutex.lock();

		// add thread info to record.lock_list
		wait = addWriteThreadInfo(trx_id, k, record);
		trx_info[trx_id].k = k;
		trx_info[trx_id].kpos = record[k].lock_list.size();

		// if need to wait, sleep until notify_all() called
		if (wait) {
			// deadlock check. if deadlock occur, move all thread info to back
			// and restart trx
			WaitNode root(trx_id);
			root.setRoot();
			is_deadlock = deadlockCheck(trx_id, root, record);
			if (is_deadlock) {
				// remove thread info from lock_list and lock_list_trx_id
				int lock_list_idx_i = removeThreadInfo(trx_id, i, record);
				int lock_list_idx_j = removeThreadInfo(trx_id, j, record);
				removeThreadInfo(trx_id, k, record);

				// undo
				record[j].val -= record_i + 1;

				// wake up other waiting thread and restart
				wakeupSleepThread(i, READ, record, lock_list_idx_i);
				wakeupSleepThread(j, WRITE, record, lock_list_idx_j);

				// unlock read and write lock
				pthread_rwlock_unlock(&record[i].rw_lock);
				pthread_rwlock_unlock(&record[j].rw_lock);

				g_mutex.unlock();
				continue;
			}

			// wait condition
			g_mutex.unlock();
			while (trx_info[trx_id].kpos != 1);
			/*{
			unique_lock<mutex> lk_k(record[k].wait_mutex);
			record[k].wait_cond.wait(lk_k, [trx_id] { 
				return trx_info[trx_id].kpos == 1;
			});
			}*/
			g_mutex.lock();
		}

		// get write lock and do trx action
		pthread_rwlock_wrlock(&record[k].rw_lock);
		g_mutex.unlock();
		record[k].val -= record_i;
	
		/////////////////////// commit ///////////////////////
		g_mutex.lock();

		// remove thread info from record.lock_list and record.lock_list_trx_id
		removeThreadInfo(trx_id, i, record);
		removeThreadInfo(trx_id, j, record);
		removeThreadInfo(trx_id, k, record);

		// wakeup READ(record[i]), WRITE(record[j], record[k])
		wakeupSleepThread(i, READ, record, 0);
		wakeupSleepThread(j, WRITE, record, 0);
		wakeupSleepThread(k, WRITE, record, 0);

		// unlock read and write lock
		pthread_rwlock_unlock(&record[i].rw_lock);
		pthread_rwlock_unlock(&record[j].rw_lock);
		pthread_rwlock_unlock(&record[k].rw_lock);
	
		g_exec_order++;

		// if commit id is over E, undo and terminate trx
		if (g_exec_order > E) {
			// undo
			record[j].val -= record_i + 1;
			record[k].val += record_i;
			g_mutex.unlock();

			// close log file
			log_file.close();

			return ;
		}

		// append commit log
		log_file << g_exec_order << " " << i << " " << j << " " << k << " ";
		log_file << record[i].val << " " << record[j].val << " " << record[k].val << endl;

		g_mutex.unlock();
	}

	// close log file
	log_file.close();
}
