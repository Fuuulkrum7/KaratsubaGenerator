#include <stdexcept>

#include "GraphVertex.h"

std::string VertexUtils::operationToString(OperationType type) {
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

std::string VertexUtils::vertexTypeToString(VertexType type) {
    switch (type) {
    case VertexType::Input:
        return "input";
    case VertexType::Output:
        return "output";
    case VertexType::Const:
        return "localparam";
    case VertexType::Graph:
        return "module";
    case VertexType::Operation:
        return "wire";
    default:
        return "";
    }
}

uint_fast64_t GraphVertex::count = 0;

GraphVertex::GraphVertex(VertexType type, OperationType operation,
                         uint64_t upper, uint64_t lower, std::string name) {
    if (lower > upper) {
        throw std::invalid_argument("Slice should have upper border greater"
                                    " than lower");
    }

    this->type = type;
    this->operation = operation;

    if (!name.size()) {
        this->name = this->getTypeName() + "_" + std::to_string(count++);
    } else {
        this->name = name;
    }

    this->multi = lower < upper;

    this->lower = lower;
    this->upper = upper;
}

GraphVertex::GraphVertex(GraphVertex *other) {
    operation = other->operation;
    inConnection = other->inConnection;
    outConnection = other->outConnection;
    name = other->name;
    multi = other->multi;
    lower = other->lower;
    upper = other->upper;

    type = other->type;
    level = other->level;
}

std::string GraphVertex::getTypeName() const {
    switch (type) {
    case VertexType::Input:
        return "input";
    case VertexType::Output:
        return "output";
    case VertexType::Const:
        return "const";
    case VertexType::Operation:
        return "g";
    case VertexType::Graph:
        return "subgraph";
    default:
        return "default";
    }

    return "default";
}

std::string GraphVertex::getName() const { return name; }

void GraphVertex::setOperation(OperationType operation) {
    this->operation = operation;
}

OperationType GraphVertex::getOperation() const { return operation; }

void GraphVertex::addParent(VertexPtr vertex) {
    inConnection.push_back(vertex);
}

void GraphVertex::addChild(VertexPtr vertex) {
    outConnection.push_back(vertex);
}

std::vector<VertexPtr> GraphVertex::getOutConnections() const {
    return outConnection;
}

std::vector<VertexPtr> GraphVertex::getInConnections() const {
    return inConnection;
}

uint64_t GraphVertex::getWireSize() const { return upper - lower; }

void GraphVertex::setCountVertex(uint_fast64_t n) { count = n; }

uint_fast64_t GraphVertex::getCountVertex() { return count; }

std::string GraphVertex::getInstance() {
    return "wire " +
           (getWireSize() ? "[" + std::to_string(getWireSize()) + " : 0] "
                          : "") +
           name + ";";
}

uint64_t GraphVertex::getLower() const { return lower; }

uint64_t GraphVertex::getUpper() const { return upper; }

std::string GraphVertex::toVerilog() {
    if (type == VertexType::Output) {
        return "assign " + name + " = " + inConnection.back()->getName() + ";";
    }
    // we do not need to call it, when we have input,
    // for example, because it parses an operation
    // in case of input, we just need to declare it
    // in special way
    if (type != VertexType::Operation) {
        return "";
    }

    std::string basic = "assign " + name + " = ";

    if (operation == OperationType::SliceOper) {
        // we have only one operation in this case
        basic += inConnection.back()->getName() + "[" + std::to_string(upper);
        basic += multi ? " : " + std::to_string(lower) + "];" : "];";

        return basic;
    }

    std::string oper = VertexUtils::operationToString(operation);

    if (operation == OperationType::LShift ||
        operation == OperationType::RShift) {
        // in default, if we did not use special class for shift
        // we just move val on 1
        basic += inConnection.back()->getName() + " " + oper + " 1;";
        return basic;
    }

    if (operation == OperationType::Not || operation == OperationType::Buf) {
        basic += oper + inConnection.back()->getName() + ";";

        return basic;
    }

    std::string end = "";

    if (operation == OperationType::Nand || operation == OperationType::Nor ||
        operation == OperationType::Xnor) {
        basic += "~ ( ";

        end = " )";
    }

    for (size_t i = 0; i < inConnection.size() - 1; ++i) {
        basic += inConnection[i]->getName() + " " + oper + " ";
    }
    basic += inConnection.back()->getName() + end + ";";

    return basic;
}

GraphVertexConst::GraphVertexConst(int constValue)
    : GraphVertex(VertexType::Const, OperationType::Default, 0, 0) {
    this->constValue = constValue;
}

std::string GraphVertexConst::toVerilog() {
    return "localparam " + name + +" = " + std::to_string(constValue) + ";";
}

GraphVertexConst::GraphVertexConst(GraphVertexConst *other)
    : GraphVertex(other) {
    constValue = other->constValue;
    level = other->level;
    name = other->name;
}

GraphVertexShift::GraphVertexShift(OperationType type, uint64_t shift,
                                   uint64_t upper, uint64_t lower)
    : GraphVertex(VertexType::Operation, type, upper, lower) {
    if (type != OperationType::RShift && type != OperationType::LShift) {
        throw std::invalid_argument(
            "GraphVertexShift type: shift should be shift!");
    }
    this->shift = shift;
}

GraphVertexShift::GraphVertexShift(GraphVertexShift *other)
    : GraphVertex(other) {
    shift = other->shift;
}

// Shift toVerilog
std::string GraphVertexShift::toVerilog() {
    std::string basic = "assign " + name + " = ";
    std::string oper = VertexUtils::operationToString(operation);

    basic += inConnection.back()->getName() + " " + oper + " " +
             std::to_string(shift) + ";";

    return basic;
}
