#ifndef GRAPH_VERTEX
#define GRAPH_VERTEX

#include <memory>
#include <string>
#include <vector>

#include "BasicType.h"

using VertexPtr = std::shared_ptr<BasicType>;

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
std::string operationToString(OperationType type) {
    switch (type) {
    case OperationType::Not:
        return "~";
    case OperationType::And:
    case OperationType::Nand:
        return "&";
    case OperationType::Or:
    case OperationType::Nor:
        return "|";
    case OperationType::Xor:
    case OperationType::Xnor:
        return "^";
    case OperationType::Sum:
        return "+";
    case OperationType::Dif:
        return "-";
    case OperationType::LShift:
        return "<<";
    case OperationType::RShift:
        return ">>";
    // Buf, Default
    default:
        return "";
    }
}
} // namespace VertexUtils

// When we call toVerilog, we want to get string
class GraphVertex : public BasicType {
  public:
    // type of vertex, operation, provided by it,
    // is it [a:b] vertex, values b and a
    GraphVertex(VertexType type,
                OperationType operation = OperationType::Default,
                uint16_t lower = 0, uint16_t upper = 0);

    GraphVertex &operator=(const GraphVertex &other) = default;
    GraphVertex &operator=(GraphVertex &&other) = default;
    GraphVertex(const GraphVertex &other) = default;
    GraphVertex(GraphVertex &&other) = default;

    virtual std::string getTypeName() const;

    virtual void addParent(VertexPtr vertex);
    virtual void addChild(VertexPtr vertex);

    virtual std::vector<VertexPtr> getOutConnections() const;
    virtual std::vector<VertexPtr> getInConnections() const;

    virtual std::string getName() const;
    virtual uint16_t getWireSize() const;
    virtual uint16_t getLower() const;
    virtual uint16_t getUpper() const;

    virtual std::string toVerilog();

  protected:
    OperationType operation;

    std::vector<VertexPtr> inConnection;
    std::vector<VertexPtr> outConnection;

    std::string name;

    static uint_fast64_t count;

    bool multi = false;
    uint16_t lower, upper;
};


// is used for shift operations
// to save mamory! More on 2 bytes :) and one function
class GraphVertexShift : GraphVertex {
    GraphVertexShift(VertexType type, uint16_t lower = 0, uint16_t upper = 0,
                     uint16_t shift)
        : GraphVertex(type, OperationType::SliceOper, lower, upper) {
        this->shift = shift;
    }

    uint16_t getConstVal() const { return shift; }

	std::string toVerilog();

  protected:
    uint16_t shift = 0;
};

#endif
