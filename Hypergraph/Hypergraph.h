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

        HEdge(HyperedgeId id, HyperedgeVector nodeIds, HyperedgeWeight weight);
    };

    std::vector<HNode> nodes;
    std::vector<HEdge> edges;

public:
    bool weightedNodes;
    bool weightedEdges;

    Hypergraph(bool weightedNodes = false, bool weightedEdges = false);

    NodeId addNode(NodeWeight weight = 0);
    // Nodes that are part of an edge must be added before the edge can be created.
    HyperedgeId addEdge(HyperedgeVector nodeIds, HyperedgeWeight weight = 0);

    bool containsNode(NodeId id);
    bool containsEdge(HyperedgeId id);

    NodeWeight getNodeWeight(NodeId id);
    HyperedgeWeight getEdgeWeight(HyperedgeId id);

    HyperedgeVector getContainedNodeIds(HyperedgeId id);

    void exportToHMetis(std::ostream &os);

};
