#include "pch.h"
#include "../Hypergraph/Hypergraph.cpp"
#include "../Hypergraph/Hypergraph.h"

class HypergraphTest : public ::testing::Test {

protected:

    HypergraphTest() : hyper() { }
    Hypergraph hyper;

};

TEST_F(HypergraphTest, CorrectInitDefault) {
    auto hyper_default = Hypergraph();

    EXPECT_FALSE(hyper_default.weightedNodes);
    EXPECT_FALSE(hyper_default.weightedEdges);
}

TEST_F(HypergraphTest, CorrectInitUnweighted) {
    auto hyper_false = Hypergraph(false, false);

    EXPECT_FALSE(hyper_false.weightedNodes);
    EXPECT_FALSE(hyper_false.weightedEdges);
}
TEST_F(HypergraphTest, CorrectInitWeightedNodes) {
    auto hyper_true = Hypergraph(true, true);

    EXPECT_TRUE(hyper_true.weightedNodes);
    EXPECT_TRUE(hyper_true.weightedEdges);
}

TEST_F(HypergraphTest, CorrectInitWeightedEdges) {
    auto hyper_mixed = Hypergraph(true, false);

    EXPECT_TRUE(hyper_mixed.weightedNodes);
    EXPECT_FALSE(hyper_mixed.weightedEdges);
}


TEST_F(HypergraphTest, addUnweightedNode) {
    auto result = hyper.addNode(0);

    EXPECT_EQ(0, result);
    EXPECT_TRUE(hyper.containsNode(result));
}

TEST_F(HypergraphTest, addWeightedNode) {
    auto result = hyper.addNode(1);

    EXPECT_EQ(0, result);
    EXPECT_TRUE(hyper.containsNode(result));
    EXPECT_EQ(1, hyper.getNodeWeight(result));
}



TEST_F(HypergraphTest, addUnweightedEdge) {
    hyper.addNode(1);
    hyper.addNode(2);

    auto vec = std::vector<int>{ 0, 1 };
    auto result = hyper.addEdge(vec);

    EXPECT_EQ(0, result);
    EXPECT_TRUE(hyper.containsEdge(result));
    EXPECT_EQ(vec, hyper.getContainedNodeIds(result));
}


TEST_F(HypergraphTest, addWeightedEdge) {
    hyper.addNode(1);
    hyper.addNode(2);

    auto vec = std::vector<int>{ 0, 1 };
    hyper.addEdge(vec, 1);

    EXPECT_TRUE(hyper.containsEdge(0));
    EXPECT_EQ(1, hyper.getEdgeWeight(0));
    EXPECT_EQ(vec, hyper.getContainedNodeIds(0));
}

TEST_F(HypergraphTest, addDuplicateEdge) {
    hyper.addNode(1);
    hyper.addNode(2);

    hyper.addEdge(std::vector<int>{1, 0}, 1);
    EXPECT_DEATH(hyper.addEdge(std::vector<int>{1, 0}, 0), "");
}


TEST_F(HypergraphTest, addDuplicateEdgeOtherOrder) {
    hyper.addNode(1);
    hyper.addNode(2);

    hyper.addEdge(std::vector<int>{1, 0}, 1);
    EXPECT_DEATH(hyper.addEdge(std::vector<int>{0, 1}), "");
}


TEST_F(HypergraphTest, addEdgeWithNotExistingNode) {
    hyper.addNode(1);

    EXPECT_DEATH(hyper.addEdge(std::vector<int>{1, 2}), "");
}

TEST_F(HypergraphTest, containsNode) {
    hyper.addNode(0);

    EXPECT_TRUE(hyper.containsNode(0));
}

TEST_F(HypergraphTest, doesNotContainNode) {
    hyper.addNode(0);

    EXPECT_FALSE(hyper.containsNode(1));
}


TEST_F(HypergraphTest, containsEdge) {
    hyper.addNode();
    hyper.addNode();
    hyper.addEdge(std::vector<int>{1, 0});

    EXPECT_TRUE(hyper.containsEdge(0));
}

TEST_F(HypergraphTest, doesNotContainEdge) {
    hyper.addNode();
    hyper.addNode();
    hyper.addEdge(std::vector<int>{1, 0});

    EXPECT_FALSE(hyper.containsEdge(1));
}


TEST_F(HypergraphTest, WritesToHMetisFileFormatWithUnweightedNodesAndEdges) {
    auto node0 = hyper.addNode();
    auto node1 = hyper.addNode();
    auto node2 = hyper.addNode();
    auto node3 = hyper.addNode();
    auto node4 = hyper.addNode();
    auto node5 = hyper.addNode();
    auto node6 = hyper.addNode();


    hyper.addEdge(std::vector<int>{node1, node2});
    hyper.addEdge(std::vector<int>{node2, node3, node4});
    hyper.addEdge(std::vector<int>{node5, node6, node4});
    hyper.addEdge(std::vector<int>{node1, node0, node5, node6});

    std::stringstream stream;
    hyper.exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "4 7 \n"
        "1 2 \n"
        "2 3 4 \n"
        "4 5 6 \n"
        "0 1 5 6 \n"
    );

    EXPECT_EQ(expected, result);
}

TEST_F(HypergraphTest, WritesToHMetisFileFormatWithUnweightedNodesAndWeightedEdges) {
    hyper.weightedEdges = true;

    auto node0 = hyper.addNode();
    auto node1 = hyper.addNode();
    auto node2 = hyper.addNode();
    auto node3 = hyper.addNode();
    auto node4 = hyper.addNode();
    auto node5 = hyper.addNode();
    auto node6 = hyper.addNode();

    hyper.addEdge(std::vector<int>{node1, node2}, 2);
    hyper.addEdge(std::vector<int>{node1, node0, node5, node6}, 3);
    hyper.addEdge(std::vector<int>{node5, node6, node4}, 8);
    hyper.addEdge(std::vector<int>{node2, node3, node4}, 7);

    std::stringstream stream;
    hyper.exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "4 7 1\n"
        "2 1 2 \n"
        "3 0 1 5 6 \n"
        "8 4 5 6 \n"
        "7 2 3 4 \n"
    );

    EXPECT_EQ(expected, result);
}

TEST_F(HypergraphTest, WritesToHMetisFileFormatWithWeightedNodesAndUnweightedEdges) {
    hyper.weightedNodes = true;

    auto node0 = hyper.addNode(3);
    auto node1 = hyper.addNode(5);
    auto node2 = hyper.addNode(1);
    auto node3 = hyper.addNode(8);
    auto node4 = hyper.addNode(7);
    auto node5 = hyper.addNode(3);
    auto node6 = hyper.addNode(9);

    hyper.addEdge(std::vector<int>{node1, node2});
    hyper.addEdge(std::vector<int>{node2, node3, node4});
    hyper.addEdge(std::vector<int>{node5, node6, node4});
    hyper.addEdge(std::vector<int>{node1, node0, node5, node6});

    std::stringstream stream;
    hyper.exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "4 7 10\n"
        "1 2 \n"
        "2 3 4 \n"
        "4 5 6 \n"
        "0 1 5 6 \n"
        "3\n"
        "5\n"
        "1\n"
        "8\n"
        "7\n"
        "3\n"
        "9\n"
    );

    EXPECT_EQ(expected, result);
}


TEST_F(HypergraphTest, WritesToHMetisFileFormatWithWeightedNodesAndEdges) {
    hyper.weightedNodes = true;
    hyper.weightedEdges = true;

    auto node0 = hyper.addNode(3);
    auto node1 = hyper.addNode(5);
    auto node2 = hyper.addNode(1);
    auto node3 = hyper.addNode(8);
    auto node4 = hyper.addNode(7);
    auto node5 = hyper.addNode(9);
    auto node6 = hyper.addNode(3);

    hyper.addEdge(std::vector<int>{node1, node2}, 2);
    hyper.addEdge(std::vector<int>{node1, node0, node5, node6}, 3);
    hyper.addEdge(std::vector<int>{node5, node6, node4}, 8);
    hyper.addEdge(std::vector<int>{node2, node3, node4}, 7);

    std::stringstream stream;
    hyper.exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "4 7 11\n"
        "2 1 2 \n"
        "3 0 1 5 6 \n"
        "8 4 5 6 \n"
        "7 2 3 4 \n"
        "3\n"
        "5\n"
        "1\n"
        "8\n"
        "7\n"
        "9\n"
        "3\n"
    );

    EXPECT_EQ(expected, result);
}


TEST_F(HypergraphTest, WritesToHMetisFileFormatWithUnweightedGraphButWeightedNodesAndEdgesAdded) {
    auto node0 = hyper.addNode(3);
    auto node1 = hyper.addNode(5);
    hyper.addEdge(std::vector<int>{node1, node0}, 2);


    std::stringstream stream;
    hyper.exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "1 2 \n"
        "0 1 \n"
    );

    EXPECT_EQ(expected, result);
}
