#include "BasicType.h"

BasicType::BasicType() {}

BasicType::~BasicType() {};

std::string BasicType::toVerilog() { return ""; };
VertexType BasicType::getType() { return type; }

std::string BasicType::getName() const { return name; };
uint32_t BasicType::getLevel() const { return level; }

void BasicType::setLevel(uint32_t level) { this->level = level; }
void BasicType::setName(std::string name) { this->name = name; }

uint32_t BasicType::getWireSize() const { return 0; }
std::string BasicType::getInstance() { return ""; }
