#include "parse_cmd_args.h"

Options processCmdLineArgs(int argc, char **argv)
{
    Options options;
    options_description desc("\nClustering proof of concept.\n\nAllowed arguments");
    desc.add_options()
        ("help,h", "Produce this help message.")
        ("centroids,c", value<int>()->default_value(options.centroid_count), "Set the number of centroids to use.")
        ("stars,a", value<int>()->default_value(options.particle_count), "Set the number of stars to use.")
        ("iterations,i", value<int>()->default_value(options.num_iterations), "Set the number of iterations to run.")
        ("random-seed,m", "Set the random number seed to use.")
        ("path,p", value<vector<string>>(),
         "Directory containing, or path of file listing the paths, of documents to cluster.")
        ("iris,s", "Use the iris data set.")
        ("wine,w", "Use the wine data set.")
        ("verbose,v", "Verbose output (including file-names and times).")
        ("quiet,q", "Quiet mode, only outputting basic statistics.");
    variables_map vm;

    try {
        store(command_line_parser(argc, argv).options(desc).run(), vm);
        notify(vm);
    } catch (std::exception &e) {
        options.perform_run = false;
        cout << endl << e.what() << endl;
        cout << desc << endl;
        return options;
    }

    if (vm.count("help") || argc <= 1) {
        options.perform_run = false;
        cout << desc << endl;
        return options;
    }

    if (vm.count("centroids")) {
        if (vm["centroids"].as<int>() <= 0) {
            options.perform_run = false;
            cout << "Need a --centroids value > 0" << endl;
        } else {
            options.centroid_count = vm["centroids"].as<int>();
        }
    }

    if (vm.count("stars")) {
        if (vm["stars"].as<int>() <= 0) {
            options.perform_run = false;
            cout << "Need a --stars value > 0" << endl;
        } else {
            options.star_count = vm["stars"].as<int>();
        }
    }

    if (vm.count("iterations")) {
        if (vm["iterations"].as<int>() <= 0) {
            options.perform_run = false;
            cout << "Need a --iterations value > 0" << endl;
        } else {
            options.num_iterations = vm["iterations"].as<int>();
        }
    }

    options.rand_seed = vm.count("random_seed") ?
                        (unsigned)vm["random_seed"].as<int>() : system_clock::now().time_since_epoch().count();
    srand(options.rand_seed);

    if (vm.count("path") + vm.count("iris") + vm.count("wine") > 1) {
        options.perform_run = false;
        cout << "Can only specify one of --path, --iris, or --wine" << endl;
    } else if (vm.count("path") && (is_directory(vm["path"].as< vector<string> >()[0])
                               || is_regular_file(vm["path"].as< vector<string> >()[0]))) {
        options.path = vm["path"].as< vector<string> >()[0];
    } else if (vm.count("iris")) {
        options.iris = true;
    } else if (vm.count("wine")) {
        options.wine = true;
    } else {
        options.perform_run = false;
        cout << "Need a --path value that is either a file or a directory" << endl;
    }

    if (vm.count("verbose") && vm.count("quiet")) {
        options.perform_run = false;
        cout << "Cannot specify --verbose and --quiet" << endl;
    } else if (vm.count("verbose")) {
        options.verbose = true;
    } else if (vm.count("quiet")) {
        options.quiet = true;
    }

    return options;
}
