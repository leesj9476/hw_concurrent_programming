#ifndef __WAITFREESNAPSHOT_H__
#define __WAITFREESNAPSHOT_H__

#include <vector>
#include <thread>

// MRSW register
typedef struct StampedSnap {
	int stamp;
	long long value;
	int *snap;

	StampedSnap();
	StampedSnap(int, int);
	StampedSnap(int, int, int *);
	~StampedSnap();
} StampedSnap;

// act update using wait-free snapshot about MRSW registers
class WaitFreeSnapshot {
private:
	StampedSnap *table;
	int size;

public:
	WaitFreeSnapshot(int, int);
	~WaitFreeSnapshot();

	StampedSnap * collect();
	void update(int, int);
	int * scan();
};

// wait-free snapshot wrapper class
class ExecWaitFreeSnapshot {
private:
	int N;
	std::vector<std::thread> t;

public:
	ExecWaitFreeSnapshot(int);
	~ExecWaitFreeSnapshot();
	
	long long execUpdate();
};

// wait-free snapshot update wrapper function
void update(int);

// sleep for time and set flag to stop update threads
void timer();

#endif
