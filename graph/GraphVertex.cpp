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
                         uint16_t upper, uint16_t lower, std::string name) {
    if (lower > upper) {
        throw std::invalid_argument("Slice should have upper border greater"
                                    "than lower");
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

uint16_t GraphVertex::getWireSize() const { return upper - lower; }

std::string GraphVertex::getWireName() {
    return "wire " + getWireSize()
               ? "[ " + std::to_string(getWireSize()) + " : 0 ]"
               : "";
}

uint16_t GraphVertex::getLower() const { return lower; }

uint16_t GraphVertex::getUpper() const { return upper; }

std::string GraphVertex::toVerilog() {
    // we do not need to call it, when we have input,
    // for example, because it parses an operation
    // in case of input, we just need to declare it
    // in special way
    if (type != VertexType::Operation)
        return "";

    std::string basic = getWireName() + " " + name + " = ";

    if (operation == OperationType::SliceOper) {
        // we have only one operation in this case
        basic += inConnection.back()->getName() + "[ " + std::to_string(upper);
        basic += multi ? " : " + std::to_string(lower) + " ];" : " ]";

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
        basic += oper + inConnection.back()->getName();

        return basic;
    }

    for (int i = 0; i < inConnection.size() - 1; ++i) {
        basic += inConnection[i]->getName() + " " + oper + " ";
    }
    basic += inConnection.back()->getName() + ";";

    return basic;
}

GraphVertexConst::GraphVertexConst(int constValue)
    : GraphVertex(VertexType::Const, OperationType::Default, 0, 0) {
    this->constValue = constValue;
}

GraphVertexShift::GraphVertexShift(uint16_t shift, uint16_t upper,
                                   uint16_t lower)
    : GraphVertex(VertexType::Operation, OperationType::SliceOper, upper,
                  lower) {
    this->shift = shift;
}

// Shift toVerilog
std::string GraphVertexShift::toVerilog() {
    std::string basic = getWireName() + " " + name + " = ";
    std::string oper = VertexUtils::operationToString(operation);

    basic +=
        inConnection.back()->getName() + " " + oper + std::to_string(shift);

    return basic;
}
