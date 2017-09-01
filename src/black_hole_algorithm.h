#ifndef BLACK_HOLE
#define BLACK_HOLE

#include "global.h"
#include "parse_cmd_args.h"
#include "document_set.h"
#include "document.h"
#include "star.h"

#include <stdint.h>
#include <vector>
#include <iostream>
#include <limits>
#include <random>

using namespace std;

class BlackHoleAlgorithm {

public:
    BlackHoleAlgorithm(const Options & options, const DocumentSet* docset);
    tuple<Star*, double> run();

private:
    void update_event_horizon();

    Options options;
    std::mt19937_64 std_generator64 {options.rand_seed};
    vector<Star> stars;
    Star* black_hole = nullptr;
    double black_hole_fitness;
    double event_horizon;
    int black_hole_index = -1;
    const DocumentSet* docset;
};

#endif //BLACK_HOLE
