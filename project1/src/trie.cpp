#include <iostream>
#include <cstring>
#include <queue>
#include <vector>

#include "trie.h"

using namespace std;

extern vector<int> voca_len;

TrieNode::TrieNode() : accept(-1), level(-1), fail(NULL), output(NULL) {
    memset(child, 0, sizeof(child));
}
    
TrieNode::~TrieNode() {
    for (int i=0; i < ALPHANUM; i++) {
        if (child[i])
            delete child[i];
    }
}

void calcFail(TrieNode *root) {
    queue<TrieNode *> q;
    TrieNode *cur_node;

    root->fail = root;
    q.push(root);
    while(!q.empty()) {
        cur_node = q.front();
        q.pop();

        for (int i=0; i < ALPHANUM; i++) {
            TrieNode *cur_child = cur_node->child[i];
            if (cur_child == NULL)
                continue;

            if (cur_node == root)
                cur_child->fail = root;
            else {
                TrieNode *t = cur_node->fail;
                while (t != root && t->child[i] == NULL)
                    t = t->fail;

                if (t->child[i])
                    t = t->child[i];

                cur_child->fail = t;
            }

            cur_child->output = cur_child->fail;

            q.push(cur_child);
        }
    }
}

void ahoCorasick(const string& s, TrieNode *trie, vector<int> &voca_len, int q_level) {
    TrieNode *state = trie;
    set<int> found_id;
    multimap<int, int> print_set;

    for (int i=0; i < s.size(); i++) {
        char ch = s[i] - 'a';

        // while: state isn't root node && can't go child[ch]
        while (state != trie && (state->child[ch] == NULL || (state->child[ch] != NULL && q_level <= state->level)))
            state = state->fail;

        // tha case while is broken by "state != trie" condition
        if (state->child[ch] && q_level > state->level)
            state = state->child[ch];

        TrieNode *t = state;

        while (t != trie) {
            if (t->accept != -1 && voca_len[t->accept] != 0 && found_id.find(t->accept) == found_id.end()) {
                found_id.insert(t->accept);
                print_set.insert(pair<int, int> (i - voca_len[t->accept] + 1, i));
            }

            t = t->output;    
        }
    }

    int print_num = print_set.size();
    if (print_num == 0) {
        cout << -1 << endl;
        return ;
    }

    //////////////////////////////////////////////////
    multimap<int, int>::const_iterator iter;
    for (iter = print_set.begin(); iter != print_set.end(); iter++, print_num--) {
        for (int j = iter->first; j <= iter->second; j++)
            cout << s[j];

        if (print_num == 1)
            cout << "\n";
        else
            cout << "|";
    }
    //////////////////////////////////////////////////
}
