# <strong><em>Sicario</em></strong>

The code for the move prcomputation and generation is absolutely janked. When I first started, it was my first time
coding in C++ and doing an engine so it was done in an absolutely convoluted and confusing way. But... it works... and
works quite well. Plus, the time I absolutely wasted on micro-memory optimization means it uses very little memory for
the move precomputation (at the price of clarity, maintenance, my time and mental sanity), so...
</br>
</br>
<h2>Bugs/Problems</h2>
<ul>
    <li>Currently, the history array is initialised to hold 500 entries and is increased to 1000 when needed. It is
    theoretically possible that the number of moves will exceed even 1000, which would cause a segfault or stack
    smashing.</li>
</ul>
</br>
<H2>To dos</H2>

---
<ul>
    <li>In some terminal fonts, the name and logo does not render correctly.</li>
    <li>Need to try detect the terminal type and font type ideally.</li>
    <li>Credit the ascii art.</li>
    <li>The PV table may have collisions due to different positions having the same hash. Very unlikely but possible.
    May need some kind of check</li>
    <li>Need to separate declaration and definition of MoveList things.</li>
</ul>
</br>
<H2>Acknowledgements</H2>

---

<strong>www.chessprogramming.org</strong> - For being my primary source of information and knowledge.</br>
<strong>Stockfish</strong> - For inspiration and guidance.</br>
<strong>Bluefeversoft</strong> - For your YouTube tutorials and explanations for concepts I found difficult to grasp.</br>
There are likely others that I have missed.</br>
</br>
And to the one who (to quote Kodaline's <em>All I Want</em>) - <em>"...brought out the best in me, a part of me I'd never seen..."</em>
