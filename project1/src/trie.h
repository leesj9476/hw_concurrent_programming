#ifndef __TRIE_H__
#define __TRIE_H__

#include <string>
#include <vector>
#include <set>
#include <map>

const int ALPHANUM = 26;

typedef struct TrieNode {
    TrieNode *child[ALPHANUM];
    int accept;
    int level;
    TrieNode *fail;
    TrieNode *output;

    TrieNode();
    ~TrieNode();
} TrieNode;

void calcFail(TrieNode *root);

void ahoCorasick(const std::string& s, TrieNode *trie, std::vector<int> &voca_len, int q_level);

#endif
