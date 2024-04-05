#include "KaratsubaGen.h"
#include <iostream>

KaratsubaGen::KaratsubaGen(uint64_t bitDepth) { this->bitDepth = bitDepth; }

uint64_t KaratsubaGen::getBitDepth() const { return bitDepth; }

void KaratsubaGen::setBitDepth(uint64_t bitDepth) { this->bitDepth = bitDepth; }

GraphPtr KaratsubaGen::getGraph() const { return graph; }

GraphPtr KaratsubaGen::startGeneration() {
    // this func can be changed, that's why we moved
    // generator in another func
    std::string prevName = OrientedGraph::getDefaultName();
    OrientedGraph::setDefaultName(algName);
    graph = generate(bitDepth);

    OrientedGraph::setDefaultName(prevName);

    return graph;
}

VertexPtr KaratsubaGen::generateDouble(GraphPtr graphInst,
                                       VertexPtr slice_a_older,
                                       VertexPtr slice_a_smaller,
                                       VertexPtr slice_b_older,
                                       VertexPtr slice_b_smaller,
                                       VertexPtr sumA, VertexPtr sumB) {
    // in case of 2, we need to calc (a1 + a0) * (b1 + b0)
    VertexPtr mulOld = graphInst->addOperation(OperationType::And);
    VertexPtr mulSmall = graphInst->addOperation(OperationType::And);

    graphInst->addEdges({ slice_a_older, slice_b_older }, mulOld);
    graphInst->addEdges({ slice_a_smaller, slice_b_smaller }, mulSmall);

    // wire sl1 = a[n - 1:m]
    VertexPtr sum_a_slice_1 =
        graphInst->addOperation(OperationType::SliceOper, 1, 1);
    // wire sl2 = a[m - 1:0]
    VertexPtr sum_a_slice_0 =
        graphInst->addOperation(OperationType::SliceOper, 0, 0);
    // wire sl3 = b[n - 1:m]
    VertexPtr sum_b_slice_1 =
        graphInst->addOperation(OperationType::SliceOper, 1, 1);
    // wire sl4 = b[m - 1:0]
    VertexPtr sum_b_slice_0 =
        graphInst->addOperation(OperationType::SliceOper, 0, 0);

    graphInst->addEdge(sumA, sum_a_slice_1);
    graphInst->addEdge(sumA, sum_a_slice_0);
    graphInst->addEdge(sumB, sum_b_slice_1);
    graphInst->addEdge(sumB, sum_b_slice_0);

    // multiplying sums

    // first & last bits of sum
    VertexPtr mulSumOld = graphInst->addOperation(OperationType::And);
    VertexPtr mulSumSmall = graphInst->addOperation(OperationType::And);

    graphInst->addEdges({ sum_a_slice_1, sum_b_slice_1 }, mulSumOld);
    graphInst->addEdges({ sum_a_slice_0, sum_b_slice_0 }, mulSumSmall);

    // calculating sum of first & second bits
    VertexPtr sumOfSumA = graphInst->addOperation(OperationType::Or);
    VertexPtr sumOfSumB = graphInst->addOperation(OperationType::Or);

    graphInst->addEdges({ sum_a_slice_1, sum_a_slice_0 }, sumOfSumA);
    graphInst->addEdges({ sum_b_slice_1, sum_b_slice_0 }, sumOfSumB);

    // mul calculated sums
    VertexPtr mulSum = graphInst->addOperation(OperationType::And);
    graphInst->addEdges({ sumOfSumA, sumOfSumB }, mulSum);

    // calc difference
    VertexPtr diff = graphInst->addOperation(OperationType::Dif);
    graphInst->addEdges({ mulSum, mulSumOld, mulSumSmall }, diff);

    // make shift for sum in sum
    VertexPtr shiftDiff =
        graphInst->addOperation(OperationType::LShift, 1, 0, 1);
    VertexPtr shiftMul =
        graphInst->addOperation(OperationType::LShift, 2, 0, 2);

    graphInst->addEdge(diff, shiftDiff);
    graphInst->addEdge(mulSumOld, shiftMul);

    // final calc of 2-nd sum element
    VertexPtr preFinal = graphInst->addOperation(OperationType::Sum, 2, 0);
    graphInst->addEdges({ mulSumSmall, shiftDiff, shiftMul }, preFinal);

    // final calc
    // calc difference
    VertexPtr mainDiff = graphInst->addOperation(OperationType::Dif, 2, 0);
    graphInst->addEdges({ preFinal, mulOld, mulSmall }, mainDiff);

    // make shift of main
    VertexPtr shiftMainDiff =
        graphInst->addOperation(OperationType::LShift, 3, 0, 1);
    VertexPtr shiftMainMul =
        graphInst->addOperation(OperationType::LShift, 2, 0, 2);

    graphInst->addEdge(mainDiff, shiftMainDiff);
    graphInst->addEdge(mulOld, shiftMainMul);

    VertexPtr finalRes = graphInst->addOperation(OperationType::Sum, 3, 0);
    graphInst->addEdges({ mulSmall, shiftMainDiff, shiftMainMul }, finalRes);

    return finalRes;
}

VertexPtr KaratsubaGen::generateTriple(GraphPtr graphInst,
                                       VertexPtr slice_a_older,
                                       VertexPtr slice_a_smaller,
                                       VertexPtr slice_b_older,
                                       VertexPtr slice_b_smaller,
                                       VertexPtr sumA, VertexPtr sumB) {
    // creating slices of sum
    VertexPtr sliceSumAOlder =
        graphInst->addOperation(OperationType::SliceOper, 2, 1);
    VertexPtr sliceSumBOlder =
        graphInst->addOperation(OperationType::SliceOper, 2, 1);
    VertexPtr sliceSumASmaller =
        graphInst->addOperation(OperationType::SliceOper, 0, 0);
    VertexPtr sliceSumBSmaller =
        graphInst->addOperation(OperationType::SliceOper, 0, 0);

    graphInst->addEdge(sumA, sliceSumAOlder);
    graphInst->addEdge(sumA, sliceSumASmaller);
    graphInst->addEdge(sumB, sliceSumBOlder);
    graphInst->addEdge(sumB, sliceSumBSmaller);

    // calculating sum of sum parts
    VertexPtr subSumA = graphInst->addOperation(OperationType::Sum, 1, 0);
    VertexPtr subSumB = graphInst->addOperation(OperationType::Sum, 1, 0);

    graphInst->addEdges({ sliceSumAOlder, sliceSumASmaller }, subSumA);
    graphInst->addEdges({ sliceSumBOlder, sliceSumBSmaller }, subSumB);

    // calc wire s0 = a[2:1] * b[2:1]
    GraphPtr sliceAB_Graph = generate(2);
    // calc wire s1 = sumA[2:1] * sumB[2:1]
    GraphPtr sliceSumAB_Graph = generate(2);
    // calc wire s2 = (sumA[2:1] + sumA[0]) * (sumB[2:1] + sumB[0])
    GraphPtr sliceSubSumAB_Graph = generate(2);

    VertexPtr multSliceAB =
        graphInst->addSubgraph(sliceAB_Graph, { slice_a_older, slice_b_older })
            .back();
    VertexPtr multSliceSumAB =
        graphInst
            ->addSubgraph(sliceSumAB_Graph, { sliceSumAOlder, sliceSumBOlder })
            .back();
    VertexPtr multSliceSubSumAB =
        graphInst->addSubgraph(sliceSubSumAB_Graph, { subSumA, subSumB })
            .back();

    // wire s3 = a[0] & b[0];
    VertexPtr multAB_Smaller = graphInst->addOperation(OperationType::And);
    // wire s4 = sumA[0] & sumB[0];
    VertexPtr multSumAB_Smaller = graphInst->addOperation(OperationType::And);

    graphInst->addEdges({ slice_a_smaller, slice_b_smaller }, multAB_Smaller);
    graphInst->addEdges({ sliceSumASmaller, sliceSumBSmaller },
                        multSumAB_Smaller);

    // wire s5 = s2 - s4 - s1;
    VertexPtr sumAB_Diff = graphInst->addOperation(OperationType::Dif, 3, 0);
    graphInst->addEdges(
        { multSliceSubSumAB, multSumAB_Smaller, multSliceSumAB }, sumAB_Diff);

    // wire s6 = s5 << 1;
    VertexPtr shiftSumAB_Diff =
        graphInst->addOperation(OperationType::LShift, 4, 0, 1);
    // wire s7 = s1 << 2;
    VertexPtr shiftMultSliceSumAB =
        graphInst->addOperation(OperationType::LShift, 4, 0, 2);

    graphInst->addEdge(sumAB_Diff, shiftSumAB_Diff);
    graphInst->addEdge(multSliceSumAB, shiftMultSliceSumAB);

    VertexPtr preFinalSum = graphInst->addOperation(OperationType::Sum, 5, 0);
    graphInst->addEdges(
        { multSumAB_Smaller, shiftSumAB_Diff, shiftMultSliceSumAB },
        preFinalSum);

    VertexPtr finalDiff = graphInst->addOperation(OperationType::Dif, 5, 0);
    graphInst->addEdges({ preFinalSum, multAB_Smaller, multSliceAB },
                        finalDiff);

    VertexPtr shiftDiff =
        graphInst->addOperation(OperationType::LShift, 6, 0, 1);
    VertexPtr shiftMultSliceAB =
        graphInst->addOperation(OperationType::LShift, 6, 0, 2);

    graphInst->addEdge(finalDiff, shiftDiff);
    graphInst->addEdge(multSliceAB, shiftMultSliceAB);

    VertexPtr finalRes = graphInst->addOperation(OperationType::Sum, 7, 0);
    graphInst->addEdges({ multAB_Smaller, shiftDiff, shiftMultSliceAB },
                        finalRes);

    return finalRes;
}

VertexPtr KaratsubaGen::generateMultiple(GraphPtr graphInst, uint64_t depth,
                                         VertexPtr slice_a_older,
                                         VertexPtr slice_a_smaller,
                                         VertexPtr slice_b_older,
                                         VertexPtr slice_b_smaller,
                                         VertexPtr sumA, VertexPtr sumB) {
    uint64_t m = depth / 2;
    // it's graph for m bits
    GraphPtr mGraph = generate(m);
    // for n - m bits
    GraphPtr nGraph = generate(depth - m);

    GraphPtr sumGraph = generate(depth - m + 1);

    // multiplying them
    VertexPtr outMGraph =
        graphInst->addSubgraph(mGraph, { slice_a_smaller, slice_b_smaller })
            .back();
    VertexPtr outNGraph =
        graphInst->addSubgraph(nGraph, { slice_a_older, slice_b_older }).back();
    VertexPtr outSumGraph =
        graphInst->addSubgraph(sumGraph, { sumA, sumB }).back();

    // Calc difference
    VertexPtr diff =
        graphInst->addOperation(OperationType::Dif, (depth - m) * 2 + 1);

    // IMPORTANT! Is sencetive for parent vertex order. We make smaller only
    // first one, like "outSum - outM - outN"
    graphInst->addEdges({ outSumGraph, outMGraph, outNGraph }, diff);

    // Making shift on m and m * 2
    VertexPtr shiftDiff =
        graphInst->addOperation(OperationType::LShift, depth * 2 - m, 0, m);
    VertexPtr shiftMult =
        graphInst->addOperation(OperationType::LShift, depth * 2 - 1, 0, m * 2);

    graphInst->addEdge(diff, shiftDiff);
    graphInst->addEdge(outNGraph, shiftMult);

    // final sum calculation
    VertexPtr finalSum =
        graphInst->addOperation(OperationType::Sum, depth * 2 - 1, 0);

    graphInst->addEdges({ outMGraph, shiftDiff, shiftMult }, finalSum);

    return finalSum;
}

GraphPtr KaratsubaGen::generate(uint64_t depth) {
    if (!depth) {
        return nullptr;
    }

    if (createdGraphs.count(depth))
        return createdGraphs[depth];

    GraphPtr graphInst(new OrientedGraph());

    // declare inputs & output
    auto inpA = graphInst->addInput(depth - 1);
    auto inpB = graphInst->addInput(depth - 1);

    auto result = graphInst->addOutput(depth * 2 - 1);

    uint64_t m = depth / 2;

    // basic case, when we have only one bit
    if (depth == 1) {
        auto andOper = graphInst->addOperation(OperationType::And);

        graphInst->addEdges({ inpA, inpB }, andOper);
        graphInst->addEdge(andOper, result);

        return graphInst;
    }

    // in any operation type we have such variables
    // wire sl1 = a[n - 1:m]
    VertexPtr slice_a_older =
        graphInst->addOperation(OperationType::SliceOper, depth - 1, m);
    // wire sl2 = a[m - 1:0]
    VertexPtr slice_a_smaller =
        graphInst->addOperation(OperationType::SliceOper, m - 1, 0);
    // wire sl3 = b[n - 1:m]
    VertexPtr slice_b_older =
        graphInst->addOperation(OperationType::SliceOper, depth - 1, m);
    // wire sl4 = b[m - 1:0]
    VertexPtr slice_b_smaller =
        graphInst->addOperation(OperationType::SliceOper, m - 1, 0);

    graphInst->addEdge(inpA, slice_a_older);
    graphInst->addEdge(inpA, slice_a_smaller);
    graphInst->addEdge(inpB, slice_b_older);
    graphInst->addEdge(inpB, slice_b_smaller);

    // adding sum of inputs parts
    VertexPtr sumA = graphInst->addOperation(OperationType::Sum, depth - m);
    VertexPtr sumB = graphInst->addOperation(OperationType::Sum, depth - m);

    graphInst->addEdges({ slice_a_older, slice_a_smaller }, sumA);
    graphInst->addEdges({ slice_b_older, slice_b_smaller }, sumB);

    if (depth == 2) {
        VertexPtr finalRes =
            generateDouble(graphInst, slice_a_older, slice_a_smaller,
                           slice_b_older, slice_b_smaller, sumA, sumB);
        graphInst->addEdge(finalRes, result);
    }

    else if (depth == 3) {
        VertexPtr finalRes =
            generateTriple(graphInst, slice_a_older, slice_a_smaller,
                           slice_b_older, slice_b_smaller, sumA, sumB);
        graphInst->addEdge(finalRes, result);
    }

    else {
        VertexPtr finalSum =
            generateMultiple(graphInst, depth, slice_a_older, slice_a_smaller,
                             slice_b_older, slice_b_smaller, sumA, sumB);
        graphInst->addEdge(finalSum, result);
    }

    createdGraphs[depth] = graphInst;

    return graphInst;
}
