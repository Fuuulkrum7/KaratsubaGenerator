#ifndef GRAPH_VERTEX
#define GRAPH_VERTEX

#include <memory>
#include <string>
#include <vector>

#include "BasicType.h"

#define VertexPtr std::shared_ptr<BasicType>

enum OperationType {
    Default,
    Not,
    Buf,
    And,
    Or,
    Nand,
    Nor,
    Xor,
    Xnor,
    Sum,
    Dif,
    LShift,
    RShift,
    SliceOper
};

namespace VertexUtils {
std::string operationToString(OperationType type);

std::string vertexTypeToString(VertexType type);
} // namespace VertexUtils

// When we call toVerilog, we want to get string
class GraphVertex : public BasicType {
  public:
    // type of vertex, operation, provided by it,
    // is it [a:b] vertex, values b and a
    GraphVertex(VertexType type,
                OperationType operation = OperationType::Default,
                uint64_t upper = 0, uint64_t lower = 0, std::string name = "");
    
    GraphVertex(GraphVertex *other);

    GraphVertex &operator=(const GraphVertex &other) = default;
    GraphVertex &operator=(GraphVertex &&other) = default;
    GraphVertex(const GraphVertex &other) = default;
    GraphVertex(GraphVertex &&other) = default;

    static uint_fast64_t getCountVertex();
    static void setCountVertex(uint_fast64_t n);

    std::string getTypeName() const;

    void addParent(VertexPtr vertex);
    void addChild(VertexPtr vertex);

    std::string getName() const;

    void setOperation(OperationType operation);
    OperationType getOperation() const;

    std::vector<VertexPtr> getOutConnections() const;
    std::vector<VertexPtr> getInConnections() const;

    uint64_t getWireSize() const;
    std::string getInstance();

    uint64_t getLower() const;
    uint64_t getUpper() const;

    virtual std::string toVerilog() override;

  protected:
    OperationType operation;

    std::vector<VertexPtr> inConnection;
    std::vector<VertexPtr> outConnection;

    std::string name;

    static uint_fast64_t count;

    bool multi = false;
    uint64_t lower, upper;
};

// is used for shift operations and is another class
// to save memory! More on 2 bytes :) and one function
class GraphVertexShift : public GraphVertex {
  public:
    GraphVertexShift(OperationType type, uint64_t shift, uint64_t upper = 0,
                     uint64_t lower = 0);
    GraphVertexShift(GraphVertexShift *other);

    uint64_t getShift() const { return shift; }

    std::string toVerilog();

  protected:
    uint64_t shift = 0;
};

class GraphVertexConst : public GraphVertex {
  public:
    GraphVertexConst(int constValue);
    GraphVertexConst(GraphVertexConst *other);

    int getConstValue() const { return constValue; }

    std::string toVerilog();

  protected:
    int constValue;
};

#endif
