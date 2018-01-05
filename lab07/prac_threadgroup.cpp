#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <vector>
#include <mutex>
#include <limits>

#include <math.h>

#define NUM_THREAD_IN_POOL 10 

boost::mutex m;

using namespace std;

typedef struct info {
	int start;
	int end;
	int id;
	int prime;

	info(int s, int e, int i, int p): start(s), end(e), id(i), prime(p) {}
	void print() {
		cout << "(" << id << ") number of primes in " << start << " ~ " << end << " is " << prime << endl;
	}

} info;

vector<info> p_info;

void getPrime(int start, int end, int id) {
	int p = 0;
	bool isPrime = true;

	for (int num=start; num < end; num++) {
		if (num < 2)
			continue;
		
		isPrime = true;
		for (int i=2; i < sqrt(num); i++) {
			if (num%i == 0) {
				isPrime = false;
				break;
			}
		}

		if (isPrime)
			p++;
	}

	m.lock();
	p_info.push_back(info(start, end, id, p));
	p_info.back().print();
	m.unlock();
}

int main (void) {
	boost::asio::io_service io;
	boost::thread_group threadpool;
	boost::asio::io_service::work *work = new boost::asio::io_service::work(io);

	for (int i=0; i < NUM_THREAD_IN_POOL; i++) {
		threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &io));
	}

	int start, end;
	int id=1;
	while(1) {
		cin >> start;
		if (start == -1) {
			break;
		}

		cin >> end;
		io.post(boost::bind(getPrime, start, end, id));

		id++;
	}

	delete work;
	threadpool.join_all();
	io.stop();

	return 0;
}
