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

#define INIT_VAL		0

typedef struct QueueNode {
    int key;
	struct QueueNode *next;

	QueueNode(int key)
		: key(key), next(NULL) {}
} Node;

Node *head = NULL;
Node *tail = NULL;

void init_queue(void) {
	head = new QueueNode(INIT_VAL);
	tail = head;
}

void enqueue(int key) {
	Node *t = new QueueNode(key);
	Node *tmp = NULL;

	while (true) {
		if (__sync_bool_compare_and_swap(&tail->next, NULL, t)) {
			tail = tail->next;
			break;
		}
	}
}

int dequeue(void) {
	Node *t = NULL;
	int ret;

	while (true) {

		if (head->next != NULL) {
				
		}
	}

	return ret; 
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

