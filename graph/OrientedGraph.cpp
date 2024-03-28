#include <sstream>
#include <stdexcept>
#include <string>

#include "OrientedGraph.h"

uint16_t OrientedGraph::count = 0;

OrientedGraph::OrientedGraph(std::string name) {
    type = VertexType::Graph;
    if (name.size()) {
        this->name = name;
    } else {
        name = "Submodule_" + std::to_string(count);
        ++count;
    }
}

GraphPtr OrientedGraph::getParent() const { return parentGraph; }

void OrientedGraph::setParent(GraphPtr parent) { parentGraph = parent; }

std::vector<VertexPtr> OrientedGraph::getVertexesByType(VertexType type) const {
    return vertexes.at(type);
}

VertexPtr OrientedGraph::addInput(uint16_t upper, uint16_t lower) {
    VertexPtr newVertex(new GraphVertex(VertexType::Input,
                                        OperationType::Default, upper, lower));
    vertexes[VertexType::Input].push_back(newVertex);

    return newVertex;
}

VertexPtr OrientedGraph::addOutut(uint16_t upper, uint16_t lower) {
    VertexPtr newVertex(new GraphVertex(VertexType::Output,
                                        OperationType::Default, upper, lower));
    vertexes[VertexType::Output].push_back(newVertex);

    return newVertex;
}

VertexPtr OrientedGraph::addConst(int value) {
    VertexPtr newVertex(new GraphVertexConst(value));
    vertexes[VertexType::Input].push_back(newVertex);

    return newVertex;
}

std::vector<VertexPtr>
OrientedGraph::addSubgraph(GraphPtr subGraph, std::vector<VertexPtr> inputs) {
    std::vector<VertexPtr> iGraph =
        subGraph->getVertexesByType(VertexType::Input);
    if (inputs.size() != iGraph.size()) {
        throw std::invalid_argument(
            "Number of inputs should be same, as subgrap inputs number");
    }

    // check if one of inputs is incorrect
    for (int i = 0; i < inputs.size(); ++i) {
        // size of input like wire
        int size_inp =
            std::static_pointer_cast<GraphVertex>(inputs[i])->getWireSize();
        // size of input from subgraph
        int size_grph =
            std::static_pointer_cast<GraphVertex>(iGraph[i])->getWireSize();
        if (size_inp != size_grph) {
            throw std::invalid_argument(
                "Inputs should have same wire size, as subgrap inputs, but " +
                std::to_string(i) + " input has another: input is " +
                std::to_string(size_inp) + ", and shoud be " +
                std::to_string(size_grph));
        }
    }

    std::vector<VertexPtr> outputs;
    outputs.reserve(subGraph->getVertexesByType(VertexType::Output).size());

    for (auto out : subGraph->getVertexesByType(VertexType::Output)) {
        std::shared_ptr<GraphVertex> outVert =
            std::static_pointer_cast<GraphVertex>(out);

        VertexPtr newVertex(
            new GraphVertex(VertexType::Output, OperationType::Default,
                            outVert->getUpper(), outVert->getLower()));

        outputs.push_back(newVertex);
    }

    // here we save our inputs and outputs to instance number
    subGraphsInputsPtr[graphInstanceCount] = inputs;
    subGraphsOutputsPtr[graphInstanceCount] = outputs;

    ++graphInstanceCount;

    // here we use subgraph like an instance of BasicType,
    // and we call it's toVerilog, having in multiple instance
    // of one subGraph, so we can have many times "moduleName name (inp, out);"
    // having different names of module, inputs and outputs
    vertexes[VertexType::Graph].push_back(subGraph);
    subGraphs.insert(subGraph);

    return outputs;
}

VertexPtr OrientedGraph::addOperation(OperationType type, uint16_t upper,
                                      uint16_t lower, uint16_t shift) {
    VertexPtr newVertex;

    if (type == OperationType::RShift || type == OperationType::LShift) {
        newVertex.reset(new GraphVertexShift(shift, upper, lower));
    } else {
        newVertex.reset(
            new GraphVertex(VertexType::Operation, type, lower, upper));
    }

    vertexes[VertexType::Operation].push_back(newVertex);

    return newVertex;
}

void OrientedGraph::setWriteStream(std::ofstream &out) {
    this->outFile.swap(out);
}

std::string OrientedGraph::toVerilog() {
    std::string verilogTab = "  ";

    outFile << "module " << name << "(\n" << verilogTab;

    std::map<int, std::vector<VertexPtr>> inputByWireSize;
    std::map<int, std::vector<VertexPtr>> outputByWireSize;
    std::map<int, std::vector<VertexPtr>> subGraphOutputsByWireSize;

    // here we are writing input names
    for (auto inp : vertexes[VertexType::Input]) {
        auto inpVert = std::static_pointer_cast<GraphVertex>(inp);
        if (!inputByWireSize.count(inpVert->getWireSize())) {
            inputByWireSize[inpVert->getWireSize()] = {};
        }

        // check if key exists
        inputByWireSize[inpVert->getWireSize()].push_back(inp);

        outFile << inp->getName() << ", ";
    }

    outFile << "\n" << verilogTab;

    // and outputs
    for (int i = 0; i < vertexes[VertexType::Output].size() - 1; ++i) {
        auto outVert = std::static_pointer_cast<GraphVertex>(
            vertexes[VertexType::Output][i]);

        if (!outputByWireSize.count(outVert->getWireSize())) {
            outputByWireSize[outVert->getWireSize()] = {};
        }

        outputByWireSize[outVert->getWireSize()].push_back(outVert);

        outFile << outVert->getName() << ", ";
    }

    // here is last output
    {
        auto outVert = std::static_pointer_cast<GraphVertex>(
            vertexes[VertexType::Output].back());

        if (!outputByWireSize.count(outVert->getWireSize())) {
            outputByWireSize[outVert->getWireSize()] = {};
        }

        outputByWireSize[outVert->getWireSize()].push_back(outVert);

        outFile << outVert->getName() << "\n"
                << ");\n"
                << verilogTab;
    }

    for (auto [key, subOutputs] : subGraphsOutputsPtr) {
        for (auto subOutput : subOutputs) {
            auto outVert = std::static_pointer_cast<GraphVertex>(subOutput);

            if (!subGraphOutputsByWireSize.count(outVert->getWireSize())) {
                subGraphOutputsByWireSize[outVert->getWireSize()] = {};
            }

            subGraphOutputsByWireSize[outVert->getWireSize()].push_back(outVert);
        }
    }

    // parsing inputs, outputs and wires for subgraphs
    for (auto byWireSize :
         { inputByWireSize, outputByWireSize, subGraphOutputsByWireSize }) {
        for (auto [key, value] : byWireSize) {
            outFile << VertexUtils::vertexTypeToString(value.back()->getType())
                    << " ";
            if (key) {
                outFile << "[ " << key << " : 0 ] ";
            }

            for (int i = 0; i < value.size() - 1; ++i) {
                outFile << value[i]->getName() << ", ";
            }
            outFile << value.back()->getName() << ";\n" << verilogTab;
        }
    }

    if (parentGraph.get() != nullptr) {
    }

    return "";
}
