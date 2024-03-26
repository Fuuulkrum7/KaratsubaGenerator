#include <sstream>
#include <string>

#include "OrientedGraph.h"

OrientedGraph::OrientedGraph(std::string name) {
    type = VertexType::Graph;
    this->name = name;
}
