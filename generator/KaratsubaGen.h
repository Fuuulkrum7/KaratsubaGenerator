#include <cstdint>
#include <map>
#include <vector>

#include <graph/GraphVertex.h>
#include <graph/OrientedGraph.h>

class KaratsubaGen {
  public:
    KaratsubaGen(uint32_t bitDepth);

    KaratsubaGen &operator=(const KaratsubaGen &other) = default;
    KaratsubaGen &operator=(KaratsubaGen &&other) = default;
    KaratsubaGen(const KaratsubaGen &other) = default;
    KaratsubaGen(KaratsubaGen &&other) = default;

    uint32_t getBitDepth() const;
    void setBitDepth(uint32_t bitDepth);

    GraphPtr getGraph() const;
    GraphPtr startGeneration();

  private:
    std::string algName = "Karatsuba";
    GraphPtr generate(uint32_t depth);

    std::map<int, GraphPtr> createdGraphs;

    uint32_t bitDepth;
    GraphPtr graph;
};
