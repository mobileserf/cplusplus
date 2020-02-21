g++ sse_memmove.c  sse_memcpy.cpp sorted_array_index.cpp -O3 -mavx2 -mfma -std=c++14 -I.
g++ avl_array.cpp -O3 -mavx2 -mfma -std=c++14 -I.

Test 10240 unique keys, approximatly 10240 operations per array => 105 Million queries

avl_array.cpp (64 bit key, 16 bit index)
Array_size 512 -  0m39.469s (78M) => 2.7 million queries/s
Array_size 1024 - 1m4.337s (153M)
Array_size 2048 - 1m36.945s (312M)

avl_array_sort.cpp  SIMD (64 bit key, 64bit index)
Array_size 512 -  0m34.631s (93M) => 3.02 million queries/s
Array_size 1024 - 0m57.190s (183M)
Array_size 2048 - 1m39.804s (363M)

avl_array_sort.cpp  without SIMD
Array_size 512 -  0m40.365s (93M)
Array_size 1024 - 1m0.765s (183M)
Array_size 2048 - 1m36.111s (363M)

avl_array_sort.cpp  SIMD (32 bit key, 32bit index)
Array_size 512 -  0m21.249s (63M)  => 4.9 million queries/s
Array_size 1024 - 0m37.278s (123M)
Array_size 2048 - 0m51.339s (243M)

avl_array_sort.cpp  without SIMD
Array_size 512 -  0m21.486s (63M)
Array_size 1024 - 0m37.500s (123M)
Array_size 2048 - 1m2.773s (243M)

avl_array_sort.cpp  SIMD (16 bit key, 16bit index)
Array_size 512 -  0m15.546s (33M) => 6.7 million queries/S
Array_size 1024 -  0m21.377s (63M)
Array_size 2048 - 0m33.015s (123M)

avl_array_sort.cpp  without SIMD
Array_size 512 -  0m16.893s (33M)  => 6.2 million queries/s
Array_size 1024 - 0m25.044s (63M)
Array_size 2048 - 0m41.050s (123M)

credit avx-array - https://github.com/mpaland/avl_array
       SIMD - https://github.com/KNNSpeed/AVX-Memmove
