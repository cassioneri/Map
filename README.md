Map
===

The file <tt>map.h</tt> implements a container similar to <tt>std::map</tt>.

The main difference is that <tt>std::map</tt> (very likely) implements a
red-black trees which guarantees logarithmic complexity for search, insertion
and deletion.

The one implemented here uses a much simpler data structure, namely, a treap
which has expected logarithmic complexity for search, insertion and deletion
but in the worst case the complexity is linear.

In addition, this implementation uses operators <tt>&lt;</tt> and <tt>==</tt>
to compare keys. In contrast, <tt>std::map</tt> allows the usage of other
function objects (the default is <tt>std::less</tt>) for comparisons and it
does not use <tt>==</tt>. Indeed, for <tt>std::map</tt> equality (or more
correctly, equivalence) of <tt>x</tt> and <tt>y</tt> is expressed by

<pre>!comp(x, y) && !comp(y, x)</pre>

where <tt>comp</tt> is the comparison object. Notice that this implies two
calls to <tt>comp::operator()</tt> which can be wasteful.

The implementation is <b>incomplete</b> (I spent just two days on it) and is not
meant for production code. This is a simple exercise that I assigned to myself.

The file <tt>map.h</tt> is the implementation.
The file <tt>test.cpp</tt> is the unit test (based on the <tt>assert</tt> macro,
sorry!).

Compile the test with

<pre>g++ --std=c++11 test.cpp -o test -Wall -Wextra -pedantic</pre>

Todo
---

<ul>
<li>
Implement missing methods. The idea is providing almost the interface
of  <tt>std::map</tt>.
</li>
<li>
Add support for allocators.
</li>
</ul>

Copyright (C) 2014 Cassio Neri Moreira
</p><p>
<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.
</p>
