Map
===

The file <tt>map.h</tt> implements a container similar to std::map.

The main difference is that <tt>std::map</tt> uses (very likely) a complex data
structure called red-black tree which guarantees logarithmic complexity for
insertion and searching.

The one implemented here uses a much simpler data structure, namely, a binary
search tree which has logarithmic complexity <b>on average</b> for insertion and
searching but in the worst case the complexity is linear.

The implementation is incomplete and is not meant for production code. This is
a simple exercise that I assigned to myself.

The file <tt>map.h</tt> is the implementation.
The file <tt>test.cpp</tt> is the unit test (based on the "assert" macro, sorry!).

Compile it with

<pre>g++ --std=c++11 test.cpp -o test -Wall -Wextra -pedantic</pre>

Copyright (C) 2014 Cassio Neri Moreira
</p><p>
<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.
</p>
