#include <stdexcept>

#include "GraphVertex.h"

uint_fast64_t GraphVertex::count = 0;

GraphVertex::GraphVertex(VertexType type, OperationType operation,
                         uint16_t lower, uint16_t upper) {
    // type cannot be a subgraph, because for subgraph it is neccessary
    // to use graph as a BasicVertex
    if (type == VertexType::Graph) {
        throw std::invalid_argument("GraphVertex cannot be a graph,"
                                    "use OrientedGraph instead");
    }
    if (lower > upper) {
        throw std::invalid_argument("Slice should have upper border greater"
                                    "than lower");
    }

    this->type = type;
    this->operation = operation;

    this->name = this->getTypeName() + "_" + std::to_string(count++);

    this->multi = lower < upper;

    this->lower = lower;
    this->upper = upper;
}

std::string GraphVertex::getTypeName() const {
    switch (type) {
    case VertexType::Input:
        return "input";
    case VertexType::Output:
        return "ouput";
    case VertexType::Const:
        return "const";
    case VertexType::Operation:
        return "g";
    default:
        return "default";
    }

    return "default";
}

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

std::string GraphVertex::getName() const { return name; }

uint16_t GraphVertex::getWireSize() const { return upper - lower; }

uint16_t GraphVertex::getLower() const { return lower; }

uint16_t GraphVertex::getUpper() const { return upper; }

std::string GraphVertex::toVerilog() {
    std::string basic = "assign " + name + " = ";

    if (operation == OperationType::SliceOper) {
        // we have only one operation in this case
        basic += inConnection.back()->getName() + "[ " + std::to_string(upper);
        basic += multi ? " : " + std::to_string(lower) + " ];" : " ]";

        return basic;
    }

    std::string oper = VertexUtils::operationToString(operation);

    if (operation == OperationType::LShift || operation == OperationType::RShift) {
        // in default, if we did not use special class for shift
        // we just move val on 1
        basic += inConnection.back()->getName() + " " + oper + " 1;";
        return basic;
    }

    if (operation == OperationType::Not || operation == OperationType::Buf) {
        basic += oper + inConnection.back()->getName();

        return basic;
    }

    for (int i = 0; i < inConnection.size() - 1; ++i) {
        basic += inConnection[i]->getName() + " " + oper + " ";
    }
    basic += inConnection.back()->getName() + ";";

    return basic;
}

// Shift toVerilog
std::string GraphVertexShift::toVerilog() {
    std::string basic = "assign " + name + " = ";
    std::string oper = VertexUtils::operationToString(operation);

    basic += inConnection.back()->getName() + " " + oper + std::to_string(shift);

    return basic;
}
