

**ARTICLES**
* https://developers.redhat.com/blog/2016/03/01/reducing-memory-access-times-with-caches/
* https://www.akkadia.org/drepper/cpumemory.pdf
* https://developers.redhat.com/blog/2014/03/10/determining-whether-an-application-has-poor-cache-performance-2/


write-through/write-back/write-around caching
  https://shahriar.svbtle.com/Understanding-writethrough-writearound-and-writeback-caching-with-python
  https://courses.cs.washington.edu/courses/cse378/09wi/lectures/lec17.pdf
  
  **write hit** -  means the page on which you want to write is present in cache
  **write miss**  - page on which write to be performed is not in cache.
  
  * write hit policy 
    * write-through- data is written to the cache and the backing store location(main memory) at the same time
    * write-back - data is written to the cache and Then I/O completion is confirmed, the data is later written to main memory
    * Write-around - data is written only to the backing store(main memory) without writing to the cache
  * write miss policy
    * Write Allocate - the block is loaded on a write miss, followed by the write-hit action.
    * No Write Allocate - the block is modified in the main memory and not loaded into the cache.
    
 * Write Through with Write Allocate:
 on hits it writes to cache and main memory
 on misses it updates the block in main memory and brings the block to the cache
 Bringing the block to cache on a miss does not make a lot of sense in this combination because the next hit to this block will generate a write to main memory anyway (according to Write Through policy)

* Write Through with No Write Allocate:
 on hits it writes to cache and main memory;
 on misses it updates the block in main memory not bringing that block to the cache;
 Subsequent writes to the block will update main memory because Write Through policy is employed. So, some time is saved not bringing the block in the cache on a miss because it appears useless anyway.

* Write Back with Write Allocate:
 on hits it writes to cache setting �dirty� bit for the block, main memory is not updated;
 on misses it updates the block in main memory and brings the block to the cache;
 Subsequent writes to the same block, if the block originally caused a miss, will hit in the cache next time, setting dirty bit for the block. That will eliminate extra memory accesses and result in very efficient execution compared with Write Through with Write Allocate combination.

* Write Back with No Write Allocate:
 on hits it writes to cache setting �dirty� bit for the block, main memory is not updated;
 on misses it updates the block in main memory not bringing that block to the cache;
 Subsequent writes to the same block, if the block originally caused a miss, will generate misses all the way and result in very inefficient execution.
  
  http://web.cs.iastate.edu/~prabhu/Tutorial/CACHE/interac.html
  
  https://github.com/mariobecerra/ComputerArchitectureP2
  
  https://www.felixcloutier.com/x86/clwb
  
  https://github.com/riteshgajare/cache-simulator
  https://github.com/brian-murphy/cache-sim
  
**The read policies are**
 * Read Through -  reading a word from main memory to CPU
 *No Read Through - reading a block from main memory to cache and then from cache to CPU
  
  
