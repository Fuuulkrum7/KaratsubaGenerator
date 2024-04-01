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

class OrientedGraph : public BasicType,
                      public std::enable_shared_from_this<OrientedGraph> {
  public:
    OrientedGraph();
    OrientedGraph(std::string name);

    OrientedGraph &operator=(const OrientedGraph &other) = default;
    OrientedGraph &operator=(OrientedGraph &&other) = default;
    OrientedGraph(const OrientedGraph &other) = default;
    OrientedGraph(OrientedGraph &&other) = default;

    ~OrientedGraph() = default;

    std::set<GraphPtr> getParents() const;
    void addParent(GraphPtr parent);

    std::string toVerilog();

    VertexPtr addInput(uint32_t upper = 0, uint32_t lower = 0);
    VertexPtr addOutput(uint32_t upper = 0, uint32_t lower = 0);
    VertexPtr addOperation(OperationType type, uint32_t upper = 0,
                           uint32_t lower = 0, uint32_t shift = 0);
    VertexPtr addConst(int value);
    std::vector<VertexPtr> addSubgraph(GraphPtr subGraph,
                                       std::vector<VertexPtr> inputs);

    void addEdge(VertexPtr from, VertexPtr to);

    void addEdges(std::vector<VertexPtr> from, VertexPtr to);

    std::vector<VertexPtr> getVertexesByType(VertexType type) const;

    void setWritePath(std::string path);
    uint32_t getWireSize() const;

    std::string getInstance();
    static void setDefaultName(std::string name);
    static std::string getDefaultName();
    void setCurrentParent(GraphPtr parent);

  protected:
    static std::string defaultName;
    // is a subgraph when size is zero
    std::set<GraphPtr> parentGraphs;
    // as we can have multiple parents, we save 
    // for toVerilog current parent graph
    GraphPtr currentParentGraph;

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
    std::map<GraphPtr, std::map<int, std::vector<VertexPtr>>>
        subGraphsOutputsPtr;
    std::vector<VertexPtr> allSubGraphsOutputs;
    // we have such pairs: number of subragh instances,
    std::map<GraphPtr, std::map<int, std::vector<VertexPtr>>>
        subGraphsInputsPtr;

    static uint32_t count;

    // also we need to now, was .v file for subgraph created, or not
    bool alreadyParsed = false;
    // We can add a subgraph multiple times
    // so we need to count instances
    std::map<GraphPtr, uint64_t> graphInstanceCount;
    // we are counting to know, which inputs and outputs should we use now
    std::map<GraphPtr, uint64_t> graphInstanceToVerilogCount;

    uint64_t parentCount = 0;

  private:
    std::string path = "";
};

#endif
