rm bin -r
mkdir bin
clang++ main.cpp -o bin/simd_trig -msse4.1 -O3
# Enable for assembly output
# clang++ main.cpp -o bin/simd_trig.asm -S -msse4.1 -O3
./bin/simd_trig
