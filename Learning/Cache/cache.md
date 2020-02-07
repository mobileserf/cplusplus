

**ARTICLES**
* https://developers.redhat.com/blog/2016/03/01/reducing-memory-access-times-with-caches/
* https://www.akkadia.org/drepper/cpumemory.pdf
* https://developers.redhat.com/blog/2014/03/10/determining-whether-an-application-has-poor-cache-performance-2/


write-through/write-back/write-around caching
  https://shahriar.svbtle.com/Understanding-writethrough-writearound-and-writeback-caching-with-python
  https://courses.cs.washington.edu/courses/cse378/09wi/lectures/lec17.pdf
  * write-through- data is written to the cache and the backing store location(main memory) at the same time
  * write-back - data is written to the cache and Then I/O completion is confirmed, the data is later written to main memory
  * Write-around - data is written only to the backing store(main memory) without writing to the cache
  https://www.felixcloutier.com/x86/clwb
  
  https://github.com/riteshgajare/cache-simulator
  https://github.com/brian-murphy/cache-sim
  
  
