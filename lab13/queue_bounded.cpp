#include <stdio.h>
#include <inttypes.h>
#include <pthread.h>

#define NUM_PRODUCER                4
#define NUM_CONSUMER                NUM_PRODUCER
#define NUM_THREADS                 (NUM_PRODUCER + NUM_CONSUMER)
#define NUM_ENQUEUE_PER_PRODUCER    10000000
#define NUM_DEQUEUE_PER_CONSUMER    NUM_ENQUEUE_PER_PRODUCER

bool flag_verification[NUM_PRODUCER * NUM_ENQUEUE_PER_PRODUCER];
void enqueue(int key);
int dequeue();

// -------- Queue with coarse-grained locking --------
// ------------------- Modify here -------------------
#define QUEUE_SIZE      1024

struct QueueNode {
    int key;
	int flag;
};

QueueNode queue[QUEUE_SIZE];
uint64_t front;
uint64_t rear;

void init_queue(void) {
    front = 0;
    rear = 0;

	for (int i = 0; i < QUEUE_SIZE; i++)
		queue[i].flag = 0;
}

void enqueue(int key) {
	int rear_val = __sync_fetch_and_add(&rear, 1);
	int idx = rear_val % QUEUE_SIZE;
	int flag_chk = rear_val / QUEUE_SIZE;

	while (flag_chk != queue[idx].flag / 2 || queue[idx].flag % 2 != 0)
		pthread_yield();
				
	queue[idx].key = key;
	queue[idx].flag++;

}

int dequeue(void) {
	int front_val = __sync_fetch_and_add(&front, 1);
	int idx = front_val % QUEUE_SIZE;
	int flag_chk = front_val / QUEUE_SIZE;

	while (flag_chk != queue[idx].flag / 2 || queue[idx].flag % 2 != 1)
		pthread_yield();

	int ret_key = queue[idx].key;
	queue[idx].flag++;
	return ret_key;
}
// ------------------------------------------------

void* ProducerFunc(void* arg) {
    long tid = (long)arg;

    int key_enqueue = NUM_ENQUEUE_PER_PRODUCER * tid;
    for (int i = 0; i < NUM_ENQUEUE_PER_PRODUCER; i++) {
        enqueue(key_enqueue);
        key_enqueue++;
    }

    return NULL;
}

void* ConsumerFunc(void* arg) {
    for (int i = 0; i < NUM_DEQUEUE_PER_CONSUMER; i++) {
        int key_dequeue = dequeue();
        flag_verification[key_dequeue] = true;
    }

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    init_queue();

    for (int i = 0; i < NUM_THREADS; i++) {
        if (i < NUM_PRODUCER) {
            pthread_create(&threads[i], 0, ProducerFunc, (void**)i);
        } else {
            pthread_create(&threads[i], 0, ConsumerFunc, NULL);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // verify
    for (int i = 0; i < NUM_PRODUCER * NUM_ENQUEUE_PER_PRODUCER; i++) {
        if (flag_verification[i] == false) {
            printf("INCORRECT!\n");
            return 0;
        }
    }
    printf("CORRECT!\n");

    return 0;
}

