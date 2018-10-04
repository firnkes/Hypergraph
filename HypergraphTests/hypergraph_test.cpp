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
    hyper.addNode(0);

    EXPECT_TRUE(hyper.containsNode(0));
}

TEST_F(HypergraphTest, addWeightedNode) {
    hyper.addNode(0, 1);

    EXPECT_TRUE(hyper.containsNode(0));
    EXPECT_EQ(hyper.getNodeWeight(0), 1);
}

TEST_F(HypergraphTest, addDuplicateNodeID) {
    hyper.addNode(0, 1);

    EXPECT_DEATH(hyper.addNode(0, 2), "");
}


TEST_F(HypergraphTest, addUnweightedEdge) {
    hyper.addNode(1);
    hyper.addNode(2);

    auto vec = std::vector<int>{ 1, 2 };
    hyper.addEdge(0, vec);

    EXPECT_TRUE(hyper.containsEdge(0));
    EXPECT_EQ(hyper.getContainedNodeIds(0), vec);

}


TEST_F(HypergraphTest, addWeightedEdge) {
    hyper.addNode(1);
    hyper.addNode(2);

    auto vec = std::vector<int>{ 1, 2 };
    hyper.addEdge(0, vec, 1);

    EXPECT_TRUE(hyper.containsEdge(0));
    EXPECT_EQ(hyper.getEdgeWeight(0), 1);
    EXPECT_EQ(hyper.getContainedNodeIds(0), vec);
}

TEST_F(HypergraphTest, addDuplicateIDEdge) {
    hyper.addNode(1);
    hyper.addNode(2);

    hyper.addEdge(0, std::vector<int>{1, 2}, 1);
    EXPECT_DEATH(hyper.addEdge(0, std::vector<int>{2, 3}, 1), "");
}

TEST_F(HypergraphTest, addNotExistingNode) {
    hyper.addNode(1);

    EXPECT_DEATH(hyper.addEdge(0, std::vector<int>{1, 2}), "");
}

TEST_F(HypergraphTest, removeExistingNode) {
    hyper.addNode(0);
    hyper.removeNode(0);

    EXPECT_FALSE(hyper.containsNode(0));
}

TEST_F(HypergraphTest, removeNotExistingNode) {
    EXPECT_DEATH(hyper.removeNode(0), "");
}


TEST_F(HypergraphTest, removeNodeThatIsPartOfEdge) {
    hyper.addNode(1);
    hyper.addNode(2);

    hyper.addEdge(0, std::vector<int>{1, 2}, 1);
    hyper.removeNode(2);

    EXPECT_FALSE(hyper.containsNode(2));
    EXPECT_EQ(hyper.getContainedNodeIds(0), std::vector<int>{1});
}

TEST_F(HypergraphTest, removeExistingEdge) {
    hyper.addNode(1);
    hyper.addNode(2);

    hyper.addEdge(0, std::vector<int>{1, 2});
    hyper.removeEdge(0);

    EXPECT_FALSE(hyper.containsEdge(0));
}

TEST_F(HypergraphTest, removeNotExistingEdge) {
    EXPECT_DEATH(hyper.removeEdge(0), "");
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
    hyper.addNode(1);
    hyper.addNode(2);
    hyper.addEdge(0, std::vector<int>{1, 2});

    EXPECT_TRUE(hyper.containsEdge(0));
}

TEST_F(HypergraphTest, doesNotContainEdge) {
    hyper.addNode(1);
    hyper.addNode(2);
    hyper.addEdge(0, std::vector<int>{1, 2});

    EXPECT_FALSE(hyper.containsEdge(1));
}


TEST_F(HypergraphTest, WritesToHMetisFileFormatWithUnweightedNodesAndEdges) {
    hyper.addNode(1);
    hyper.addNode(2);
    hyper.addNode(3);
    hyper.addNode(4);
    hyper.addNode(5);
    hyper.addNode(6);
    hyper.addNode(7);


    hyper.addEdge(0, std::vector<int>{1, 2});
    hyper.addEdge(1, std::vector<int>{2, 3, 4});
    hyper.addEdge(2, std::vector<int>{5, 6, 4});
    hyper.addEdge(3, std::vector<int>{1, 7, 5, 6});

    std::stringstream stream;
    hyper.exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "4 7 \n"
        "1 2 \n"
        "2 3 4 \n"
        "5 6 4 \n"
        "1 7 5 6 \n"
    );

    EXPECT_EQ(expected, result);
}

TEST_F(HypergraphTest, WritesToHMetisFileFormatWithUnweightedNodesAndWeightedEdges) {
    hyper.weightedEdges = true;

    hyper.addNode(1);
    hyper.addNode(2);
    hyper.addNode(3);
    hyper.addNode(4);
    hyper.addNode(5);
    hyper.addNode(6);
    hyper.addNode(7);


    hyper.addEdge(0, std::vector<int>{1, 2}, 2);
    hyper.addEdge(1, std::vector<int>{1, 7, 5, 6}, 3);
    hyper.addEdge(2, std::vector<int>{5, 6, 4}, 8);
    hyper.addEdge(3, std::vector<int>{2, 3, 4}, 7);

    std::stringstream stream;
    hyper.exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "4 7 1\n"
        "2 1 2 \n"
        "3 1 7 5 6 \n"
        "8 5 6 4 \n"
        "7 2 3 4 \n"
    );

    EXPECT_EQ(expected, result);
}

TEST_F(HypergraphTest, WritesToHMetisFileFormatWithWeightedNodesAndUnweightedEdges) {
    hyper.weightedNodes = true;

    hyper.addNode(1, 5);
    hyper.addNode(2, 1);
    hyper.addNode(3, 8);
    hyper.addNode(4, 7);
    hyper.addNode(5, 3);
    hyper.addNode(6, 9);
    hyper.addNode(7, 3);


    hyper.addEdge(0, std::vector<int>{1, 2});
    hyper.addEdge(1, std::vector<int>{2, 3, 4});
    hyper.addEdge(2, std::vector<int>{5, 6, 4});
    hyper.addEdge(3, std::vector<int>{1, 7, 5, 6});

    std::stringstream stream;
    hyper.exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "4 7 10\n"
        "1 2 \n"
        "2 3 4 \n"
        "5 6 4 \n"
        "1 7 5 6 \n"
        "5\n"
        "1\n"
        "8\n"
        "7\n"
        "3\n"
        "9\n"
        "3\n"
    );

    EXPECT_EQ(expected, result);
}


TEST_F(HypergraphTest, WritesToHMetisFileFormatWithWeightedNodesAndEdges) {
    hyper.weightedNodes = true;
    hyper.weightedEdges = true;

    hyper.addNode(5, 3);
    hyper.addNode(1, 5);
    hyper.addNode(2, 1);
    hyper.addNode(3, 8);
    hyper.addNode(4, 7);
    hyper.addNode(6, 9);
    hyper.addNode(7, 3);


    hyper.addEdge(0, std::vector<int>{1, 2}, 2);
    hyper.addEdge(1, std::vector<int>{1, 7, 5, 6}, 3);
    hyper.addEdge(2, std::vector<int>{5, 6, 4}, 8);
    hyper.addEdge(3, std::vector<int>{2, 3, 4}, 7);

    std::stringstream stream;
    hyper.exportToHMetis(stream);

    std::string result = stream.str();
    auto expected = std::string(
        "4 7 11\n"
        "2 1 2 \n"
        "3 1 7 5 6 \n"
        "8 5 6 4 \n"
        "7 2 3 4 \n"
        "5\n"
        "1\n"
        "8\n"
        "7\n"
        "3\n"
        "9\n"
        "3\n"
    );

    EXPECT_EQ(expected, result);
}
