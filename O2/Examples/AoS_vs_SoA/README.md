# Comparison of array-of-struct vs. struct-of-arrays for data analysis

## description
This example tries to estimate the performance difference between storing data in a SoA or a AoS method.

It does so by running an arbitrary analysis over generated data. This analysis can be summarized as:
```
sum = 0
foreach track:
 if (uint32_t)track.some_value > cutoff {
   r = sqrt(track.x*track.x+track.y*track.y+track.z*track.z);
   sum += track.pt - r;
 }
 return sum;
 ```
Here `some_value` is an arbitrary type to cut on, it is referred to as a `muonScore` in the code because it could be, for example, some precomputed value describing how likely it is that a given track describes a muon.

There are three implementations that are tested: SoA, AoS, and a vectorized SoA implementation. All versions support threading via OpenMP.

Different values for `cutoff` are tested and for each value the analysis is repeated 11 times in a row to obtain better results. The default size of the data is large enough that caching should have no effect on the results.

## requirements
The vectorized version uses AVX2 instructions, therefore it is required to have a processor capable of executing these instructions (Intel Haswell or later, or the equivalent AMD).

The program allocates a significant amount of memory so at least 8 GiB of ram are recommended. If this is not available the memory used can be reduced by tweaking the variable `count` inside the `main` function.

**note:** if you reduce `count` too much, the data will still be stored in the cache between iterations. If this is not desired there is a function `flush_cache()` which can be used in between iterations to remove the data from the cache. This function does increase the run-time by a significant amount.

## running
In order to build the examples run
```
  clang++ -fopenmp --std=c++11 -O3 -march=native src/*.cxx main.cxx -o comparison
```
or replace `clang++` by `g++`.
For the best measurement results it is recommended, but not necessary, to run the binary in real-time mode to reduce any influence from other processes. On Linux this can be done by running:
```
sudo chrt 99 ./comparison
```
**Note:** this will cause all other programs to almost certainly freeze up for the duration of the comparison. This is expected behavior. This should only take a few seconds.

 Output is of the form `probability minimum median maximum bandwidth result` for each version, where: `probability` is the odds of a track passing the initial filter, `result` is the computed sum (included only to prevent optimizations from removing it), `minimum` is the shortest time taken of all iterations (in ns), `median` is the median time, `maximum` is the maximum time, `bandwidth` is the predicted effective bandwidth in GB/s (4 bytes for each track that does not pass the filter and sizeof(Track) bytes for each track that does, divided by the `minimum` time).

## plotting
There is a provided Gnuplot script for making a plot of the effective bandwidth (as shown in media/bandwidth.png). To use it
save the results of the binary to a file, for example `data.dat`. Then run
```
gnuplot -e "data='data.dat'" make_plot.gnuplot
```
And the script will produce a png called `bandwidth.png`.

## expected results
The following results were obtained on an i7-7700HQ based system with a theoretical max bandwidth of 34.1 GB/s (but ~30 is in line with benchmarks).
![](media/bandwidth.png "Measured effective bandwidth")
The dip near the start can be explained by the access pattern, where a lot of data gets pulled in from other cache-lines when a track passes the filter, but are not likely to pass the filter themselves resulting in wasted bandwidth.

 Performance peaks at both extremes, in the case of the probability going to one this is due to the fact that all data loaded is used and is loaded in linear fashion.
In the case of the probablity going to zero most tracks will fail the filter and in the SoA case this means the program only scans a single flat-array. In the AoS the filter parameter is not packed together and thus part of the loaded data (from the same cache-line) is wasted bytes. (the difference matches the expected results: a flat track in this example is 20 bytes, 4 of which are the filter values therefore we expect a difference on the order of 5 times).
