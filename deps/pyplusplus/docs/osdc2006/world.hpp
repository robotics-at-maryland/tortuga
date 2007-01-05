#ifndef __world_hpp__
#define __world_hpp__
#include <string>

struct world{
  world(const std::string& msg)
  : m_msg(msg) 
  {}
  
  void set(const std::string& msg)
  { m_msg = msg; }
  
  const std::string& greet() const
  { return m_msg; }

private:
    std::string m_msg;
};

#endif//__world_hpp__