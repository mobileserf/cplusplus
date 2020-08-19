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
  IterView(const IterView& view) = default;
  IterView& operator=(const IterView& view) = default;

  size_t distance() const { return std::distance(_begin, _end );}
  size_t size() const { return distance(); }

  iterator begin()  const { return _begin ; }
  iterator end() const { return _end ; }

  const_iterator  const_begin()  const { return _begin ; }
  const_iterator  const_end() const { return _end ; }

  private:
   iterator _begin;
   iterator _end;
};

 template <typename Container>
class ClassIter {

 public:
  typedef typename Container::iterator iterator;
  typedef typename Container::const_iterator const_iterator;

  ClassIter() { _it = _end;}
  ClassIter(iterator start, iterator end): _it(start), _end(end) {}
  ClassIter(iterator start, size_t distance): _it(start), _end(start) {
     std::advance(_end, distance);
  }
  ClassIter(const IterView<Container>& view): _it(view.begin()), _end(view.end()) {}
  void init(iterator start, iterator end)  { _it  = start; _end = end;}
  bool is_next() { return (_it != _end)? true: false; }
  iterator next() { return _it++; }

  private:
   iterator _it;
   iterator _end;
};

} //end namespace mserf

#endif //__MSERF_COMMON_BUFREF_H__
 
