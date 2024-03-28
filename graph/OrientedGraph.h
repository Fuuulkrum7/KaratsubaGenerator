#ifndef ORIENTED_GRAPH
#define ORIENTED_GRAPH

#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "BasicType.h"
#include "GraphVertex.h"

#define GraphPtr std::shared_ptr<OrientedGraph>

class OrientedGraph : public BasicType {
  public:
    OrientedGraph(std::string name);

    OrientedGraph &operator=(const OrientedGraph &other) = default;
    OrientedGraph &operator=(OrientedGraph &&other) = default;
    OrientedGraph(const OrientedGraph &other) = default;
    OrientedGraph(OrientedGraph &&other) = default;

    ~OrientedGraph() = default;

    GraphPtr getParent() const;
    void setParent(GraphPtr parent);

    std::string toVerilog();

    VertexPtr addInput(uint16_t upper = 0, uint16_t lower = 0);
    VertexPtr addOutut(uint16_t upper = 0, uint16_t lower = 0);
    VertexPtr addOperation(OperationType type, uint16_t upper = 0,
                           uint16_t lower = 0, uint16_t shift = 0);
    VertexPtr addConst(int value);
    std::vector<VertexPtr> addSubgraph(GraphPtr subGraph,
                                       std::vector<VertexPtr> inputs);

    void addEdge(VertexPtr from, VertexPtr to);

    void addEdges(std::vector<VertexPtr> from, VertexPtr to);

    std::vector<VertexPtr> getVertexesByType(VertexType type) const;

    void setWriteStream(std::ofstream &out);

  protected:
    // in default is nullptr, when has a parent - is a subgraph
    GraphPtr parentGraph = nullptr;

    std::map<VertexType, std::vector<VertexPtr>> vertexes{
        { VertexType::Input, std::vector<VertexPtr>() },
        { VertexType::Output, std::vector<VertexPtr>() },
        { VertexType::Const, std::vector<VertexPtr>() },
        { VertexType::Operation, std::vector<VertexPtr>() },
        { VertexType::Graph, std::vector<VertexPtr>() }
    };

    // here are all added subgraphs
    std::set<GraphPtr> subGraphs;

    // each subgraph has one or more outputs. We save them,
    // depending on subgraph instance number
    std::map<int, std::vector<VertexPtr>> subGraphsOutputsPtr;
    // we have such pairs: number of subragh instances,
    std::map<int, std::vector<VertexPtr>> subGraphsInputsPtr;

    static uint16_t count;

    // also we need to now, was .v file for subgraph created, or not
    bool alreadyParsed = false;
    // We can add a subgraph multiple times
    // so we need to count instances
    uint64_t graphInstanceCount = 0;
    // we are counting to know, which inputs and outputs should we use now
    uint64_t graphInstanceToVerilogCount = 0;

  private:
    std::ofstream outFile;
};

#endif
