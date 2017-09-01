#ifndef DOCUMENT_SET
#define DOCUMENT_SET

#include "global.h"
#include "parse_cmd_args.h"
#include "document.h"
#include "porter2_stemmer.h" //3rd party

#include <assert.h>
#include <stdint.h>
#include <sys/stat.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <map>
#include <vector>
#include <unordered_set>

#define BOOST_NO_CXX11_SCOPED_ENUMS // fixes linker error when compiling on Linux

#include <boost/iostreams/device/file.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/random.hpp>

using namespace std;
using namespace boost;

struct Stats {
	unsigned global_word_freq = 0;
	unsigned global_word_index = 0;

	Stats() {}

	Stats(unsigned tmp_global_word_freq, unsigned tmp_global_word_index) :
		global_word_freq(tmp_global_word_freq), global_word_index(tmp_global_word_index) {}

	Stats(const Stats & stat) :
		global_word_freq(stat.global_word_freq), global_word_index(stat.global_word_index) {}
};

class DocumentSet {

public:
    /**
     * Ctor for DocumentSet. Creates new directory for bitstring caches, initialises the SVM, and calls initFiles()
     *
     * @param   const Options &     options
     */
    DocumentSet(const Options & options);

    /**
     * Train the SVM on the training set, then rank the testing set, and move the top scoring options.batch_size-items
     * into the training set.
     */
    void runClustering();
    unsigned int size() const { return documents.size(); }
    Document operator  [](unsigned long index) const { return documents[index]; }
    Document & operator [](const unsigned long index) { return documents[index]; }

private:
    Options options;

    // Left-over hack from testing on Enron documents, which have a boilerplate disclaimer
    boost::regex enronRegex {"\\*+[^\\*]+\\*+"};
    boost::regex tfidfRegex {"[[:space:]]+|[()]|\\.{2,}"};
    boost::regex outerPunctRegex {"^[^[:alpha:]]+|[^[:alpha:]]+$"};
    boost::regex wordRegex {"[[:alpha:]]+"};
    //boost::regex validTokenRegex {"^(?:[[:alpha:]\\.=&'\\-@]+[/#]*)+$|^[[:digit:]:]+^"};
    boost::regex validTokenRegex {"^[[:alpha:]\\.=&'\\-@]+$|^[[:digit:]:]+^"};
    boost::regex nonWordRegex {"[^[:alpha:]]"};
    boost::mt19937 generator {options.rand_seed};
    boost::random::uniform_int_distribution<> distribution;
    boost::random::uniform_int_distribution<uint64_t> distribution64;
    boost::mt19937_64 boost_generator64 {options.rand_seed};

    //term, <global_word_freq, global_word_index>
    map<string, Stats> file_statistics;
    unordered_set<string> stop_words { "a", "the", "in", "to", "i", "he", "she", "it" };
    unordered_set<string> amplified_words;

    // It may be preferable to use another random number generator in production, http://www.pcg-random.org/ lists
    // Mersenne Twister as having some failures for statistical quality.
    std::mt19937_64 std_generator64 {options.rand_seed};

    uint64_t processPaths(string target_path, bool (DocumentSet::*f) (const string & filepath));
    bool processFileGlobally(const string & filepath);
    bool processFileLocally(const string & filepath);

    string wordFromIndex(unsigned index);
    map<string, int> getUpdated(const string & filepath);
    vector<Document> documents;
    uint64_t total_count = 0;
    map<string, int> result;

    vector<tuple<double, double, double, double, string>> irisData;
    vector<tuple<double, double, double, double, double, double, double, double, double, double, double, double, double, double>> wineData;

    /**
     * Calculates the required number of responsive and non-responsive seed documents, and generates the index and cache files.
     */
    void initFiles();
    // Alternative source population, using the iris test dataset
    void initIris();
    void initIrisData();
    void initWine();
    void initWineData();
};

#endif //DOCUMENT_SET
