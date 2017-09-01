#include "star.h"

Star::Star(std::mt19937_64* std_generator64, const Options & options, const DocumentSet* docset, int index)
        : options(options),
          boost_generator64{(*std_generator64)()},
          docset(docset),
          is_black_hole(false)
{
    vector<int> uniform_random_selection(docset->size());

    for (int i = 0, stop = docset->size(); i < stop; i++) {
        uniform_random_selection[i] = i;
    }
    shuffle(uniform_random_selection.begin(), uniform_random_selection.end(), *std_generator64);

    for(unsigned i = 0; i < options.centroid_count; i++) {
        current_position.push_back((*docset)[uniform_random_selection[i]].weights);
    }
    update_fitness();
}

/**
 * xi(t + 1) = xi(t) + rand() * (xBH - xi(t)) i = 1,2,...,N
 */
void Star::move_towards_black_hole(const vector<vector<double>> & black_hole_position)
{
    if (!is_black_hole) {
        for (unsigned i = 0; i < options.centroid_count; i++) {
            for (int j = 0; j < Dimension; j++) {
                current_position[i][j] += get_random() * (black_hole_position[i][j] - current_position[i][j]);
            }
        }
        update_fitness();
    }
    if (options.verbose) {
        //cout<< "fitness: " << current_fitness << endl;
    }
}

void Star::update_fitness()
{
    double total_distance = 0.0;

    for (int i = 0, i_stop = docset->size(); i < i_stop; i++) {
        double min_distance = numeric_limits<double>::max();
        const Document & doc = (*docset)[i];

        for (int j = 0, j_stop = current_position.size(); j < j_stop; j++) {
            double distance = doc.documentDistance(current_position[j]);

            if (distance < min_distance) {
                min_distance = distance;
            }
        }
        total_distance += min_distance;
    }
    current_fitness = total_distance;
}
