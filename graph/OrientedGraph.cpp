#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "OrientedGraph.h"

uint32_t OrientedGraph::count = 0;
std::string OrientedGraph::defaultName = "Submodule";

OrientedGraph::OrientedGraph() {
    type = VertexType::Graph;

    name = defaultName + "_" + std::to_string(count);
    ++count;
}

OrientedGraph::OrientedGraph(std::string name) {
    type = VertexType::Graph;
    if (name.size()) {
        this->name = name;
    } else {
        name = defaultName + "_" + std::to_string(count);
        ++count;
    }
}

std::set<GraphPtr> OrientedGraph::getParents() const {
    return parentGraphs;
}

void OrientedGraph::setDefaultName(std::string name) { defaultName = name; }

std::string OrientedGraph::getDefaultName() { return defaultName; }

void OrientedGraph::addParent(GraphPtr parent) {
    parentGraphs.insert(parent);
}

std::vector<VertexPtr> OrientedGraph::getVertexesByType(VertexType type) const {
    return vertexes.at(type);
}

VertexPtr OrientedGraph::addInput(uint32_t upper, uint32_t lower) {
    VertexPtr newVertex(new GraphVertex(VertexType::Input,
                                        OperationType::Default, upper, lower));
    vertexes[VertexType::Input].push_back(newVertex);

    return newVertex;
}

VertexPtr OrientedGraph::addOutput(uint32_t upper, uint32_t lower) {
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

    subGraph->addParent(shared_from_this());

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
    // outputs.reserve(subGraph->getVertexesByType(VertexType::Output).size());

    for (auto out : subGraph->getVertexesByType(VertexType::Output)) {
        std::shared_ptr<GraphVertex> outVert =
            std::static_pointer_cast<GraphVertex>(out);

        VertexPtr newVertex(
            new GraphVertex(VertexType::Operation, OperationType::Buf,
                            outVert->getUpper(), outVert->getLower()));

        outputs.push_back(newVertex);
        allSubGraphsOutputs.push_back(newVertex);
    }

    if (!subGraph->graphInstanceCount.count(shared_from_this())) {
        subGraph->graphInstanceCount[shared_from_this()] = 0;
    }
    uint64_t *val = &subGraph->graphInstanceCount[shared_from_this()];
    // here we save our inputs and outputs to instance number
    subGraph->subGraphsInputsPtr[shared_from_this()][*val] = inputs;
    subGraph->subGraphsOutputsPtr[shared_from_this()][*val] = outputs;
    // subGraphsInputsPtr[graphInstanceCount] = inputs;
    // subGraphsOutputsPtr[graphInstanceCount] = outputs;

    ++(*val);

    // here we use subgraph like an instance of BasicType,
    // and we call it's toVerilog, having in multiple instance
    // of one subGraph, so we can have many times "moduleName name (inp, out);"
    // having different names of module, inputs and outputs
    vertexes[VertexType::Graph].push_back(subGraph);
    subGraphs.insert(subGraph);

    return outputs;
}

VertexPtr OrientedGraph::addOperation(OperationType type, uint32_t upper,
                                      uint32_t lower, uint32_t shift) {
    VertexPtr newVertex;

    if (type == OperationType::RShift || type == OperationType::LShift) {
        newVertex.reset(new GraphVertexShift(type, shift, upper, lower));
    } else {
        newVertex.reset(
            new GraphVertex(VertexType::Operation, type, upper, lower));
    }

    vertexes[VertexType::Operation].push_back(newVertex);

    return newVertex;
}

void OrientedGraph::addEdge(VertexPtr from, VertexPtr to) {
    auto fromVert = std::static_pointer_cast<GraphVertex>(from);
    auto toVert = std::static_pointer_cast<GraphVertex>(to);

    toVert->addParent(fromVert);
    fromVert->addChild(toVert);
}

void OrientedGraph::addEdges(std::vector<VertexPtr> from, VertexPtr to) {
    auto toVert = std::static_pointer_cast<GraphVertex>(to);

    for (auto elem : from) {
        auto fromVert = std::static_pointer_cast<GraphVertex>(elem);

        toVert->addParent(fromVert);
        fromVert->addChild(toVert);
    }
}

void OrientedGraph::setWritePath(std::string path) {
    this->path = path;

    // we will stop, when subgraph would not have any subgraphs
    for (auto i : subGraphs) {
        i->setWritePath(path);
    }
}

void OrientedGraph::setCurrentParent(GraphPtr parent) {
    currentParentGraph = parent;
}

// for parsing graph to module instance
std::string OrientedGraph::curInstToString() {
    uint64_t *verilogCount = &graphInstanceToVerilogCount[currentParentGraph];
    uint64_t *allCount = &graphInstanceCount[currentParentGraph];

    if (*verilogCount == *allCount) {
        throw std::out_of_range(
            "Incorrect curInstToString call. All modules (" +
            std::to_string(*allCount) + ") were already parsed");
    }

    std::string verilogTab = "  ";
    // module_name module_name_inst_1 (
    std::string module_ver = verilogTab + name + " " + name + "_inst_" +
                             std::to_string(*verilogCount) + " (\n";
    for (auto inp : subGraphsInputsPtr[currentParentGraph][*verilogCount]) {
        module_ver += verilogTab + verilogTab;
        module_ver += inp->getName() + ",\n";
    }

    for (int i = 0;
         i < subGraphsOutputsPtr[currentParentGraph][*verilogCount].size() - 1; ++i) {
        VertexPtr out = subGraphsOutputsPtr[currentParentGraph][*verilogCount][i];
        module_ver += verilogTab + verilogTab;
        module_ver += out->getName() + ",\n";
    }

    module_ver += verilogTab + verilogTab;
    module_ver +=
        subGraphsOutputsPtr[currentParentGraph][*verilogCount].back()->getName() +
        "\n";
    module_ver += verilogTab + "); \n";

    ++(*verilogCount);

    return module_ver;
}

std::string OrientedGraph::toVerilog() {
    std::string verilogTab = "  ";
    // if it's a subGraph and it was already parsed to .v file
    // we can just return a new instance of module
    if (alreadyParsed && parentGraphs.size()) {
        return curInstToString();
    }
    std::string curPath = path;

    if (parentGraphs.size()) {
        curPath += "submodules";
    }
    std::filesystem::create_directories(curPath);

    std::ofstream outFile(curPath + "/" + name + ".v");
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

    // writing wires for modules
    // i thought it would look better
    for (auto subOutput : allSubGraphsOutputs) {
        auto outVert = std::static_pointer_cast<GraphVertex>(subOutput);

        if (!subGraphOutputsByWireSize.count(outVert->getWireSize())) {
            subGraphOutputsByWireSize[outVert->getWireSize()] = {};
        }

        subGraphOutputsByWireSize[outVert->getWireSize()].push_back(outVert);
    }

    // parsing inputs, outputs and wires for subgraphs
    for (auto byWireSize :
         { inputByWireSize, outputByWireSize, subGraphOutputsByWireSize }) {
        for (auto [key, value] : byWireSize) {
            outFile << VertexUtils::vertexTypeToString(value.back()->getType())
                    << " ";
            if (key) {
                outFile << "[" << key << " : 0] ";
            }

            for (int i = 0; i < value.size() - 1; ++i) {
                outFile << value[i]->getName() << ", ";
            }
            outFile << value.back()->getName() << ";\n" << verilogTab;
        }
    }

    if (vertexes[VertexType::Const].size()) {
        outFile << "\n";
    }
    // writing consts
    for (auto oper : vertexes[VertexType::Const]) {
        outFile << verilogTab << oper->toVerilog() << "\n";
    }

    if (vertexes[VertexType::Graph].size()) {
        outFile << "\n";
    }
    // and all modules
    for (auto sub : vertexes[VertexType::Graph]) {
        auto subPtr = std::static_pointer_cast<OrientedGraph>(sub);
        subPtr->setCurrentParent(shared_from_this());
        outFile << sub->toVerilog();
    }

    if (vertexes[VertexType::Operation].size()) {
        outFile << "\n";
    }
    // and all operations
    for (auto oper : vertexes[VertexType::Operation]) {
        outFile << verilogTab << oper->toVerilog() << "\n";
    }

    outFile << "\n";
    // and all outputs
    for (auto oper : vertexes[VertexType::Output]) {
        outFile << verilogTab << oper->toVerilog() << "\n";
    }

    outFile << "endmodule";

    alreadyParsed = true;

    if (parentGraphs.size()) {
        return curInstToString();
    }

    return "";
}
