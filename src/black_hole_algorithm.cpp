#include "black_hole_algorithm.h"

BlackHoleAlgorithm::BlackHoleAlgorithm(const Options & options, const DocumentSet* docset)
    : options(options),
      black_hole_fitness(numeric_limits<double>::max()),
      black_hole_index(-1),
      docset(docset)
{
    for (unsigned i = 0; i < options.star_count; i++) {
        Star s(&std_generator64, options, docset, i);
        stars.push_back(s);
        double fitness = s.get_current_fitness();
        cout << i << " fitness: " << fitness << endl;

        if (fitness < black_hole_fitness) {
            black_hole_fitness = fitness;
            black_hole_index = i;
        }
    }
    if (black_hole_index != -1) {
        stars[black_hole_index].set_black_hole();
        black_hole = &stars[black_hole_index];
        update_event_horizon();
    } else {
        cout<< "Failed to set black hole." << endl;
        exit(-1);
    }
    cout<< "Starting fitness: " << black_hole_fitness << " when using "
        << options.centroid_count << " centroids." << endl;
}

tuple<Star*, double> BlackHoleAlgorithm::run()
{
    //update_event_horizon();
    for (unsigned i = 0; i < options.star_count; i++) {
        if ((signed)i == black_hole_index) {
            continue;
        }
        stars[i].move_towards_black_hole(*black_hole->get_position());
    }

    int swaps = 0;
    int immediate_swaps = 0;
    int new_stars = 0;
    for (unsigned i = 0; i < options.star_count; i++) {
        if ((signed)i == black_hole_index) {
            continue;
        }
        double fitness = stars[i].get_current_fitness();

        // swap star and black hole if star is fitter
        if (fitness < black_hole_fitness) {
            //cout<< "Swapping star and black-hole" << endl;
            swaps++;
            black_hole->set_not_black_hole();
            stars[i].set_black_hole();
            black_hole = &stars[i];
            black_hole_fitness = fitness;
            black_hole_index = i;
            update_event_horizon();

        // spawn new star if star is closer than event horizon
        } else if (fitness - event_horizon < black_hole_fitness) {
            //cout<< "Creating new star" << endl;
            new_stars++;
            Star s(&std_generator64, options, docset, i);
            stars[i] = s;
            double fitness = s.get_current_fitness();

            // swap star and black hole if star is fitter
            if (fitness < black_hole_fitness) {
                //cout<< "Swapping star and black-hole immediately" << endl;
                immediate_swaps++;
                black_hole->set_not_black_hole();
                stars[i].set_black_hole();
                black_hole_fitness = fitness;
                black_hole = &stars[i];
                black_hole_index = i;
                update_event_horizon();
            }
        }
    }
    if (options.verbose && (swaps + immediate_swaps + new_stars != 0)) {
        if (new_stars) {
            cout << "New stars: " << new_stars << (swaps || immediate_swaps ? ", " : "");
        }
        if (swaps) {
            cout << "Black hole swaps: " << swaps << (immediate_swaps ? ", " : "");;
        }
        if (immediate_swaps) {
            cout << "Immediate swaps: " << immediate_swaps;
        }
        cout << endl;
    }
    return make_tuple(black_hole, black_hole_fitness);
}

void BlackHoleAlgorithm::update_event_horizon()
{
    double total_candidate_fitness = 0;

    for (unsigned i = 0; i < options.star_count; i++) {
        if ((signed)i != black_hole_index) {
            total_candidate_fitness += stars[i].get_current_fitness();
        }
    }
    if (total_candidate_fitness == 0) {
        cout<< "Total candidate fitness is 0." << endl;
    }
    event_horizon = (total_candidate_fitness == 0) ? 0 : black_hole_fitness / total_candidate_fitness;
}
