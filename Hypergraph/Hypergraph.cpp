#include "Hypergraph.h"

#include <algorithm>
#include <assert.h>

template <class T>
T getElement(std::map<int, T> &v, int id) {
    auto it = v.find(id);
    assert(it != v.end() && "Not existing id.");

    return it->second;
}

template <class T>
void removeID(std::map<int, T> &v, int id) {
    auto it = v.find(id);
    assert(it != v.end() && "Not existing id.");

    v.erase(it);
}

template <class T>
bool containsID(std::map<int, T> &v, int id) {
    return v.find(id) != v.end();
}


Hypergraph::Hypergraph(bool weightedNodes, bool weightedEdges) : weightedNodes(weightedNodes), weightedEdges(weightedEdges) {}


void Hypergraph::addNode(NodeId id, NodeWeight weight)
{
    bool success = nodes.insert(std::pair<int, HNode>(id, HNode(id, weight))).second;
    assert(success && "Duplicate id.");
}

void Hypergraph::addEdge(HyperedgeId id, HyperedgeVector nodeIds, HyperedgeWeight weight)
{
    assert(!nodeIds.empty() && "Edges with no containing nodes are not allowed.");

    for (int nId : nodeIds) {
        assert(nodes.find(nId) != nodes.end() && "Node contained by edge must be added before.");
    }

    bool success = edges.insert(std::pair<int, HEdge>(id, HEdge(id, std::move(nodeIds), weight))).second;
    assert(success && "Duplicate id.");
}

void Hypergraph::removeNode(NodeId id)
{
    auto iteratorMap = edges.begin();
    while (iteratorMap != edges.end()) {
        auto &edge = iteratorMap->second;
        auto iteratorId = std::find(edge.nodeIds.begin(), edge.nodeIds.end(), id);

        if (iteratorId != edge.nodeIds.end()) {
            if (edge.nodeIds.size() == 1) {
                iteratorMap = edges.erase(iteratorMap);
                continue;
            }
            else {
                iter_swap(iteratorId, edge.nodeIds.end() - 1);
                edge.nodeIds.pop_back();
            }
        }
        iteratorMap++;
    }
    removeID(nodes, id);
}

void Hypergraph::removeEdge(HyperedgeId id)
{
    removeID(edges, id);
}

bool Hypergraph::containsNode(NodeId id)
{
    return containsID(nodes, id);
}

bool Hypergraph::containsEdge(HyperedgeId id)
{
    return containsID(edges, id);
}

NodeWeight Hypergraph::getNodeWeight(NodeId id)
{
    return getElement(nodes, id).weight;
}

HyperedgeWeight Hypergraph::getEdgeWeight(HyperedgeId id)
{
    return getElement(edges, id).weight;
}


Hypergraph::HNode::HNode(NodeId id, NodeWeight weight) : id(id), weight(weight) {}


Hypergraph::HEdge::HEdge(HyperedgeId id, HyperedgeVector nodeIds, HyperedgeWeight weight) : id(id), weight(weight), nodeIds(std::move(nodeIds)) {};

HyperedgeVector Hypergraph::getContainedNodeIds(HyperedgeId id)
{
    return getElement(edges, id).nodeIds;
}

void Hypergraph::exportToHMetis(std::ostream &os)
{
    /*
    The first line contains either two or three integers. The first integer is the number of hyperedges (|Eh|), the second
    is the number of vertices (|V|), and the third integer (fmt) contains information about the type of the hypergraph. In
    particular, depending on the value of fmt, the hypergraph H can have weights on either the hyperedges, the vertices,
    or both. In the case that H is unweighted (i.e., all the hyperedges and vertices have the same weight), fmt is omitted.
    */
    auto fmt = "";
    if (weightedEdges && weightedNodes) {
        fmt = "11";
    }
    else if (weightedNodes) {
        fmt = "10";
    }
    else if (weightedEdges) {
        fmt = "1";
    }
    os << edges.size() << " " << nodes.size() << " " << fmt << std::endl;

    /*
    After this first line, the remaining |Eh| lines store the vertices contained in each hyperedge–one line per hyperedge. In
    particular, the i th line (excluding comment lines) contains the vertices that are included in the (i−1)th hyperedge.
    The first integer in each line contains the weight of the respective hyperedge.
    */
    for (auto &entry : edges) {
        auto &edge = entry.second;

        if (weightedEdges) {
            os << edge.weight << " ";
        }
        for (auto &id : edge.nodeIds) {
            os << id << " ";
        }
        os << std::endl;
    }

    /*
    If weights on nodes are enabled,| V | lines are appended to the input file, containing the weight of the | V | vertices.
    */
    if (weightedNodes) {
        for (auto &entry : nodes) {
            auto &node = entry.second;

            os << node.weight << std::endl;
        }
    }
}

