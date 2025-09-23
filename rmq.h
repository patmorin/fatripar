#ifndef __RMQ_H
#define __RMQ_H

#include<vector>

class rmq_structure {
protected:
  std::vector<int> const &data;

public:
  rmq_structure() { }
  void init(const std::vector<int>& _data) { data = _data; };
};

#endif //__RMQ_H
