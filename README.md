
Implementations  of SIMD trig (sin/cos) functions for games.

Focus is on quick computation without too much error.



# Accuracy
Do not use this for anything numerical really.

Nevertheless samples.csv compares my implementation with the 'cmath' implementation of trigonometric functions.

# SIMD
- ``sinSlow`` - 1 wide   sine, branching
- ``cosSlow`` - 1 wide cosine, branching
- ``sinSSE4_1`` - 4 wide   sine, branchless
- ``cosSSE4_1`` - 4 wide cosine, branchless
