#ifndef __MSERF_COMMON_BUFREF_H__
#define __MSERF_COMMON_BUFREF_H__

namespace mserf {

template <typename Container>
class IterView {

 public:
  typedef typename Container::iterator iterator;
  typedef typename Container::const_iterator const_iterator;

  IterView(iterator start, iterator end): _begin(start), _end(end) {}
  IterView(iterator start, size_t distance): _begin(start), _end(start) {
     std::advance(_end, distance);
  }

  iterator begin()  const { return _begin ; }
  iterator end() const { return _end ; }

  const_iterator  const_begin()  const { return _begin ; }
  const_iterator  const_end() const { return _end ; }

  private:
   iterator _begin;
   iterator _end;
};

} //end namespace mserf

#endif //__MSERF_COMMON_BUFREF_H__
