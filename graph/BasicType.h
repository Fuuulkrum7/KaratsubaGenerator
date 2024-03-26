#ifndef BASIC_TYPE
#define BASIC_TYPE

#include <string>

enum VertexType { Input, Output, Const, Operation, Graph };

class BasicType {
  public:
    virtual std::string toVerilog() = 0;
    virtual VertexType getType() { return type; }

    virtual std::string getName() = 0;
    virtual u_int32_t getLevel() { return level; }

  protected:
    VertexType type;
    u_int32_t level = 0;
    std::string name;
};

#endif
