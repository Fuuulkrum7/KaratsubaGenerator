#include "KaratsubaGen.h"

KaratsubaGen::KaratsubaGen(uint32_t bitDepth) {
    this->bitDepth = bitDepth;
}

uint32_t KaratsubaGen::getBitDepth() const {
    return bitDepth;
}

void KaratsubaGen::setBitDepth(uint32_t bitDepth) {
    this->bitDepth = bitDepth;
}

GraphPtr KaratsubaGen::getGraph() const {
    return graph;
}

GraphPtr KaratsubaGen::startGeneration() {
    // this func can be changed, that's why we moved 
    // generator in another func
    graph = generate(bitDepth);

    return graph;
}

GraphPtr KaratsubaGen::generate(uint32_t depth) {
    if (!depth) {
        return nullptr;
    }

    GraphPtr graphInst(new OrientedGraph());

    if (depth == 1) {
        auto inp1 = graphInst->addInput();
        auto inp2 = graphInst->addInput();

        auto out = graphInst->addOutput();

        auto andOper = graphInst->addOperation(OperationType::And);

        return graphInst;
    }

    int m = depth / 2;

    if (depth == 2) {

    }
    if (depth == 3) {

    }
}
