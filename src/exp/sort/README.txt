https://dirtyhandscoding.github.io/

Linear Search - https://github.com/stgatilov/linear-vs-binary-search

      https://dirtyhandscoding.wordpress.com/2017/08/25/performance-comparison-linear-search-vs-binary-search/
      
  compile: g++ linear_search.cpp  -O2 --std=c++11 -mavx2
  
  result:  winners 64 byte (6.6 ns) - linear_search_avx_UR<SIZE> linear_search_sse_UR<SIZE> hybridX_search
  
           for big array use binary_search_branchless or hybridX_search size >64. 1024 took only 9.7 ns

merge - https://gist.github.com/stgatilov/ac832c6dd30e695e566f42b1ef8ef45a

        https://dirtyhandscoding.github.io/posts/vectorizing-stdmerge-with-vpermd-from-avx2-and-lookup-table.html
        
        compile: g++ merge.cpp  -O2 --std=c++11 -mavx2
        
        result: total keys > 1024, then use Merge_Simd_KeysOnly else std::merge
        

sort - https://github.com/stgatilov/position-counting-sort/blob/master/sort.cpp

       https://dirtyhandscoding.github.io/posts/vectorizing-small-fixed-size-sort.html
       
        compile: g++ sort.cpp  -O2 --std=c++11 -mavx2
        
        result: key - int32, value int32. sort array size 32
        
      const bool WithValues = false; /* no value is sorted
      
         114.4 ns : PCSort_Trans:any                           (104505344)
         
         114.4 ns : PCSort_Main:distinct                       (1266135040)
         
         114.4 ns : PCSort_WideOuter:distinct                  (1266135040)
         
         

      const bool WithValues = true; /* no value is sorted
      
           133.5 ns : PCSort_Trans:any                           (104505344)
           
           133.5 ns : PCSort_Main:distinct                       (1266135040)
           
           124.0 ns : PCSort_WideOuter:distinct                  (1266135040)
           
