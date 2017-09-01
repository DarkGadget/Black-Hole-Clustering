#ifndef CLUSTERING
#define CLUSTERING

#include "global.h"
#include "parse_cmd_args.h"
#include "document.h"
#include "document_set.h"
#include "black_hole_algorithm.h"
#include "star.h"

#include <stdint.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace std::chrono;

// These are declared in global.h
int Dimension = 0;
vector<double> MaxDimensions(0);

string timeElapsed(const chrono::high_resolution_clock::time_point & begin, const chrono::high_resolution_clock::time_point & end);

#endif //CLUSTERING
