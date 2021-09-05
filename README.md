# cryptopals
My solutions to Cryptopals 

I am working through the Cryptopals challenges, from cryptopals.com, 
in C. The header file cryptopals.h contains a collection of utility 
fuctions written during the first set of challenges which are helpful 
later. Some of my code uses the OpenSSL library, so compiling my code 
requires installation of OpenSSL and running gcc with -lssl and 
-lcrypto. One of my goals is to leverage concurrency and get 
experience with pthreads (https://hpc-tutorials.llnl.gov/posix/) 
wherever possible in writing these solutions, even if my usage might 
be a bit contrived. Compile with -pthread as appropriate.
