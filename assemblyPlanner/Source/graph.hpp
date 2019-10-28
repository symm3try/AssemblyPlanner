#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <cassert>
#include <cstddef>
#include <iterator> // std::random_access_iterator
#include <vector>
#include <set> 
#include <iostream>
#include <utility> // std::pair
#include <algorithm> // std::fill

#include <string>
#include <unordered_map>
#include <exception>
#include <memory>
#include "node.hpp"
#include "visitor.hpp"

template<typename edgeData, typename nodeData, typename Visitor = VerboseGraphVisitor<std::string> >
class Graph{
public:
    // Construction
    Graph(const Visitor& = Visitor());
    Graph(const std::size_t, const std::size_t, const Visitor& = Visitor());

    // General Information
    std::size_t numberOfNodes() const;
    std::size_t numberOfEdges() const;
    std::size_t numberOfEdgesFromNode(const std::string);
    std::size_t numberOfEdgesToNode(const std::string);

    // Access specific nodes/vertices.
    Edge<nodeData, edgeData> * edgeFromNode(const std::string, const std::size_t) const;
    Edge<nodeData, edgeData> * edgeToNode(const std::string, const std::size_t) const;
    std::vector<Node<nodeData, edgeData>*>& nodesFromNode(const std::string, const std::size_t) const;
    std::vector<Node<nodeData, edgeData>*>& nodesToNode(const std::string, const std::size_t) const;
    // std::pair<bool, std::size_t> findEdge(const std::size_t, const std::size_t) const;

    // manipulation
    // inserttion
    std::size_t insertNode( const std::string, const nodeData);
    std::size_t insertNodes(const std::vector<Node<nodeData, edgeData>*>& );
    std::size_t insertEdge(  const edgeData, const std::string , const std::string);
    std::size_t insertEdges( const edgeData, 
                             const std::string , 
                             const std::vector<std::string> & );
    
    // manipulation
    // removal
    // void eraseNode(const std::string nodeId); 
    // void eraseEdge(const std::size_t);

private:
    std::unordered_map< std::string, Node<nodeData, edgeData>* > nodes_;
    std::vector< Edge<nodeData, edgeData>* > edges_;
    Visitor visitor_;
};

/* Construct a graph
    @param visitor: Visitor to follow changes of integer indices of vertices and edges.
**/
template<typename edgeData, typename nodeData, typename Visitor>
inline 
Graph<edgeData, nodeData, Visitor>::Graph(
    const Visitor& visitor
)
:   nodes_(),
    edges_(),
    visitor_(visitor)
{}

/* Construct a graph with preallocating memory for given Edges.
    @numberOfVertices: Number of vertices.
    @numberOfEdges: Number of edges.
    @visitor: Visitor to follow changes of integer indices of vertices and edges.
**/
template<typename edgeData, typename nodeData, typename Visitor>
inline 
Graph<edgeData, nodeData, Visitor>::Graph(
    const std::size_t numberOfNodes,
    const std::size_t numberOfEdges,
    const Visitor& visitor
)
:   nodes_(),
    edges_(),
    visitor_(visitor)
{
    edges_.reserve(numberOfEdges);
    // visitor_.insertVertices("0", );
}
    
/* Get the number of nodes.
**/
template<typename edgeData, typename nodeData, typename Visitor>
inline std::size_t
Graph<edgeData, nodeData, Visitor>::numberOfNodes() const { 
    return nodes_.size(); 
}

/* Get the number of edges.
*/
template<typename edgeData, typename nodeData, typename Visitor>
inline std::size_t
Graph<edgeData, nodeData, Visitor>::numberOfEdges() const { 
    return edges_.size(); 
}

/* Get the number of edges that originate from a given node.
    @node string-id of a node.
**/
template<typename edgeData, typename nodeData, typename Visitor>
inline std::size_t
Graph<edgeData, nodeData, Visitor>::numberOfEdgesFromNode(
    const std::string node
) { 
    nodes_[node]->numberOfSuccessors();
    return 1;
}

/* Get the number of edges that are incident to a given node.
    @node: string-id of a node.
**/
template<typename edgeData, typename nodeData, typename Visitor>
inline std::size_t
Graph<edgeData, nodeData, Visitor>::numberOfEdgesToNode(
    const std::string node
) { 
    return nodes_[node]->numberOfPredecessors();
}

/* Get the pointer to the j`th edge that originates from a given node.
    @node: string-id of a node.
    @j; number of edge. Between 0 and numberOfedgesFromNode(node) - 1.
**/
template<typename edgeData, typename nodeData, typename Visitor>
inline Edge<nodeData, edgeData>*
Graph<edgeData, nodeData, Visitor>::edgeFromNode(
    const std::string node,
    const std::size_t j
) const {
    return nodes_[node]->children[j];
}

/* Get the pointer to the j`th edge that is incident to a given node.
    @node: string-id of a node.
    @j: index of edge. Between 0 and numberOfedgesToNode(node) - 1.
**/
template<typename edgeData, typename nodeData, typename Visitor>
inline Edge<nodeData, edgeData>*
Graph<edgeData, nodeData, Visitor>::edgeToNode(
    const std::string node,
    const std::size_t j
) const {
    return nodes_[node]->parents[j];
}

/* Get pointers to the nodes reachable from a given node via a specified edge.
    @node: string-id of a node.
    @j: index of the edge being used. Between 0 and numberOfedgesFromNode(node) - 1.
**/
template<typename edgeData, typename nodeData, typename Visitor>
inline std::vector<Node<nodeData, edgeData>*> &
Graph<edgeData, nodeData, Visitor>::nodesFromNode(
    const std::string node,
    const std::size_t j
) const {
    return nodes_[node]->children[j].getSuccessors();
}

/* Get pointers to the nodes incident to a given node via a specified edge.
    @node: string-id of a node.
    @j: index of the edge being used. Between 0 and numberOfedgesFromNode(node) - 1.
**/
template<typename edgeData, typename nodeData, typename Visitor>
inline std::vector<Node<nodeData, edgeData>*> &
Graph<edgeData, nodeData, Visitor>::nodesToNode(
    const std::string node,
    const std::size_t j
) const {
    return nodes_[node]->parents[j].getPredecessors();
}

/* Insert an additional Node.
    @node: string-id of the newly inserted node.
    @data: data asociated with the created node.
    \return: number of nodes present in the graph.
**/
template<typename edgeData, typename nodeData, typename Visitor>
inline std::size_t
Graph<edgeData, nodeData, Visitor>::insertNode(std::string nodeId , nodeData data) {
    Node<nodeData,edgeData> * tempNode = new Node<nodeData,edgeData>(nodeId, data);
    nodes_.insert(std::make_pair(nodeId ,tempNode));
    visitor_.insertVertex(nodeId);
    return nodes_.size() - 1;
}

/* Insert multiple additional Nodes.
    @nodes: vector containing nodes to be inserted into the graph.
    \return: number of nodes present in the graph.
**/
template<typename edgeData, typename nodeData, typename Visitor>
inline std::size_t
Graph<edgeData, nodeData, Visitor>::insertNodes(
    const std::vector<Node<nodeData, edgeData>*>& nodes
) {
    for(auto const node: nodes){
        nodes_.insert(std::make_pair(node->id_, node));
    }
    std::size_t position = nodes_.size();
    for(const auto nd: nodes){
        visitor_.insertVertices(nd->id_, nodes.size());
    }
    return position;
}

/* Insert additional edge.
    @srcNodeId: string-ids of the source nodes of the edges.
    @destNodeId: string-ids of the destinaion nodes of the edges.
    \return Integer index of the newly inserted edge.
**/ 
template<typename edgeData, typename nodeData, typename Visitor>
std::size_t
Graph<edgeData, nodeData, Visitor>::insertEdge(
    const edgeData data,
    const std::string srcNodeId,
    const std::string destNodeId
) {
    Edge<edgeData, nodeData> *edge = new Edge<edgeData, nodeData>(data);
    edges_.push_back(edge);

    if ( nodes_.find(srcNodeId) == nodes_.end() ) {
        std::cerr << "Unable to create edge. " 
                    << "Node " << srcNodeId << " not in graph." << std::endl;
        edges_.pop_back();
        throw std::range_error("Unable to create edge.");
    } else {
        edges_.back()->setSource(nodes_[srcNodeId]);
    }
    nodes_[srcNodeId]->addSuccessor(edges_.back());


    if ( nodes_.find(destNodeId) == nodes_.end() ) {
        std::cerr << "Unable to create edge. " 
                    << "Node" << destNodeId << " not in graph." << std::endl;
        edges_.pop_back();
        throw std::range_error("Unable to create edge.");
    } else {
        edges_.back()->setDestination(nodes_[destNodeId]);
    }
    nodes_[destNodeId]->addPredecessor(edges_.back());

    return edges_.size();
}

/* Insert additional edges.
    @srcNodeId: string-ids of the source nodes of the edges.
    @destNodeId: string-ids of the destinaion nodes of the edges.
    \return Integer index of the newly inserted edge.
**/ 
template<typename edgeData, typename nodeData, typename Visitor>
inline std::size_t
Graph<edgeData, nodeData, Visitor>::insertEdges(
    const edgeData data,
    const std::string srcNodeId,
    const std::vector<std::string> & destNodeId
) {
    for(auto const& dst: destNodeId) {
        insertEdge(data, srcNodeId, dst);
    }
    return edges_.size();
}

/* Erase a Node and all edges concerning this Node.
    @nodeId Integer index of the vertex to be erased.
**/ 
// template<typename edgeData, typename nodeData, typename Visitor>
// void 
// Graph<edgeData, nodeData, Visitor>::eraseNode(
//     const std::string nodeId
// ) {
//     if ( nodes_.find(nodeId) == nodes_.end() ) {
//         std::cerr << "Unable to find node to remove. " 
//                   << "Node: " << nodeId << " not in graph." << std::endl;
//         throw std::range_error;
//     }

//     // erase all edges connected to the vertex
//     while(vertices_[vertexIndex].size() != 0) {
//         eraseEdge(vertices_[vertexIndex].begin()->edge());
//     }

//     if(vertexIndex == numberOfVertices()-1) { // if the last vertex is to be erased        
//         vertices_.pop_back(); // erase vertex
//         visitor_.eraseVertex(vertexIndex);
//     }
//     else { // if a vertex is to be erased which is not the last vertex
//         // move last vertex to the free position:

//         // collect indices of edges affected by the move
//         std::size_t movingVertexIndex = numberOfVertices() - 1;
//         std::set<std::size_t> affectedEdgeIndices;
//         for(Vertex::const_iterator it = vertices_[movingVertexIndex].begin();
//         it != vertices_[movingVertexIndex].end(); ++it) {
//             affectedEdgeIndices.insert(it->edge());
//         }
        
//         // for all affected edges:
//         for(std::set<std::size_t>::const_iterator it = affectedEdgeIndices.begin();
//         it != affectedEdgeIndices.end(); ++it) { 
//             // remove adjacencies
//             eraseAdjacenciesForEdge(*it);

//             // adapt vertex labels
//             for(std::size_t j=0; j<2; ++j) {
//                 if(edges_[*it][j] == movingVertexIndex) {
//                     edges_[*it][j] = vertexIndex;
//                 }
//             }
//             // if(!(edges_[*it].directedness()) && edges_[*it][0] > edges_[*it][1]) {
//             if(edges_[*it][0] > edges_[*it][1]) {
//                 std::swap(edges_[*it][0], edges_[*it][1]);
//             }
//         }

//         // move vertex
//         vertices_[vertexIndex] = vertices_[movingVertexIndex]; // copy
//         vertices_.pop_back(); // erase

//         // insert adjacencies for edges of moved vertex
//         for(std::set<std::size_t>::const_iterator it = affectedEdgeIndices.begin();
//         it != affectedEdgeIndices.end(); ++it) { 
//             insertAdjacenciesForEdge(*it);
//         }

//         visitor_.eraseVertex(vertexIndex);
//         visitor_.relabelVertex(movingVertexIndex, vertexIndex);
//     }
// }

// /// Erase an edge.
// ///
// /// \param edgeIndex Integer index of the edge to be erased.
// /// 
// template<typename edgeData, typename nodeData, typename Visitor>
// inline void 
// Graph<edgeData, nodeData, Visitor>::eraseEdge(
//     const std::size_t edgeIndex
// ) {
//     assert(edgeIndex < numberOfEdges()); 

//     eraseAdjacenciesForEdge(edgeIndex);
//     if(edgeIndex == numberOfEdges() - 1) { // if the last edge is erased
//         edges_.pop_back(); // delete
//         visitor_.eraseEdge(edgeIndex);
//     }
//     else { 
//         std::size_t movingEdgeIndex = numberOfEdges() - 1;
//         eraseAdjacenciesForEdge(movingEdgeIndex);
//         edges_[edgeIndex] = edges_[movingEdgeIndex]; // copy
//         edges_.pop_back(); // delete
//         insertAdjacenciesForEdge(edgeIndex);
//         visitor_.eraseEdge(edgeIndex);
//         visitor_.relabelEdge(movingEdgeIndex, edgeIndex);
//     }
// }

/// Search for an edge (in logarithmic time).
///
/// \param vertex0 first vertex of the edge.
/// \param vertex1 second vertex of the edge.
/// \return if an edge from vertex0 to vertex1 exists, pair.first is true 
///     and pair.second is the index of such an edge. if no edge from vertex0
///     to vertex1 exists, pair.first is false and pair.second is undefined.
///
// template<typename edgeData, typename nodeData, typename Visitor>
// inline std::pair<bool, std::size_t>
// Graph<edgeData, nodeData, Visitor>::findEdge(
//     const std::size_t vertex0,
//     const std::size_t vertex1
// ) const {
//     assert(vertex0 < numberOfVertices());
//     assert(vertex1 < numberOfVertices());
//     std::size_t v0 = vertex0;
//     std::size_t v1 = vertex1;
//     if(numberOfEdgesFromVertex(vertex1) < numberOfEdgesFromVertex(vertex0)) {
//         v0 = vertex1;
//         v1 = vertex0;
//     }
//     VertexIterator it = std::lower_bound(
//         verticesFromVertexBegin(v0),
//         verticesFromVertexEnd(v0),
//         v1
//     ); // binary search
//     if(it != verticesFromVertexEnd(v0) && *it == v1) {
//         // access the corresponding edge in constant time
//         const std::size_t j = std::distance(verticesFromVertexBegin(v0), it);
//         return std::make_pair(true, edgeFromVertex(v0, j));
//     }
//     else {
//         return std::make_pair(false, 0);
//     }
// }

#endif //GRAPH_HPP
