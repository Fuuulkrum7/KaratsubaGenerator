#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "OrientedGraph.h"

uint64_t OrientedGraph::count = 0;

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

OrientedGraph::OrientedGraph(OrientedGraph *other) {
    type = other->type;
    level = other->level;
    name = other->name;

    parentGraphs = other->parentGraphs;
    currentParentGraph = other->currentParentGraph;
    vertices = other->vertices;

    subGraphs = other->subGraphs;
    subGraphsOutputsPtr = other->subGraphsOutputsPtr;
    allSubGraphsOutputs = other->allSubGraphsOutputs;
    subGraphsInputsPtr = other->subGraphsInputsPtr;

    alreadyParsed = other->alreadyParsed;
    graphInstanceCount = other->graphInstanceCount;
    graphInstanceToVerilogCount = other->graphInstanceToVerilogCount;

    parentCount = other->parentCount;
    path = other->path;
}

void OrientedGraph::setCountGraphs(uint64_t n) {
    count = n;
}

uint64_t OrientedGraph::getCountGraphs() {
    return count;
}

std::set<GraphPtr> OrientedGraph::getParents() const { return parentGraphs; }

void OrientedGraph::setDefaultName(std::string name) { defaultName = name; }

std::string OrientedGraph::getDefaultName() { return defaultName; }

void OrientedGraph::addParent(GraphPtr parent) { parentGraphs.insert(parent); }

std::vector<VertexPtr> OrientedGraph::getVertexesByType(VertexType type) const {
    return vertices.at(type);
}

VertexPtr OrientedGraph::addInput(uint64_t upper, uint64_t lower) {
    VertexPtr newVertex(new GraphVertex(VertexType::Input,
                                        OperationType::Default, upper, lower));
    vertices[VertexType::Input].push_back(newVertex);

    return newVertex;
}

VertexPtr OrientedGraph::addOutput(uint64_t upper, uint64_t lower) {
    VertexPtr newVertex(new GraphVertex(VertexType::Output,
                                        OperationType::Default, upper, lower));
    vertices[VertexType::Output].push_back(newVertex);

    return newVertex;
}

VertexPtr OrientedGraph::addConst(int value) {
    VertexPtr newVertex(new GraphVertexConst(value));
    vertices[VertexType::Input].push_back(newVertex);

    return newVertex;
}

std::vector<VertexPtr>
OrientedGraph::addSubgraph(GraphPtr subGraph, std::vector<VertexPtr> inputs) {
    std::vector<VertexPtr> iGraph =
        subGraph->getVertexesByType(VertexType::Input);

    subGraph->addParent(shared_from_this());
    subGraph->setCurrentParent(shared_from_this());

    if (inputs.size() != iGraph.size()) {
        throw std::invalid_argument(
            "Number of inputs should be same, as subgrap inputs number");
    }

    // check if one of inputs is incorrect
    for (size_t i = 0; i < inputs.size(); ++i) {
        // size of input like wire
        uint64_t size_inp = inputs[i]->getWireSize();
        // size of input from subgraph
        uint64_t size_grph = iGraph[i]->getWireSize();
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

    ++(*val);

    // here we use subgraph like an instance of BasicType,
    // and we call it's toVerilog, having in multiple instance
    // of one subGraph, so we can have many times "moduleName name (inp, out);"
    // having different names of module, inputs and outputs
    vertices[VertexType::Graph].push_back(subGraph);
    subGraphs.insert(subGraph);

    return outputs;
}

VertexPtr OrientedGraph::addOperation(OperationType type, uint64_t upper,
                                      uint64_t lower, uint64_t shift) {
    VertexPtr newVertex;

    if (type == OperationType::RShift || type == OperationType::LShift) {
        newVertex.reset(new GraphVertexShift(type, shift, upper, lower));
    } else {
        newVertex.reset(
            new GraphVertex(VertexType::Operation, type, upper, lower));
    }

    vertices[VertexType::Operation].push_back(newVertex);

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
    if (this->path != path) {
        this->path = path;

        // we will stop, when subgraph would not have any subgraphs
        for (auto i : subGraphs) {
            i->setWritePath(path);
        }
    }
}

uint64_t OrientedGraph::getWireSize() const {
    return vertices.at(VertexType::Input).size() +
           vertices.at(VertexType::Output).size();
}

void OrientedGraph::setCurrentParent(GraphPtr parent) {
    currentParentGraph = parent;
}

void OrientedGraph::resetCounters(GraphPtr where) {
    graphInstanceToVerilogCount[where] = 0;
}

// for parsing graph to module instance
std::string OrientedGraph::getInstance() {
    uint64_t *verilogCount = &graphInstanceToVerilogCount[currentParentGraph];
    uint64_t *allCount = &graphInstanceCount[currentParentGraph];

    if (*verilogCount == *allCount) {
        throw std::out_of_range("Incorrect getInstance call. All modules (" +
                                std::to_string(*allCount) +
                                ") were already parsed");
    }

    std::string verilogTab = "  ";
    // module_name module_name_inst_1 (
    std::string module_ver = verilogTab + name + " " + name + "_inst_" +
                             std::to_string(*verilogCount) + " (\n";
    for (size_t i = 0; i < vertices[VertexType::Input].size(); ++i) {
        auto inp = subGraphsInputsPtr[currentParentGraph][*verilogCount][i];
        std::string inp_name = vertices[VertexType::Input][i]->getName();

        module_ver += verilogTab + verilogTab + "." + inp_name + "( ";
        module_ver += inp->getName() + " ),\n";
    }

    for (size_t i = 0; i < vertices[VertexType::Output].size() - 1; ++i) {
        VertexPtr out =
            subGraphsOutputsPtr[currentParentGraph][*verilogCount][i];
        std::string out_name = vertices[VertexType::Output][i]->getName();

        module_ver += verilogTab + verilogTab + "." + out_name + "( ";
        module_ver += out->getName() + " ),\n";
    }

    std::string out_name = vertices[VertexType::Output].back()->getName();

    module_ver += verilogTab + verilogTab + "." + out_name + "( ";
    module_ver += subGraphsOutputsPtr[currentParentGraph][*verilogCount]
                      .back()
                      ->getName() +
                  " )\n";
    module_ver += verilogTab + "); \n";

    ++(*verilogCount);

    return module_ver;
}

std::string OrientedGraph::toVerilog() {
    std::string verilogTab = "  ";
    // if it's a subGraph and it was already parsed to .v file
    // we can just return a new instance of module
    if (alreadyParsed && parentGraphs.size()) {
        return getInstance();
    }
    std::string curPath = path;
    if (curPath.back() != '/') {
        curPath += "/";
    }

    if (parentGraphs.size()) {
        curPath += "submodules";
    }
    std::filesystem::create_directories(curPath);

    std::ofstream outFile(curPath + "/" + name + ".v");
    outFile << "module " << name << "(\n" << verilogTab;

    std::map<uint64_t, std::vector<VertexPtr>> inputByWireSize;
    std::map<uint64_t, std::vector<VertexPtr>> outputByWireSize;
    std::map<uint64_t, std::vector<VertexPtr>> subGraphOutputsByWireSize;

    // here we are parsing inputs by their wire size
    for (auto inp : vertices[VertexType::Input]) {
        if (!inputByWireSize.count(inp->getWireSize())) {
            inputByWireSize[inp->getWireSize()] = {};
        }

        // check if key exists
        inputByWireSize[inp->getWireSize()].push_back(inp);

        outFile << inp->getName() << ", ";
    }
    outFile << '\n' << verilogTab;

    // and outputs
    for (auto outVert : vertices[VertexType::Output]) {
        if (!outputByWireSize.count(outVert->getWireSize())) {
            outputByWireSize[outVert->getWireSize()] = {};
        }

        outputByWireSize[outVert->getWireSize()].push_back(outVert);

        outFile << outVert->getName()
                << ((outVert == vertices[VertexType::Output].back()) ? "\n"
                                                                     : ", ");
    }
    outFile << ");\n" << verilogTab;

    // writing wires for modules
    // i thought it would look better
    for (auto outVert : allSubGraphsOutputs) {
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

            for (size_t i = 0; i < value.size() - 1; ++i) {
                outFile << value[i]->getName() << ", ";
            }
            outFile << value.back()->getName() << ";\n" << verilogTab;
        }
    }

    if (vertices[VertexType::Const].size()) {
        outFile << "\n";
    }
    // writing consts
    for (auto oper : vertices[VertexType::Const]) {
        outFile << verilogTab << oper->toVerilog() << "\n";
    }

    if (vertices[VertexType::Graph].size()) {
        outFile << "\n";
    }
    // and all modules
    for (auto sub : vertices[VertexType::Graph]) {
        auto subPtr = std::static_pointer_cast<OrientedGraph>(sub);
        subPtr->setCurrentParent(shared_from_this());
        outFile << sub->toVerilog();
    }

    if (vertices[VertexType::Operation].size()) {
        outFile << "\n";
    }
    // and all operations
    for (auto oper : vertices[VertexType::Operation]) {
        outFile << verilogTab << oper->getInstance() << "\n";
        outFile << verilogTab << oper->toVerilog() << "\n";
    }

    outFile << "\n";
    // and all outputs
    for (auto oper : vertices[VertexType::Output]) {
        outFile << verilogTab << oper->toVerilog() << "\n";
    }

    outFile << "endmodule";

    alreadyParsed = true;

    if (parentGraphs.size()) {
        return getInstance();
    }

    return "";
}
