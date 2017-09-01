#ifndef PARSE_CMD_ARGS
#define PARSE_CMD_ARGS

#include <chrono>
#include <iostream>
#include <vector>
#include <string>

#define BOOST_NO_CXX11_SCOPED_ENUMS // fixes linker error when compiling on Linux

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/range/iterator_range.hpp>

using namespace std;
using namespace std::chrono;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace boost::program_options;

// An options object is used to store user command line options.
struct Options {
    bool perform_run = true;
    string path;
    //test data, see document_set_data.cpp
    bool iris = false;
    bool wine = false;
    // Control amount of program output
    bool verbose = false;
    bool quiet = false;
    // When processing the files, rather than output tons of lines for each file on the console,
    // it uses \r to update the same console line over and over again. This technique doesn't work when
    // the output is redirected to a file, so this is used to detect and avoid that problem.
    bool have_stdout = isatty(fileno(stdout));
    unsigned rand_seed = 3725841767;

    // Particle Swarm Optimization algorithm
    unsigned particle_count = 10;   // number of particles
    //unsigned centroid_count = 4;    // number of centroids in each particle
    unsigned num_iterations = 100;  // number of iterations to run

    // Black Hole algorithm
    unsigned star_count = 20;
    unsigned centroid_count = 4;    // number of centroids in each star
};

/**
 * Process the user's command line input
 *
 * @param   int     argc    length of argv
 * @param   char**  argv    array of char*
 * @return  Options         Options object with values set appropriately
 */
Options processCmdLineArgs(int argc, char** argv);

#endif //PARSE_CMD_ARGS
