#include "document_set.h"

DocumentSet::DocumentSet(const Options & options)
{
    this->options = options;
    if (!options.path.empty()) {
        initFiles();
    } else if (options.iris) { //these are in document_set_data.cpp
        initIris();
    } else if (options.wine) {
        initWine();
    } else {
        cout<< "No documents available." << endl;
        exit(-1);
    }
}

void DocumentSet::initFiles()
{
    total_count = processPaths(options.path, &DocumentSet::processFileGlobally);

    //rewrite file_statistics to remove all rare terms (ones that aren't meaningful for clustering).
    map<string, Stats> min_stats;
    unsigned counter = 0;

    multiset<unsigned> sorted_set;

    unsigned stop = (unsigned)((double)file_statistics.size() / 100.0 * 0.1);
    for (auto & stat : file_statistics) {
        if (sorted_set.size() < stop) {
            sorted_set.insert(stat.second.global_word_freq);
        } else if (stat.second.global_word_freq > *sorted_set.begin()) {
            sorted_set.erase(sorted_set.begin());
            sorted_set.insert(stat.second.global_word_freq);
        }
        /*if (stat.second.global_word_freq > 3000) {
            cout << stat.first << " " << stat.second.global_word_freq << endl;
        }*/
    }

    for (auto & stat : file_statistics) {
    	if (stat.second.global_word_freq > 1 && stat.second.global_word_freq < *sorted_set.begin()) {
            min_stats[stat.first] = Stats {stat.second.global_word_freq, counter++};
        }
    }
    file_statistics = min_stats;

    Dimension = file_statistics.size(); //global variables from global.h
    MaxDimensions.resize(Dimension);
    total_count = processPaths(options.path, &DocumentSet::processFileLocally);
    cout<< endl;

    if (total_count == 0) {
        cout << "No valid documents exist. Exiting." << endl;
        exit(-1);
    } else if (total_count < options.centroid_count) {
        cout << "Fewer documents ("
             << total_count << ") than specified number of centroids ("
             << options.centroid_count << ") found. Exiting." << endl;
        exit(-1);
    }

    if (options.verbose) {
        cout << "Successfully processed " << total_count << " files" << endl;
    }
}

uint64_t DocumentSet::processPaths(string target_path, bool (DocumentSet::*f) (const string & filepath))
{
    uint64_t success_count = 0;
    uint64_t total_count = 0;
    if (is_directory(target_path)) {
        path targetDir(target_path);
        recursive_directory_iterator iter(targetDir), end;
        while (iter != end) {
            if (is_regular_file(iter->path())) {
                if ((this->*f)(iter->path().string())) {
                    success_count++;
                }
                total_count++;
            }
            ++iter;
        }
    } else if (is_regular_file(target_path)) {
        std::ifstream fin(target_path);
        string line;

        while(getline(fin, line)) {
            trim_right(line);
            if (line.length() > 0) {
                if ((this->*f)(line)) {
                    success_count++;
                }
                total_count++;
            }
        }
    } else {
        throw string("Invalid file type");
    }
    if (total_count > success_count) {
        cout << "Unable to process " << (total_count - success_count) << " files." << endl;
    }
    return success_count;
}

bool DocumentSet::processFileGlobally(const string & filepath)
{
    static int file_count = 0;
    if (options.verbose && options.have_stdout) {
        cout << "\r" << "Processing file globally #" << (file_count + 1) << " " << filepath;
    }
	file_count++;
	map<string, int> result = getUpdated(filepath);
	static unsigned term_counter = 0;
	for (auto & stats : result) {
		if (file_statistics.find(stats.first) == file_statistics.end()) {
			file_statistics[stats.first] = Stats {1, term_counter++};
		} else {
			file_statistics[stats.first].global_word_freq += 1;//stats.second if not doing per file level...
		}
	}
    return true;
}

bool DocumentSet::processFileLocally(const string & filepath)
{
    static int file_count = 0;
    if (options.verbose && options.have_stdout) {
        cout << "\r" << "Processing file locally #" << (file_count + 1) << " " << filepath;
    }
	file_count++;
	result = getUpdated(filepath);
	int wc = 0;
	for (auto & stats : result) {
		wc += stats.second;
	}
	vector<double> weights(Dimension);
	for (auto & stats : result) {
		auto it = file_statistics.find(stats.first);
		if (it != file_statistics.end()) {
			int global_word_freq = it->second.global_word_freq;
			int global_word_index = it->second.global_word_index;

			//stats.second is the files's term-frequency of the stats.first term

			//try: idf(t) = 1 + log(numDocs / (docFreq + 1))
			double tf = (double)stats.second / (double)wc;
			double idf = 1 + log((double)file_count / ((double)global_word_freq + 1.0));
			double weighting = tf * idf;
			weights[global_word_index] = weighting;// / (double)result.size();
			MaxDimensions[global_word_index] = max(MaxDimensions[global_word_index], weights[global_word_index]);
		}
	}
	documents.push_back(Document { filepath, weights });
    return true;
}

map<string, int> DocumentSet::getUpdated(const string & filepath)
{
    std::ifstream ifs(filepath, ios::in | ios::binary | ios::ate);
    uint64_t sz = static_cast<uint64_t>(ifs.tellg());//read whole file for now... TODO: limit this later on
    ifs.seekg(0, ios::beg);
    vector<char> bytes(sz);
    ifs.read(&bytes[0], sz);
    ifs.close();

    //hack because all my test Enron docs have a boilerplate disclaimer
    string updated = boost::regex_replace(string(&bytes[0], sz), enronRegex, [] (const smatch & m) -> string { return ""; });

    vector<string> result;
    boost::algorithm::split_regex(result, updated, tfidfRegex);

    map<string, int> processed;
    for (string & line : result) {
        string tmp = boost::regex_replace(line, outerPunctRegex, [] (const smatch & m) -> string { return ""; });
        if (tmp.length() > 0) {
            std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
            if (boost::regex_match(tmp, validTokenRegex)) {
                Porter2Stemmer::stem(tmp);
                if (stop_words.find(tmp) == stop_words.end() && tmp.size() > 1) {
                    if (processed.find(tmp) == processed.end()) {
                        processed[tmp] = 1;
                    } else {
                        processed[tmp] += 1;
                    }
                }
            } else {
                vector<string> split_words;
                boost::algorithm::split_regex(split_words, tmp, nonWordRegex);
                for (string & split_line : split_words) {
                    Porter2Stemmer::stem(split_line);
                    if (stop_words.find(split_line) == stop_words.end() && split_line.size() > 1) {
                        if (processed.find(split_line) == processed.end()) {
                            processed[split_line] = 1;
                        } else {
                            processed[split_line] += 1;
                        }
                    }
                }
            }
        }
    }
    return processed;
}

string DocumentSet::wordFromIndex(unsigned index)
{
	for (auto & stats : file_statistics) {
		if (stats.second.global_word_index == index) {
			return stats.first;
		}
	}
	return "NO WORD FOUND!";
}
