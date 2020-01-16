#ifndef __MSERF_TOOL_PERF_STATS_H__
#define __MSERF_TOOL_PERF_STATS_H__
// https://github.com/WojciechMula/toys/blob/master/000helpers/linux-perf-events.h
#pragma once
#ifdef __linux__

#include <asm/unistd.h>       // for __NR_perf_event_open
#include <linux/perf_event.h> // for perf event constants
#include <sys/ioctl.h>        // for ioctl
#include <unistd.h>           // for syscall

#include <cerrno>  // for errno
#include <cstring> // for memset
#include <stdexcept>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <functional>

struct PerfEvent {
   PerfEvent(int32_t event, const char* name):_eventId(event), _iocId(0), _result(0) {
     strncpy(_eventName, name, 63);
     _eventName[63] = '\0';
   }
   PerfEvent(const PerfEvent& other) {
      memcpy(this, &other, sizeof(PerfEvent));
   }
   PerfEvent& operator = (const PerfEvent& other) {
      memcpy(this, &other, sizeof(PerfEvent));
      return *this;
   }
   void printDetails() const
   {
      std::cout<<_eventId<<" ("<<_iocId<<") " << _eventName<<" : "<< _result<<std::endl;
   }
   int32_t _eventId; /*enum value of perf_hw_id, perf_hw_cache_id etc*/
   uint64_t _iocId; /* perf ioc id */
   uint64_t _result;
   char _eventName[64];

};

#define ADD_EVENT(objPtr, id) \
     objPtr->addEvent(PerfEvent(id, #id));

template <int TYPE = PERF_TYPE_HARDWARE> class LinuxEvents {
  int fd;
  mutable bool working;
  std::vector<PerfEvent> _perfEvents;
  std::chrono::high_resolution_clock::time_point _startTime;
  double _nanoseconds;

public:
  void addEvent(const PerfEvent& event) {
   if(!fd)
      _perfEvents.push_back(event);
  }
  LinuxEvents(bool defaultEvents = true)
    : fd(0)
    ,working(true)
  {

    _startTime = std::chrono::high_resolution_clock::now();
    if(defaultEvents) {
      ADD_EVENT(this, PERF_COUNT_HW_CPU_CYCLES)
      ADD_EVENT(this, PERF_COUNT_HW_INSTRUCTIONS)
      ADD_EVENT(this, PERF_COUNT_HW_BRANCH_MISSES)
      //ADD_EVENT(this, PERF_COUNT_HW_CACHE_REFERENCES)
      //ADD_EVENT(this, PERF_COUNT_HW_CACHE_MISSES)
   }
    init();
  }

  void init() {
    working = true; //lets restart working
    if(fd) return;
    perf_event_attr attribs;
    memset(&attribs, 0, sizeof(attribs));
    attribs.type = TYPE;
    attribs.size = sizeof(attribs);
    attribs.disabled = 1;
    attribs.exclude_kernel = 1;
    attribs.exclude_hv = 1;

    attribs.sample_period = 0;
    attribs.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
    const int pid = 0;  // the current process
    const int cpu = -1; // all CPUs
    const unsigned long flags = 0;

    int group = -1; // no group
    for (auto& config : _perfEvents) {
      attribs.config = config._eventId;
      fd = syscall(__NR_perf_event_open, &attribs, pid, cpu, group, flags);
      if (fd == -1) {
        report_error("perf_event_open");
      }
      ioctl(fd, PERF_EVENT_IOC_ID, &config._iocId);
      if (group == -1) {
        group = fd;
      }
    }
  }
  ~LinuxEvents() { close(fd); }

  inline void start() {
    init();
    if (ioctl(fd, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP) == -1) {
      report_error("ioctl(PERF_EVENT_IOC_RESET)");
    }

    if (ioctl(fd, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP) == -1) {
      report_error("ioctl(PERF_EVENT_IOC_ENABLE)");
    }
    _startTime = std::chrono::high_resolution_clock::now();
  }

  inline void end() {
    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    _nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>( endTime - _startTime ).count();

    std::vector<uint64_t> temp_result_vec;
    temp_result_vec.resize(_perfEvents.size() * 2 + 1);
    if (ioctl(fd, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP) == -1) {
      report_error("ioctl(PERF_EVENT_IOC_DISABLE)");
    }

    if (read(fd, temp_result_vec.data(), temp_result_vec.size() * 8) == -1) {
      report_error("read");
    }
    // our actual results are in slots 1,3,5, ... of this structure
    // we really should be checking our ids obtained earlier to be safe
    auto event = _perfEvents.begin();
    for (uint32_t i = 1 ; i < temp_result_vec.size(); i += 2, ++event) {
      event->_result = temp_result_vec[i];
    }
#if 0
    for (uint32_t i = 0 ; i < temp_result_vec.size(); ++i) {
      std::cout<<i<<":"<< temp_result_vec[i]<<std::endl;
    }
#endif
  }
  double timeDiff() const { return _nanoseconds;}
  size_t numberOfEvents() const { return _perfEvents.size();}

  std::vector<PerfEvent> perfEvents() const {
    return _perfEvents;
  }

  uint64_t getValueForEvent(int32_t eventId) const {
    for(auto& event: _perfEvents) {if(eventId == event._eventId) return event._result;}
    report_error("unknown value");
    return 0;
  }
  std::vector<uint64_t> getEventValues() const {
    std::vector<uint64_t> tmp;
    tmp.reserve(_perfEvents.size());
    for(auto& event: _perfEvents) {tmp.push_back(event._result);}
    std::cout<<"TMP: "<<tmp.size()<<std::endl;
    return tmp;
  }

  void print() const {
    for(auto& event: _perfEvents) {event.printDetails(); }
  }
  double cyclesPerByte(size_t bytes) const {
    return getValueForEvent(PERF_COUNT_HW_CPU_CYCLES)/(bytes * 1.0); //cycles/byte
  }

  double cyclesPerIteration(size_t iter) const {
    return ((getValueForEvent(PERF_COUNT_HW_CPU_CYCLES)*1.0)/iter); //cycles/iteration
  }
  double instructionsPerIteration(size_t iter) const  {
    return ((getValueForEvent(PERF_COUNT_HW_INSTRUCTIONS)*1.0)/iter); //instructions/iteration
  }
  double branchMissesPerIteration(size_t iter) const  {
    return ((getValueForEvent(PERF_COUNT_HW_BRANCH_MISSES)*1.0)/iter); //branchmisses/iteration
  }
  double MBPerSecond(size_t bytes)const  {
     double volume = bytes / (1024. * 1024);
     return volume * 1000000000/ timeDiff(); //MB/S
  }
  void printDefault(const char* name, size_t bytes, size_t iter) const {
    uint64_t cpuCycles =  getValueForEvent(PERF_COUNT_HW_CPU_CYCLES);
    uint64_t instructions =  getValueForEvent(PERF_COUNT_HW_INSTRUCTIONS);
    uint64_t branchMiss =  getValueForEvent(PERF_COUNT_HW_BRANCH_MISSES);
    std::cout<<"name: "<<name<<" bytes: "<<bytes<<" iteration: "<<iter<<std::endl;
    std::cout<<"\t Cycles Per Iteration      : "<<(cpuCycles*1.0/iter)<<std::endl;
    std::cout<<"\t Instruction Per Iteration : "<<(instructions*1.0/iter)<<std::endl;
    std::cout<<"\t Branch Miss Per Iteration : "<<(branchMiss*1.0/iter)<<std::endl;
    std::cout<<"\t MB/s                      : "<<(((bytes / (1024. * 1024)*1000000000))/timeDiff())<<std::endl;
    std::cout<<"\t Cycles Per Byte           : "<<(cpuCycles/(bytes *1.0))<<std::endl;
    std::cout<<std::endl;

  }

    //uint64_t cpuCycles =  unified.getValueForEvent(PERF_COUNT_HW_CPU_CYCLES);
    //uint64_t instructions =  unified.getValueForEvent(PERF_COUNT_HW_INSTRUCTIONS);
    //uint64_t branchMiss =  unified.getValueForEvent(PERF_COUNT_HW_BRANCH_MISSES);
private:
  void report_error(const std::string &context) const {
    if (working)
      std::cerr << (context + ": " + std::string(strerror(errno))) << std::endl;
    working = false;
  }
};

std::vector<uint64_t>
compute_mins(const std::vector<std::vector<uint64_t>>& allresults) {
  //std::cout<<"inside compute_mins"<<std::endl;
  if (allresults.size() == 0)
    return std::vector<uint64_t>();

  std::vector<uint64_t> answer = allresults[0];

  for (size_t k = 1; k < allresults.size(); k++) {
    for (size_t z = 0; z < answer.size(); z++) {
      if (allresults[k][z] < answer[z])
        answer[z] = allresults[k][z];
    }
  }
  return answer;
}

std::vector<uint64_t>
compute_averages(const std::vector<std::vector<uint64_t>>& allresults) {
  //std::cout<<"inside compute_averages"<<std::endl;
  if (allresults.size() == 0)
    return std::vector<uint64_t>();

  std::vector<uint64_t> answer(allresults[0].size());
  for (size_t k = 0; k < allresults.size(); k++) {
    for (size_t z = 0; z < answer.size(); z++) {
      answer[z] += allresults[k][z];
    }
  }

  for (size_t z = 0; z < answer.size(); z++) {
    answer[z] /= allresults.size();
  }
  return answer;
}

enum {
   PERF_ITER_AVERAGE = 0,
   PERF_ITER_MIN = 1,
   PERF_MAX_ITER_STATS
};

struct PerfIterStats {
   int32_t _id;
   const char* _name;
   std::function< std::vector<uint64_t>(const std::vector<std::vector<uint64_t>>& allresults)> _cb;
};
PerfIterStats perfIterStats[PERF_MAX_ITER_STATS+1] = {
  {PERF_ITER_AVERAGE, "AVG", compute_averages},
  {PERF_ITER_MIN, "MIN", compute_mins},
  {PERF_MAX_ITER_STATS, "NULL", nullptr},
};

template <int TYPE = PERF_TYPE_HARDWARE> class LinuxEventsStats {
public:
  LinuxEventsStats(size_t numberOfIter = 1)
  :_count(0)
  {
    _allresults.reserve(numberOfIter);
  }
  void addResult(const LinuxEvents<TYPE>& events)
  {
    if(!_perfEvents.size()) {
      _perfEvents = events.perfEvents();
    }
     //std::vector<uint64_t> tmp = events.getEventValues() ;
    // std::cout<<_count<<" "<<tmp.size()<<std::endl;
    _allresults.push_back(events.getEventValues());
   // std::cout<<_count<<" "<<_allresults[_count].size()<<std::endl;
    ++_count;

  }
  void printStats() const {
    std::vector<std::vector<uint64_t>>  stats;
    stats.resize(PERF_MAX_ITER_STATS);
    std::cout<<"NUMBER OF ITERATION: "<<_count<<std::endl;
    std::cout<<"NAME\t\t\t\t";
    for(int i = 0; i<PERF_MAX_ITER_STATS; ++i) {
      stats[i] = perfIterStats[i]._cb(_allresults);
      std::cout<<std::setw(15)<<perfIterStats[i]._name;
      //for(int j = 1; j < stats[i].size(); ++j) std::cout <<stats[i][j]<<"  ";
      //std::cout<<"\t"<< stats[i].size();
    }
    std::cout<<std::endl;
    std::cout<<std::setw(15);
    uint32_t i = 0;
    for(auto& events: _perfEvents) {
     std::cout<<events._eventName<<"\t";
     for(int j = 0; j<PERF_MAX_ITER_STATS; ++j)
     {
       //std::vector<uint64_t>& tmp = stats[j];
       std::cout<<std::setw(15)<<stats[j][i];
     }
     std::cout<<std::endl;
     ++i;
    }
  }
private:
  size_t _count;
  std::vector<std::vector<uint64_t>> _allresults;
  std::vector<PerfEvent> _perfEvents;
};

#endif
#endif// __MSERF_TOOL_PERF_STATS_H__
