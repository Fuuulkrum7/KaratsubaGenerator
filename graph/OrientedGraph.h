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
#define GraphPtrWeak std::weak_ptr<OrientedGraph>

class OrientedGraph : public BasicType,
                      public std::enable_shared_from_this<OrientedGraph> {
  public:
    OrientedGraph();
    OrientedGraph(std::string name);
    OrientedGraph(OrientedGraph *other);

    OrientedGraph &operator=(const OrientedGraph &other) = default;
    OrientedGraph &operator=(OrientedGraph &&other) = default;
    OrientedGraph(const OrientedGraph &other) = default;
    OrientedGraph(OrientedGraph &&other) = default;

    ~OrientedGraph() = default;

    std::vector<GraphPtrWeak> getParents() const;
    void addParent(GraphPtrWeak parent);

    std::string toVerilog();

    VertexPtr addInput(uint64_t upper = 0, uint64_t lower = 0);
    VertexPtr addOutput(uint64_t upper = 0, uint64_t lower = 0);
    VertexPtr addOperation(OperationType type, uint64_t upper = 0,
                           uint64_t lower = 0, uint64_t shift = 0);
    VertexPtr addConst(int value);
    std::vector<VertexPtr> addSubgraph(GraphPtr subGraph,
                                       std::vector<VertexPtr> inputs);

    void addEdge(VertexPtr from, VertexPtr to);

    void addEdges(std::vector<VertexPtr> from, VertexPtr to);

    std::vector<VertexPtr> getVertexesByType(VertexType type) const;

    void setWritePath(std::string path);
    uint64_t getWireSize() const;

    std::string getInstance();
    static void setDefaultName(std::string name);
    static std::string getDefaultName();
    void setCurrentParent(GraphPtr parent);

    static void setCountGraphs(uint64_t n);
    static uint64_t getCountGraphs();

    void resetCounters(GraphPtr where);

  protected:
    static std::string defaultName;
    // whole instance count
    static size_t countNewGraphInstance;
    // curr instance ID
    size_t graphID;
    // is a subgraph when size is zero
    std::vector<GraphPtrWeak> parentGraphs;
    // as we can have multiple parents, we save
    // for toVerilog current parent graph
    GraphPtrWeak currentParentGraph;

    // all graph inputs
    // all outputs
    // all localparam
    // all operations
    // all subGraphs (in order they are created)
    std::map<VertexType, std::vector<VertexPtr>> vertices{
        { VertexType::Input, std::vector<VertexPtr>() },
        { VertexType::Output, std::vector<VertexPtr>() },
        { VertexType::Const, std::vector<VertexPtr>() },
        { VertexType::Operation, std::vector<VertexPtr>() },
        { VertexType::Graph, std::vector<VertexPtr>() }
    };

    // here are all unique added subgraphs
    std::set<GraphPtr> subGraphs;

    // each subgraph has one or more outputs. We save them,
    // depending on subgraph instance number
    std::map<uint64_t, std::vector<std::vector<VertexPtr>>> subGraphsOutputsPtr;
    std::vector<VertexPtr> allSubGraphsOutputs;
    // we have such pairs: number of subragh instances,
    std::map<uint64_t, std::vector<std::vector<VertexPtr>>> subGraphsInputsPtr;

    // graph instances count
    static uint64_t count;

    // also we need to now, was .v file for subgraph created, or not
    bool alreadyParsed = false;
    // We can add a subgraph multiple times
    // so we need to count instances to verilog.
    // We are counting to know, which inputs and outputs should we use now
    std::map<uint64_t, uint64_t> graphInstanceToVerilogCount;

  private:
    // path to creating file (.v)
    std::string path = "";
};

#endif
