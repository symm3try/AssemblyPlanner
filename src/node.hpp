#pragma once

#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
#include "edge.hpp"

/* Class representing the Nodes within a graph. 
    Nodes can represent any type of data-structure.
    Their content can be specified inside the NodeData-class which can be extended with any structure.
    Nodes are connected with Edges. 
**/
class Node
{
public:
    Node(std::size_t identifier, NodeData data);

    bool hasSuccessor() const;
    bool hasPredecessor() const;
    std::size_t numberOfSuccessors() const;
    std::size_t numberOfPredecessors() const;

    void addPredecessor(Edge *);
    void addSuccessor(Edge *);
    void removePredecessor(std::size_t);
    void removeSuccessor(std::size_t);

    Edge *getSuccessor(std::size_t) const;
    Edge *getPredecessor(std::size_t) const;

    std::vector<Edge *> getSuccessors();
    std::vector<Edge *> getPredecessors();

    std::vector<Node *> getSuccessorNodes();
    std::vector<Node *> getPredecessorNodes();

    void print();

    bool visited = false;
    std::size_t id_;
    NodeData data_;

// private:
    std::unordered_map<std::size_t, Edge *> parents_;
    std::unordered_map<std::size_t, Edge *> children_;
};

/* Constructor. 
    @identifier: unique id which is tied to a given node.
    @data: data container of Node. Type specified by user.
**/
Node::Node(std::size_t identifier, NodeData data)
{
    id_ = identifier;
    data_ = data;
}

/* Check if node has successors attached to it.
    \return: boolean indicating if given Node has at least one successor.
**/
inline bool
Node::hasSuccessor(void) const
{
    return !children_.empty();
}

/* Check if node has predecessors attached to it.
    \return: boolean indicating if given Node has at least one predecessor.
**/
inline bool
Node::hasPredecessor(void) const
{
    return !parents_.empty();
}

/* Obtain successor edge connecting the node to the one specified by the index-id.
    @index: node id. 
    \return: pointer to the edge connecting given node to the one specified by @index.
**/
inline Edge *
Node::getSuccessor(std::size_t index) const
{
    return children_.at(index);
}

/* Obtain predecessor edge connecting the node to the one specified by the index-id.
    @index: node id. 
    \return: pointer to the edge connecting given node to the one specified by @index.
**/
inline Edge *
Node::getPredecessor(std::size_t index) const
{
    return parents_.at(index);
}

/* Add provided edge to the set of predecessors of Node.
    @predecessor: pointer to the Edge-object which should be added as predecessor of Node.
**/
inline void
Node::addPredecessor(
    Edge *predecessor)
{
    parents_.insert(std::make_pair(predecessor->getSource()->id_, predecessor));
}

/* Add provided edge to the set of successors of Node.
    @successor: pointer to the Edge-object which should be added as successor of Node.
**/
inline void
Node::addSuccessor(
    Edge *successor)
{
    children_.insert(std::make_pair(successor->getDestination()->id_, successor));
}

/* Remove Edge connecting the Node with the one specified by @predId from the set of predecessors.
    @predId: id of node specifying the Edge to delete.
**/
inline void
Node::removePredecessor(
    std::size_t predId)
{
    parents_.erase(predId);
}

/* Remove Edge connecting the Node with the one specified by @succId from the set of successors.
    @succId: id of node specifying the Edge to delete.
**/
inline void
Node::removeSuccessor(
    std::size_t succId)
{
    children_.erase(succId);
}

/* Get the number of successors connected to the Node.
    \return: number of successors.
**/
inline std::size_t
Node::numberOfSuccessors() const
{
    return children_.size();
}

/* Get the number of predecessors connected to the Node.
    \return: number of predecessors.
**/
inline std::size_t
Node::numberOfPredecessors() const
{
    return parents_.size();
}

/* Obtain predecessor edge connecting the node to the one specified by the index-id.
    @index: node id. 
    \return: pointer to the edge connecting given node to the one specified by @index.
**/
inline std::vector<Edge *>
Node::getSuccessors()
{
    std::vector<Edge *> vecOfValues;
    vecOfValues.reserve(children_.size());

    /*** Copy all value fields from map to a vector using Lambda function ***/
    for (auto elem : children_)
        vecOfValues.push_back(elem.second);

    return vecOfValues;
}

/* Obtain predecessor edge connecting the node to the one specified by the index-id.
    @index: node id. 
    \return: pointer to the edge connecting given node to the one specified by @index.
**/
inline std::vector<Edge *>
Node::getPredecessors()
{
    std::vector<Edge *> vecOfValues;
    vecOfValues.reserve(parents_.size());

    /*** Copy all value fields from map to a vector using Lambda function ***/
    for (auto elem : parents_)
        vecOfValues.push_back(elem.second);

    return vecOfValues;
}

/* Obtain Successor Nodes. Skip connecting edges. Useful if graph not weighted.
    \return: a vector containing pointers to the succeeding nodes.
**/
inline std::vector<Node *>
Node::getSuccessorNodes()
{
    std::vector<Node *> vecOfValues;
    vecOfValues.reserve(children_.size());

    /*** Copy all value fields from map to a vector using Lambda function ***/
    for (auto elem : children_)
        vecOfValues.push_back(elem.second->getDestination());

    return vecOfValues;
}

/* Obtain Predecessor Nodes. Skip connecting edges. Useful if graph not weighted.
    \return: a vector containing pointers to the preceeding nodes.
**/
inline std::vector<Node *>
Node::getPredecessorNodes()
{
    std::vector<Node *> vecOfValues;
    vecOfValues.reserve(parents_.size());

    /*** Copy all value fields from map to a vector using Lambda function ***/
    for (auto elem : parents_)
        vecOfValues.push_back(elem.second->getSource());

    return vecOfValues;
}

/* DEBUG Function.
    Can be extended to print the conntents/metainfo about a given Node.
**/
inline void
Node::print()
{
}

/* Check if given sueprnode is Goal.
    Check whether a given supernode inside the A* search has any subassemblies to continue the search.
    Is declared as a member of the NodeData contained within Nodes.
    Placed within this files due to linking issues.
    //TODO: Move into containers.hpp and fix linking issues.
**/
bool NodeData::isGoal()
{
    for (auto &x : subassemblies)
    {
        if (x.second->hasSuccessor())
        {
            return false;
        }
    }
    return true;
}

/* Calculate the h_Score needed for the A* search.
    Is declared as a member of the NodeData contained within Nodes.
    Placed within this files due to linking issues.
    //TODO: Move into containers.hpp and fix linking issues.
**/
void NodeData::calc_hscore()
{
    std::size_t maximum_length_subassembly = 0;
    for (auto &x : subassemblies)
    {
        if (x.second->data_.name.length() > maximum_length_subassembly)
            maximum_length_subassembly = x.second->data_.name.length();
    }
    h_score = log2f(maximum_length_subassembly) * minimum_cost_action;
}

/* Calculate the f_Score needed for the A* search.
    Is declared as a member of the NodeData contained within Nodes.
    Placed within this files due to linking issues.
    //TODO: Move into containers.hpp and fix linking issues.
**/
void NodeData::calc_fscore()
{
    f_score = g_score + h_score;
}
