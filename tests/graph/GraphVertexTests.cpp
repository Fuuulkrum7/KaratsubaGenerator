#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <graph/GraphVertex.h>

std::vector<std::shared_ptr<GraphVertex>> test_data;

TEST(vertex_test, test_names) {
    GraphVertex::setCountVertex(0);
    std::vector<std::string> names = { "input_0", "output_1", "input_2",
                                       "g_3",     "const_4",  "g_5" };

    std::shared_ptr<GraphVertex> ptr(new GraphVertex(VertexType::Input));
    test_data.push_back(ptr);

    ptr.reset(new GraphVertex(VertexType::Output));
    test_data.push_back(ptr);

    ptr.reset(new GraphVertex(VertexType::Input));
    test_data.push_back(ptr);

    ptr.reset(new GraphVertex(VertexType::Operation, OperationType::Not));
    test_data.push_back(ptr);

    ptr.reset(new GraphVertex(VertexType::Const));
    test_data.push_back(ptr);
    
    ptr.reset(new GraphVertexShift(OperationType::LShift, 1, 1, 0));
    test_data.push_back(ptr);

    for (int i = 0; i < names.size(); ++i) {
        EXPECT_EQ(names[i], test_data[i]->getName());
    }
}

TEST(vertex_test, test_wire_info_and_size) {
    int up = 10;
    int low = 5;
    std::shared_ptr<GraphVertex> vert(
        new GraphVertex(VertexType::Operation, OperationType::Or, up, low));

    EXPECT_EQ(vert->getWireSize(), up - low);
    EXPECT_EQ(vert->getLower(), low);
    EXPECT_EQ(vert->getUpper(), up);
    EXPECT_EQ(vert->getInstance(), "wire [5 : 0] g_6;");
}

TEST(vertex_test, test_to_verilog) {
    EXPECT_EQ(test_data[3]->getInstance(), "wire g_3;");

    EXPECT_EQ(test_data[0]->toVerilog(), "");

    test_data[1]->addParent(test_data[0]);
    test_data[3]->addParent(test_data[0]);
    test_data[5]->addParent(test_data[0]);

    EXPECT_EQ(test_data[1]->toVerilog(), "assign output_1 = input_0;");
    EXPECT_EQ(test_data[3]->toVerilog(), "assign g_3 = ~input_0;");

    EXPECT_EQ(test_data[5]->toVerilog(), "assign g_5 = input_0 << 1;");

    test_data[5]->setOperation(OperationType::RShift);
    EXPECT_EQ(test_data[5]->toVerilog(), "assign g_5 = input_0 >> 1;");

    test_data[3]->setOperation(OperationType::Buf);
    EXPECT_EQ(test_data[3]->toVerilog(), "assign g_3 = input_0;");

    test_data[3]->setOperation(OperationType::SliceOper);
    EXPECT_EQ(test_data[3]->toVerilog(), "assign g_3 = input_0[0];");

    test_data[3]->setOperation(OperationType::Sum);
    test_data[3]->addParent(test_data[2]);
    EXPECT_EQ(test_data[3]->toVerilog(), "assign g_3 = input_0 + input_2;");

    test_data[3]->setOperation(OperationType::Dif);
    EXPECT_EQ(test_data[3]->toVerilog(), "assign g_3 = input_0 - input_2;");

    test_data[3]->setOperation(OperationType::And);
    EXPECT_EQ(test_data[3]->toVerilog(), "assign g_3 = input_0 & input_2;");

    test_data[3]->setOperation(OperationType::Or);
    EXPECT_EQ(test_data[3]->toVerilog(), "assign g_3 = input_0 | input_2;");

    test_data[3]->setOperation(OperationType::Xor);
    EXPECT_EQ(test_data[3]->toVerilog(), "assign g_3 = input_0 ^ input_2;");

    test_data[3]->setOperation(OperationType::Nand);
    EXPECT_EQ(test_data[3]->toVerilog(),
              "assign g_3 = ~ ( input_0 & input_2 );");

    test_data[3]->setOperation(OperationType::Nor);
    EXPECT_EQ(test_data[3]->toVerilog(),
              "assign g_3 = ~ ( input_0 | input_2 );");

    test_data[3]->setOperation(OperationType::Xnor);
    EXPECT_EQ(test_data[3]->toVerilog(),
              "assign g_3 = ~ ( input_0 ^ input_2 );");
}
