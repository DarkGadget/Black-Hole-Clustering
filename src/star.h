#ifndef PARTICLE
#define PARTICLE

#include "document.h"
#include "document_set.h"

#include <stdint.h>
#include <vector>
#include <iostream>
#include <limits>
#include <random>

using namespace std;

class Star {
public:
    Star(std::mt19937_64* std_generator64, const Options & options, const DocumentSet* docset, int index);

    void move_towards_black_hole(const vector<vector<double>> & black_hole_position);
    double get_current_fitness() { return current_fitness; }
    vector<vector<double>>* get_position() { return &current_position; }
    void set_black_hole() { is_black_hole = true; }
    void set_not_black_hole() { is_black_hole = false; }

private:
    void update_fitness();

    vector<vector<double>> current_position;
    Options options;
    boost::mt19937_64 boost_generator64;
    boost::uniform_01<boost::mt19937_64> get_random{boost_generator64};
    double current_fitness = 0.0;
    const DocumentSet* docset;
    bool is_black_hole = false;
};

#endif //PARTICLE
