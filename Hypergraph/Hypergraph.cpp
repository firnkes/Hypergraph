#include "Hypergraph.h"

#include <algorithm>
#include <assert.h>
#include <boost/functional/hash.hpp>

template <class T>
T getElement(std::vector<T> &v, int id) {
    auto it = std::find_if(v.begin(), v.end(), [id](T &n) {return n.id == id; });
    assert(it != v.end() && "Not existing id.");

    return *it;
}

template <class T>
void removeId(std::vector<T> &v, int id) {
    auto it = std::find_if(v.begin(), v.end(), [id](T &n) {return n.id == id; });
    assert(it != v.end() && "Not existing id.");

    v.erase(it);
}

template <class T>
bool containsId(std::vector<T> &v, int id) {
    return std::find_if(v.begin(), v.end(), [id](T &n) {return n.id == id; }) != v.end();
}

template <class T>
int getNextId(std::vector<T> &v) {
    if (v.empty()) {
        return 0;
    }
    return v.back().id + 1;
}


Hypergraph::Hypergraph(bool weightedNodes, bool weightedEdges) : weightedNodes(weightedNodes), weightedEdges(weightedEdges) {}


NodeId Hypergraph::addNode(NodeWeight weight)
{
    NodeId id = getNextId(nodes);
    nodes.push_back(HNode(id, weight));

    return id;
}

HyperedgeId Hypergraph::addEdge(HyperedgeVector nodeIds, HyperedgeWeight weight)
{
    assert(!nodeIds.empty() && "Edges with no containing nodes are not allowed.");
    std::sort(nodeIds.begin(), nodeIds.end());

    for (int nId : nodeIds) {
        assert(containsId(nodes, nId) && "Node contained by edge must be added before.");
    }

    size_t fingerprint = 0;
    boost::hash_range(fingerprint, nodeIds.begin(), nodeIds.end());
    for (auto &edge : edges) {
        assert(edge.fingerprint != fingerprint && "Duplicate edge dedected.");
    }

    HyperedgeId id = getNextId(edges);
    edges.push_back(HEdge(id, std::move(nodeIds), weight, fingerprint));

    return id;
}

bool Hypergraph::containsNode(NodeId id)
{
    return containsId(nodes, id);
}

bool Hypergraph::containsEdge(HyperedgeId id)
{
    return containsId(edges, id);
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


Hypergraph::HEdge::HEdge(HyperedgeId id, HyperedgeVector nodeIds, HyperedgeWeight weight, HyperedgeHash fingerprint) : id(id), weight(weight), nodeIds(std::move(nodeIds)), fingerprint
(fingerprint) {};

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
    for (auto &edge : edges) {

        if (weightedEdges) {
            os << edge.weight << " ";
        }
        for (auto &id : edge.nodeIds) {
            // Metis - File - Format requires that the ids of the nodes are numbered from 0.. | V | -1.
            os << id << " ";
        }
        os << std::endl;
    }

    /*
    If weights on nodes are enabled,| V | lines are appended to the input file, containing the weight of the | V | vertices.
    */
    if (weightedNodes) {
        for (auto &node : nodes) {
            os << node.weight << std::endl;
        }
    }
}

