RMDIR bin /s /q
MKDIR bin
clang++ main.cpp -o bin/simd_trig.exe -msse4.1 -O3
REM Enable for assembly output
REM clang++ main.cpp -o bin/simd_trig.asm -S -msse4.1
.\bin\simd_trig.exe
