#ifndef __UTIL_H__
#define __UTIL_H__

#include <set>
#include <vector>

#include "exec2PL.h"
#include "transaction.h"

// convert argument N, R, E to integer
bool argvToInteger(int &, int &, int &, char **);

// get random number range(0, 1, ..., R-1)
void randInit(int);
int randNum();
void randDelete();

// get random i, j, k
void get_ijk(int &, int &, int &);

// union id vectors to set
void unionVectorsToSet(int, std::set<int> &, Records *, 
							std::vector<TrxInfo> &);

#endif
