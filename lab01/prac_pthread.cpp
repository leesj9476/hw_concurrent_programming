#include <pthread.h>
#include <stdio.h>

#define NUM_THREAD		100
#define NUM_INCREASE	100000000

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

int cnt_global = 0;

long int ThreadFunc1() {
    long cnt_local = 0;

    for (int i=0; i < NUM_INCREASE; i++) {
        if (i % 10 == 0)
            cnt_global++;
        cnt_local++;
    }

    return cnt_local;
}

void *ThreadFunc(void *arg) {
	long cnt_local = 0;

    pthread_mutex_lock(&m);
	for (int i=0; i < NUM_INCREASE; i++) {
        if (i % 10 == 0)
		    cnt_global++;
		cnt_local++;
	}
    pthread_mutex_unlock(&m);

	return (void *)cnt_local;
}

int main (void) {/*
	pthread_t threads[NUM_THREAD];

	for (int i=0; i < NUM_THREAD; i++) {
		if (pthread_create(&threads[i], 0, ThreadFunc, NULL) < 0) {
			printf("pthread_create error!\n");
			return 0;
		}
	}

	long ret;
	for (int i=0; i < NUM_THREAD; i++) {
		pthread_join(threads[i], (void **)&ret);
		printf("thread %ld, local count: %ld\n", threads[i], ret);
	}*/
    
    for (int i=0; i < NUM_THREAD; i++)
        printf("id %d, local count: %ld\n", i, ThreadFunc1());

	printf("global count: %d\n", cnt_global);

	return 0;
}
