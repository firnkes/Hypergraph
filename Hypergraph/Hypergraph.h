#pragma once

#include <vector>
#include <map>
#include <iostream>

using NodeId = int;
using NodeWeight = int;
using HyperedgeId = int;
using HyperedgeWeight = int;
using HyperedgeVector = std::vector<NodeId>;

class Hypergraph
{
    struct HNode {
        NodeId id;
        NodeWeight weight;

        HNode(NodeId id, NodeWeight weight);
    };


    struct HEdge {
        HyperedgeId id;
        HyperedgeWeight weight;
        HyperedgeVector nodeIds;

        HEdge(int id, std::vector<int> nodeIds, int weight);
    };

    std::map<int, HNode> nodes;
    std::map<int, HEdge> edges;

public:
    bool weightedNodes;
    bool weightedEdges;

    Hypergraph(bool weightedNodes = false, bool weightedEdges = false);

    void addNode(NodeId id, NodeWeight weight = 0);
    // Nodes that are part of an edge must be added before the edge can be created.
    void addEdge(HyperedgeId id, HyperedgeVector nodeIds, HyperedgeWeight weight = 0);

    void removeNode(NodeId id);
    void removeEdge(HyperedgeId id);

    bool containsNode(NodeId id);
    bool containsEdge(HyperedgeId id);

    NodeWeight getNodeWeight(NodeId id);
    HyperedgeWeight getEdgeWeight(HyperedgeId id);

    HyperedgeVector getContainedNodeIds(HyperedgeId id);

    void exportToHMetis(std::ostream &os);

};
