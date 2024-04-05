#ifndef BASIC_TYPE
#define BASIC_TYPE

#include <string>
#include <cstdint>

enum VertexType { Input, Output, Const, Operation, Graph };

class BasicType {
  public:
    BasicType();

    BasicType &operator=(const BasicType &other) = default;
    BasicType &operator=(BasicType &&other) = default;
    BasicType(const BasicType &other) = default;
    BasicType(BasicType &&other) = default;

    virtual ~BasicType(); 

    virtual std::string toVerilog();
    VertexType getType();

    virtual std::string getName() const;
    uint32_t getLevel() const;

    void setName(std::string name);
    void setLevel(uint32_t level);

    virtual uint64_t getWireSize() const;
    virtual std::string getInstance();

  protected:
    VertexType type;
    uint32_t level = 0;
    std::string name;
};

#endif
