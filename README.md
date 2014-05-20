Map
===

The file map.h implements an container similar to std::map.

The main difference is that, std::map uses (very likely) a complex data
structure called red-black tree which guarantees logarithmic complexity for
insertion and searching.

The map implemented here uses a much simpler data structure, namely, a binary
search tree which has logarithmic complexity on average for insertion and
searching but in the worst case the complexity is linear.

The implementation is incomplete and is not meant for production code. This is
a simple exercise that I assigned to myself.

The file map.h is the implementation.
The file test.cpp is the unit test (based on the "assert" macro, sorry!).

Compile it with

g++ --std=c++11 test.cpp -o test -Wall -Wextra -pedantic

Copyright (C) 2011-2014 Cassio Neri Moreira
</p><p>
<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.
</p>
