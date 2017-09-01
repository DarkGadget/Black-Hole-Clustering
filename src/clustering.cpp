#include "clustering.h"

string timeElapsed(const high_resolution_clock::time_point & begin, const high_resolution_clock::time_point & end)
{
    long total_microseconds = duration_cast<std::chrono::microseconds>(end - begin).count();
    long days = total_microseconds / 86400000000;
    long hours = (total_microseconds % 86400000000) / 3600000000;
    long mins = (total_microseconds % 3600000000) / 60000000;
    long secs = (total_microseconds % 60000000) / 1000000;
    long ms = (total_microseconds % 1000000) / 1000;
    ostringstream time;
    time << (days ? to_string(days) + " days, " : "")
         << (hours ? to_string(hours) + ":" : "00:")
         << setfill('0')
         << setw(2) << mins << ":"
         << setw(2) << secs << "."
         << setw(3) << ms;
    return time.str();
}

int main(int argc, char **argv)
{
    Options options(processCmdLineArgs(argc, argv));
    locale::global(locale("en_US.UTF-8"));

    if (options.perform_run) {
        for (int i = 0; i < argc; i++) {
            cout << argv[i] << " ";
        }
        cout << endl;
        cout << setprecision(32);
        high_resolution_clock::time_point total_start = high_resolution_clock::now();

        DocumentSet docset(options);
        if (options.verbose) {
            cout << "Time taken so to process files: " << timeElapsed(total_start, high_resolution_clock::now()) << endl;
        }
        high_resolution_clock::time_point algorithm_start = high_resolution_clock::now();

        BlackHoleAlgorithm black_hole_algorithm(options, &docset);

        if (options.verbose) {
            cout << "Time to create universe: " << timeElapsed(algorithm_start, high_resolution_clock::now()) << endl;
        }

        cout<< "Using " << options.centroid_count << " centroids, and " << options.star_count << " stars." << endl;

        double best_fitness = -1;
        Star* best_solution = nullptr;

        for (unsigned i = 0; i < options.num_iterations; i++) {
            high_resolution_clock::time_point iteration_start = high_resolution_clock::now();
            tie(best_solution, best_fitness) = black_hole_algorithm.run();
            cout<< "Iteration " << (i + 1) << ":\tbest_fitness: " << best_fitness << endl;
            cout<< "Cycle time: " << timeElapsed(iteration_start, high_resolution_clock::now()) << endl;
            cout<< "Total time: " << timeElapsed(total_start, high_resolution_clock::now()) << endl;
        }
        if (!best_solution) {
            cout<< "No best solution." << endl;
            exit(-1);
        }

        vector<pair<int, Document*>> centroid_docs;
        vector<vector<double>>* centroids = (*best_solution).get_position();

        for (int i = 0, i_stop = centroids->size(); i < i_stop; i++) {
            const vector<double> & centroid = (*centroids)[i];
            int pos = -1;
            double distance = numeric_limits<double>::max();
            Document* doc = nullptr;

            for (int j = 0, j_stop = docset.size(); j < j_stop; j++) {
                double d = docset[j].documentDistance(centroid);
                if (d <= distance) {
                    distance = d;
                    pos = j;
                    doc = &docset[j];
                }
            }
            if (doc) {
                centroid_docs.push_back(make_pair(pos, doc));
            }
        }
        if (centroid_docs.size() != options.centroid_count) {
            cout<< "Mismatch in centroid-count (" << options.centroid_count
                << ") and documents (" << centroid_docs.size() << ") found. Exiting." << endl;
            exit(-1);
        }

        cout<< "Found " << centroid_docs.size() << " documents closest to the centroids." << endl;

        int i = 1;
        for (auto & d : centroid_docs) {
            cout<< i++ << " Centroid: " << d.first << " " << (*d.second) << endl;
        }

        vector<tuple<int, Document*, double>> clustered_docs;

        for (int i = 0, i_stop = docset.size(); i < i_stop; i++) {
            Document* doc = &docset[i];
            double distance = numeric_limits<double>::max();
            int centroid_index = -1;

            for (int j = 0, j_stop = centroids->size(); j < j_stop; j++) {
                double d = doc->documentDistance((*centroids)[j]);
                if (d <= distance) {
                    distance = d;
                    centroid_index = j;
                }
            }
            if (centroid_index != -1) {
                clustered_docs.push_back(make_tuple(centroid_index, doc, distance));
            }
        }
        if (clustered_docs.size() != docset.size()) {
            cout<< "Mismatch in clustered_docs (" << clustered_docs.size()
                << ") and documents (" << docset.size() << ") found. Exiting." << endl;
            exit(-1);
        }
        //sort(std::begin(clustered_docs), std::end(clustered_docs));
        sort(std::begin(clustered_docs), std::end(clustered_docs),
            [] (const tuple<int, Document*, double> & lhs, const tuple<int, Document*, double> & rhs)
               //{ return get<2>(lhs) < get<2>(rhs); });
               { return get<1>(lhs)->path < get<1>(rhs)->path; });

        int prev = -1;
        for (int i = 0, i_stop = centroids->size(); i < i_stop; i++) {
            for (int j = 0, j_stop = docset.size(); j < j_stop; j++) {
                if (get<0>(clustered_docs[j]) == i) {
                    if (prev != i) {
                        cout << endl;
                        prev = i;
                    }
                    cout<< "Cluster: " << (i + 1) << " " << (*get<1>(clustered_docs[j]))
                        << " distance: " << get<2>(clustered_docs[j]) << endl;
                }
            }
        }

        vector<int> cluster_counts(centroids->size());

        for (int i = 0, i_stop = docset.size(); i < i_stop; i++) {
            double distance = numeric_limits<double>::max();
            int centroid_index = -1;

            for (int j = 0, j_stop = centroids->size(); j < j_stop; j++) {
                double d = docset[i].documentDistance((*centroids)[j]);
                if (d <= distance) {
                    distance = d;
                    centroid_index = j;
                }
            }
            if (centroid_index != -1) {
                cluster_counts[centroid_index] += 1;
            }
        }

        for (int i = 0, i_stop = cluster_counts.size(); i < i_stop; i++) {
            cout<< "Cluster: " << (i + 1) << " contains " << cluster_counts[i] << " documents." << endl;
        }

        if (options.verbose) {
            cout << "Time taken in total: " << timeElapsed(total_start, high_resolution_clock::now()) << endl;
        }
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
