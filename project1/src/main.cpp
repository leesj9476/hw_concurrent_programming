#include <pthread.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <map>

#include "trie.h"
#include "voca.h"

using namespace std;

int main (void) {
    ios_base::sync_with_stdio(false);
    int input_num;
    string input_voca;
    TrieNode *trie = new TrieNode();

    cin >> input_num;

    for (int i=0; i < input_num; i++) {
        cin >> input_voca;
        vocaAdd(trie, input_voca);
    }

    cout << "R\n";
    runVocaProject(trie);

    delete trie;

    return 0;
}
