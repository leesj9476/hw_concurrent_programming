#include <vector>
#include <cstring>
#include <iostream>
#include <thread>

#include "voca.h"
#include "trie.h"

using namespace std;

vector<int> voca_len;
int voca_num = 0;
int q_level = 0; // how to use?
bool dirty = true;
bool QAD_on[3] = {false, false, false};

void vocaAdd(TrieNode *trie, const string &s) {
    TrieNode *cur_node = trie;
    TrieNode *tmp_node = NULL;
    int next;
    dirty = true;
    
    const char *ch = s.c_str();
    for (int i=0; i < s.size(); i++) {
        next = *ch - 'a';
        ////////////////////////////////////////////////
        if (cur_node->child[next] == NULL) {
            tmp_node = new TrieNode();
            tmp_node->level = q_level;
            cur_node->child[next] = tmp_node;
        }
        ////////////////////////////////////////////////

        cur_node = cur_node->child[next];
        ch++;
    }

    if (cur_node->accept == -1) {
        cur_node->accept = voca_num;
        voca_len.push_back(s.size());
        voca_num++;
    }   
    else 
        voca_len[cur_node->accept] = s.size();
}

void vocaDelete(TrieNode *trie, const string &s) {
    //dirty = true;
    TrieNode *cur_node = trie;
    const char *ch = s.c_str();
    int next;

    for (int i=0; i < s.size(); i++) {
        next = *ch - 'a';
        if (cur_node->child[next] == NULL)
            return ;

        cur_node = cur_node->child[next];
        ch++;
    }
    
    voca_len[cur_node->accept] = 0;
}

void vocaQuery(TrieNode *trie, const string &s) {
    if (dirty) {
        calcFail(trie);
        dirty = false;
    }

    q_level++;
    ahoCorasick(s, trie, voca_len, q_level);
}

void runVocaProject(TrieNode *trie) {
    char op;
    string voca;
    while (cin >> op) {
        cin.get();
        getline(cin, voca);
        switch (op) {
            case 'Q':
                QAD_on[0] = true;
                vocaQuery(trie, voca);
                QAD_on[0] = false;
                break;
            
            case 'A':
                QAD_on[1] = true;
                vocaAdd(trie, voca);
                QAD_on[1] = false;
                break;
            
            case 'D':
                QAD_on[2] = true;
                vocaDelete(trie, voca);
                QAD_on[2] = false;
                break;
        }
    }
}
