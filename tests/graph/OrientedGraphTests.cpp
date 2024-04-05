#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <graph/OrientedGraph.h>

std::vector<GraphPtr> test_data_graph;

TEST(graph_test, test_graph_names) {
    GraphVertex::setCountVertex(0);
    std::vector<std::string> names = { "Submodule_0", "Submodule_1",
                                       "TestGraph_2" };

    test_data_graph.push_back(
        std::make_shared<OrientedGraph>(new OrientedGraph()));
    test_data_graph.push_back(
        std::make_shared<OrientedGraph>(new OrientedGraph()));
    OrientedGraph::setDefaultName("TestGraph");
    test_data_graph.push_back(
        std::make_shared<OrientedGraph>(new OrientedGraph()));

    for (int i = 0; i < names.size(); ++i) {
        EXPECT_EQ(names[i], test_data_graph[i]->getName());
    }
}

TEST(graph_test, test_add_vertex) {
    VertexPtr inp_1 = test_data_graph[0]->addInput(1, 0);
    VertexPtr inp_2 = test_data_graph[0]->addInput();

    VertexPtr gate_3 =
        test_data_graph[0]->addOperation(OperationType::Sum, 2, 0);

    VertexPtr out_4 = test_data_graph[0]->addOutput(2, 0);

    test_data_graph[0]->addEdges({ inp_1, inp_2 }, gate_3);
    test_data_graph[0]->addEdge(gate_3, out_4);

    EXPECT_EQ(test_data_graph[0]->getWireSize(), 3);

    EXPECT_EQ(test_data_graph[0]->getVertexesByType(VertexType::Const).size(),
              0);
    EXPECT_EQ(
        test_data_graph[0]->getVertexesByType(VertexType::Operation).size(), 1);

    VertexPtr inp_5 = test_data_graph[1]->addInput(1, 0);
    VertexPtr inp_6 = test_data_graph[1]->addInput(1, 0);

    VertexPtr splt_7 =
        test_data_graph[1]->addOperation(OperationType::SliceOper, 0);

    test_data_graph[1]->addEdge(inp_6, splt_7);

    VertexPtr out_8 = test_data_graph[1]
                          ->addSubgraph(test_data_graph[0], { inp_5, splt_7 })
                          .back();

    VertexPtr dif_9 =
        test_data_graph[1]->addOperation(OperationType::Dif, 2, 0);
    test_data_graph[1]->addEdges({ out_8, inp_5 }, dif_9);

    VertexPtr out_10 = test_data_graph[1]->addOutput(2, 0);
    test_data_graph[1]->addEdge(dif_9, out_10);

    // set parent
    test_data_graph[0]->setCurrentParent(test_data_graph[1]);

    EXPECT_EQ(test_data_graph[0]->getInstance(),
              "  Submodule_0 Submodule_0_inst_0 (\n    .input_0( input_4 ),\n"
              "    .input_1( g_6 ),\n    .output_3( g_7 )\n  ); \n");
    EXPECT_THROW(test_data_graph[0]->getInstance(), std::out_of_range);
}

TEST(graph_test, test_add_used_graph) {

    VertexPtr inp_11 = test_data_graph[1]->addInput(1, 0);
    VertexPtr inp_12 = test_data_graph[1]->addInput(1, 0);

    VertexPtr splt_13 =
        test_data_graph[2]->addOperation(OperationType::SliceOper, 0);

    test_data_graph[2]->addEdge(inp_12, splt_13);

    VertexPtr out_14 =
        test_data_graph[2]
            ->addSubgraph(test_data_graph[0], { inp_11, splt_13 })
            .back();

    VertexPtr dif_15 =
        test_data_graph[2]->addOperation(OperationType::Dif, 2, 0);
    test_data_graph[2]->addEdges({ out_14, inp_11 }, dif_15);

    VertexPtr out_16 = test_data_graph[2]->addOutput(2, 0);
    test_data_graph[2]->addEdge(dif_15, out_16);

    test_data_graph[0]->setCurrentParent(test_data_graph[2]);

    EXPECT_EQ(test_data_graph[0]->getInstance(),
              "  Submodule_0 Submodule_0_inst_0 (\n    .input_0( input_10 ),\n"
              "    .input_1( g_12 ),\n    .output_3( g_13 )\n  ); \n");
    EXPECT_THROW(test_data_graph[0]->getInstance(), std::out_of_range);

    test_data_graph[0]->resetCounters(test_data_graph[2]);
    EXPECT_EQ(test_data_graph[0]->getInstance(),
              "  Submodule_0 Submodule_0_inst_0 (\n    .input_0( input_10 ),\n"
              "    .input_1( g_12 ),\n    .output_3( g_13 )\n  ); \n");
    EXPECT_THROW(test_data_graph[0]->getInstance(), std::out_of_range);
}
