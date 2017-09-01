#ifndef DOCUMENT
#define DOCUMENT

#include "global.h"

#include <math.h>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct Document {
public:
    string path;
    vector<double> weights;

    Document(string path, vector<double> weights) : path(path), weights(weights) {}

    // using this allows easily changing to another distance metric
    double documentDistance(const vector<double> & v) const {
        return euclideanDistance(v);
    }

    double documentDistance(const Document & d) const {
        return documentDistance(d.weights);
    }

    friend std::ostream & operator<< (std::ostream & os, const Document & d) {
        os << " [dimensions: " << d.weights.size() << "] " << string("path: ") << d.path;// << endl;
        /*for (unsigned long i = 0; i < d.weights.size(); i++) {
            if (d.weights[i]) {
                os << i << ":" << d.weights[i] <<  " ";
            }
        }
        os << endl;*/
        return os;
    }

private:
    double euclideanDistance(const vector<double> & v) const {
        double sum = 0.0;
        for (int i = 0, stop = weights.size(); i < stop; i++) {
            sum += pow(weights[i] - v[i], 2.0);
        }
        return sqrt(sum / Dimension);
    }

    double euclideanDistance(const Document & d) const {
        return euclideanDistance(d.weights);
    }

    double cosineDistance(const vector<double> & v) const {
        double dot_product = 0.0;
        double v_dot_product = 0.0;
        double w_dot_product = 0.0;

        for (int i = 0, stop = weights.size(); i < stop; i++) {
            dot_product += weights[i] * v[i];
            v_dot_product += v[i] * v[i];
            w_dot_product += weights[i] * weights[i];
        }
        return dot_product / (sqrt(v_dot_product) * sqrt(w_dot_product));
    }

    double cosineDistance(const Document & d) const {
        return cosineDistance(d.weights);
    }
};

#endif //DOCUMENT
