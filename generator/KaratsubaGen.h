#include <cstdint>
#include <map>
#include <vector>

#include <graph/GraphVertex.h>
#include <graph/OrientedGraph.h>

class KaratsubaGen {
  public:
    KaratsubaGen(uint64_t bitDepth);

    KaratsubaGen &operator=(const KaratsubaGen &other) = default;
    KaratsubaGen &operator=(KaratsubaGen &&other) = default;
    KaratsubaGen(const KaratsubaGen &other) = default;
    KaratsubaGen(KaratsubaGen &&other) = default;

    uint64_t getBitDepth() const;
    void setBitDepth(uint64_t bitDepth);

    GraphPtr getGraph() const;
    GraphPtr startGeneration();

  private:
    std::string algName = "Karatsuba";
    // recursive generation
    GraphPtr generate(uint64_t depth);

    // basic case for 2-bit multiplier
    VertexPtr generateDouble(GraphPtr graphInst, VertexPtr slice_a_older,
                             VertexPtr slice_a_smaller, VertexPtr slice_b_older,
                             VertexPtr slice_b_smaller, VertexPtr sumA,
                             VertexPtr sumB);
    // basic case for 3-bit multiplier
    VertexPtr generateTriple(GraphPtr graphInst, VertexPtr slice_a_older,
                             VertexPtr slice_a_smaller, VertexPtr slice_b_older,
                             VertexPtr slice_b_smaller, VertexPtr sumA,
                             VertexPtr sumB);
    // standart generator for n-bit (n > 3)
    VertexPtr
    generateMultiple(GraphPtr graphInst, uint64_t depth, VertexPtr slice_a_older,
                     VertexPtr slice_a_smaller, VertexPtr slice_b_older,
                     VertexPtr slice_b_smaller, VertexPtr sumA, VertexPtr sumB);

    // here we store already calculated Graphs
    std::map<uint64_t, GraphPtr> createdGraphs;

    uint64_t bitDepth;
    // final graph
    GraphPtr graph;
};
