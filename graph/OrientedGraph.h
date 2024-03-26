#ifndef ORIENTED_GRAPH
#define ORIENTED_GRAPH

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "BasicType.h"
#include "GraphVertex.h"

using GraphPtr = std::shared_ptr<OrientedGraph>;

class OrientedGraph : public BasicType {
  public:
    OrientedGraph(std::string name);

    OrientedGraph &operator=(const OrientedGraph &other) = default;
    OrientedGraph &operator=(OrientedGraph &&other) = default;
    OrientedGraph(const OrientedGraph &other) = default;
    OrientedGraph(OrientedGraph &&other) = default;

    GraphPtr getParent();

    std::string toVerilog();

    VertexPtr addInput();
    VertexPtr addOutut();
    VertexPtr addConst();
    VertexPtr addSubgraph();
    VertexPtr addOperation();

    void addEdge(VertexPtr from, VertexPtr to);

    void addEdges(std::vector<VertexPtr> from, VertexPtr to);

  protected:
    // in default is nullptr, when has a parent - is a subgraph
    GraphPtr parent = nullptr;

    std::vector<VertexPtr> inputs;
    std::vector<VertexPtr> outputs;
    std::vector<VertexPtr> consts;
    std::vector<VertexPtr> operations;

    std::vector<GraphPtr> subgraphs;

    bool alreadyParsed = false;
};

#endif
