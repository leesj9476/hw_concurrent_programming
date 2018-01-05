#ifndef __VOCA_H__
#define __VOCA_H__ 

#include <string>

#include "trie.h"

void vocaAdd(TrieNode *trie, const std::string &s);

void vocaDelete(TrieNode *trie, const std::string &s);

void vocaQuery(TrieNode *trie, const std::string &s);

void runVocaProject(TrieNode *trie);

#endif
