Black-Hole Clustering Algorithm
===============================

This implements the Black-Hole clustering algorithm (DOI-link: http://dx.doi.org/10.1016/j.ins.2012.08.023).

```shell
Clustering proof of concept.

Allowed arguments:
    -h [ --help ]                  Produce this help message.
    -c [ --centroids ] arg (=4)    Set the number of centroids to use.
    -a [ --stars ] arg (=10)       Set the number of stars to use.
    -i [ --iterations ] arg (=100) Set the number of iterations to run.
    -m [ --random-seed ]           Set the random number seed to use.
    -p [ --path ] arg              Directory containing, or path of file listing
                                the paths, of documents to cluster.
    -s [ --iris ]                  Use the iris data set.
    -w [ --wine ]                  Use the wine data set.
    -v [ --verbose ]               Verbose output (including file-names and
                                times).
    -q [ --quiet ]                 Quiet mode, only outputting basic statistics.
```

The iris and wine options will use black-hole to cluster the standard wine and iris test data sets (with each dimension weighted equally).

The more interesting option is to use black-hole to cluster a directory of documents. The algorithm will use the porter-stemming algorithm (https://tartarus.org/martin/PorterStemmer/) and TF-IDF (https://en.wikipedia.org/wiki/Tf%E2%80%93idf) to convert the documents to vectors, and then uses black-hole clustering to cluster the documents by using these vectors.

The number of centroids, stars, and iterations paramaters of the algorithm may be set, as well as the random number seed used.

Build instructions
------------------

This requires clang (v7.3.0 or newer is recommended) or gcc (untested, but v5 or newer should likely work) with c++11 support with the C++ Boost libraries (v1.60 or newer is recommended).

```shell
cd src/
make
./black-hole-clustering
```
