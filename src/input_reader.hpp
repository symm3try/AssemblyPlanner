#pragma once

#include <string>
#include <exception>
#include <tuple>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "dotwriter.hpp"
#include "tinyxml2.h"
#include "graph_generator.hpp"
#include "containers.hpp"

/* Check if a string represents a float-number.
**/
bool is_float(std::string my_string)
{
    std::istringstream iss(my_string);
    float f;
    iss >> std::noskipws >> f;
    return iss.eof() && !iss.fail();
}

/* XML InputReader.
    Reads the input provieded inside the input-XML file,
    Does some error-checking.
**/
class InputReader
{
public:

    // Constructor, Destructor
    InputReader(std::string);
    ~InputReader();

    // Read the provided XML representing the assembly with agents, costs...
    std::tuple<Graph<> *, config::Configuration*, bool> read(std::string);

private:

    int parse_graph(tinyxml2::XMLNode *);
    int parse_nodes(tinyxml2::XMLNode *);
    int parse_edges(tinyxml2::XMLNode *);

    int parse_actions(tinyxml2::XMLNode *);
    int parse_costmap(std::string, tinyxml2::XMLNode *, config::Action &);

    int parse_subassemblies(tinyxml2::XMLNode *);
    int parse_reachmap(std::string, tinyxml2::XMLNode *, config::Subassembly&);

    int parse_agents(tinyxml2::XMLNode *);

    // Parse the particular parts of the XML input.

    // XML Root Element
    tinyxml2::XMLElement *root;

    // XML Document
    tinyxml2::XMLDocument *doc;
    
    GraphGenerator *graph_gen;
    Graph<> *graph;

    config::Configuration *config;
};

/* Constructorr.
    @path: string specifying the path of the XML to read.
**/
InputReader::InputReader(std::string path)
{
    // Create a new XMLDocument. 
    // Try to load file.
    doc = new tinyxml2::XMLDocument;
    tinyxml2::XMLError result = doc->LoadFile(path.c_str());
    if (result != tinyxml2::XML_SUCCESS)
        throw std::runtime_error("Could not open XML file.");

    // Allocate objects    
    graph = new Graph;
    graph_gen = new GraphGenerator(graph);
    config = new config::Configuration;
}

/* Destructor.
    Deallocate all Object that have been put on the heap inside the constructor.
**/
InputReader::~InputReader()
{
    delete doc;
    delete graph;
    delete config;
    delete graph_gen;
}

/* Top-Level Read.
    @root_name: name of the element where reading should start.
    \return: tuple containg the graph, config represented inside the XML 
             and a boolean indicating success.
**/
std::tuple<Graph<> *, config::Configuration*, bool> InputReader::read(std::string root_name)
{

    // Find the root node of the document.
    root = doc->FirstChildElement(root_name.c_str());
    if (root == nullptr)
    {
        std::cerr << "XML: Could not find root element." << std::endl;
        return std::make_tuple(nullptr, nullptr, false);
    }


    // Find and Parse elements corresponing to the <graph/> structure.
    tinyxml2::XMLElement *graph_e = root->FirstChildElement("graph");
    if (graph_e == nullptr)
    {
        std::cerr << "XML: Could not find graph element." << std::endl;
        return std::make_tuple(nullptr, nullptr, false);
    }
    if (parse_graph(graph_e) == tinyxml2::XML_ERROR_PARSING)
    {
        std::cerr << "XML: Error Parsing Graph." << std::endl;
        return std::make_tuple(nullptr, nullptr, false);
    }


    // Find and Parse the top-level elements for the <actions/> tree.
    tinyxml2::XMLElement *actions_e = root->FirstChildElement("actions");
    if (actions_e == nullptr)
    {
        std::cerr << "XML: Could not find actions element." << std::endl;
        return std::make_tuple(nullptr, nullptr, false);
    }
    if (parse_actions(actions_e) == tinyxml2::XML_ERROR_PARSING)
    {
        std::cerr << "XML: Error Parsing Actions." << std::endl;
        return std::make_tuple(nullptr, nullptr, false);
    }


    // Find and Parse the top-level elements for the <subassemblies/> tree.
    tinyxml2::XMLElement *subassemblies_e = root->FirstChildElement("subassemblies");
    if (subassemblies_e == nullptr)
    {
        std::cerr << "XML: Could not find subassemblies element." << std::endl;
        return std::make_tuple(nullptr, nullptr, false);
    }
    if (parse_subassemblies(subassemblies_e) == tinyxml2::XML_ERROR_PARSING)
    {
        std::cerr << "XML: Error Parsing subassemblies." << std::endl;
        return std::make_tuple(nullptr, nullptr, false);
    }


    // Find and Parse the top-level elements for the <subassemblies/> tree.
    tinyxml2::XMLElement *agents_e = root->FirstChildElement("agents");
    if (agents_e == nullptr)
    {
        std::cerr << "XML: Could not find agents element." << std::endl;
        return std::make_tuple(nullptr, nullptr, false);
    }
    if (parse_agents(agents_e) == tinyxml2::XML_ERROR_PARSING)
    {
        std::cerr << "XML: Error Parsing agents." << std::endl;
        return std::make_tuple(nullptr, nullptr, false);
    }


    const char *attribute_text = nullptr;
    attribute_text = root->Attribute("root");
    if (attribute_text == NULL)
        return std::make_tuple(nullptr, nullptr, false);

    if (!graph_gen->setRoot(attribute_text))
        return std::make_tuple(nullptr, nullptr, false);

    return std::make_tuple(graph_gen->graph_, config, true);
}

int InputReader::parse_graph(tinyxml2::XMLNode *graph_root){

    // Find and Parse <nodes/> tree.
    tinyxml2::XMLElement *nodes_e = graph_root->FirstChildElement("nodes");
    if (nodes_e == nullptr)
    {
        std::cerr << "XML: Could not find nodes element." << std::endl;
        return tinyxml2::XML_ERROR_PARSING;
    }
    if (parse_nodes(nodes_e) == tinyxml2::XML_ERROR_PARSING)
    {
        std::cerr << "XML: Error Parsing Nodes." << std::endl;
        return tinyxml2::XML_ERROR_PARSING;
    }

    // Find and Parse <edges/> tree.
    tinyxml2::XMLElement *edges_e = graph_root->FirstChildElement("edges");
    if (edges_e == nullptr)
    {
        std::cerr << "XML: Could not find edges element." << std::endl;
        return tinyxml2::XML_ERROR_PARSING;
    }
    if (parse_edges(edges_e) == tinyxml2::XML_ERROR_PARSING)
    {
        std::cerr << "XML: Error Parsing Edges." << std::endl;
        return tinyxml2::XML_ERROR_PARSING;
    }

    return tinyxml2::XML_SUCCESS;
}


/* Parse nodes. 
**/
int InputReader::parse_nodes(tinyxml2::XMLNode *nodes_root)
{

    const char *attribute_text = nullptr;

    for (tinyxml2::XMLElement *child = nodes_root->FirstChildElement("node");
         child != nullptr; child = child->NextSiblingElement("node"))
    {

        attribute_text = child->Attribute("name");
        if (attribute_text == NULL){
            std::cerr << "Can't read *name* attribute of node." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string node_name = attribute_text;

        attribute_text = child->Attribute("type");
        if (attribute_text == NULL){
            std::cerr << "Can't read *type* attribute of node." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string node_type = attribute_text;

        // std::cout << node_name << " " << node_type << std::endl;

        if (node_type == "OR")
            graph_gen->insertOr(node_name);
        else if (node_type == "AND")
            graph_gen->insertAnd(node_name);
        else
            return tinyxml2::XML_ERROR_PARSING;
    }

    return tinyxml2::XML_SUCCESS;
}

/* Parse edges. 
**/
int InputReader::parse_edges(tinyxml2::XMLNode *edges_root)
{

    const char *attribute_text = nullptr;

    for (tinyxml2::XMLElement *child = edges_root->FirstChildElement("edge");
         child != nullptr; child = child->NextSiblingElement("edge"))
    {
        attribute_text = child->Attribute("start");
        if (attribute_text == NULL){
            std::cerr << "Can't read *start* attribute of edge." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string start_node = attribute_text;

        attribute_text = child->Attribute("end");
        if (attribute_text == NULL){
            std::cerr << "Can't read *end* attribute of edge." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string end_node = attribute_text;

        bool inserted = graph_gen->insertEdge(start_node, end_node);
    }

    return tinyxml2::XML_SUCCESS;
}

int InputReader::parse_actions(tinyxml2::XMLNode *actions_root){

    const char *attribute_text = nullptr;

    for (tinyxml2::XMLElement *action = actions_root->FirstChildElement("action");
         action != nullptr; action = action->NextSiblingElement("action"))
    {

        attribute_text = action->Attribute("name");
        if (attribute_text == NULL){
            std::cerr << "Can't read *name* attribute of action." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string action_name = attribute_text;

        config::Action action_temp;
        action_temp.name = action_name;

        tinyxml2::XMLElement *costmap_e = action->FirstChildElement("costmap");
        if (costmap_e == nullptr)
        {
            std::cerr << "XML: Could not find costmap element within action." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        if (parse_costmap(action_name, costmap_e, action_temp) == tinyxml2::XML_ERROR_PARSING)
        {
            std::cerr << "XML: Error Parsing Costmap." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }

        config->actions[action_name] = action_temp;
        // for (auto &i : action_temp.costs){
        //     std::cout << i.second << std::endl;
        // }
    }


    
    return tinyxml2::XML_SUCCESS;
}


int InputReader::parse_subassemblies(tinyxml2::XMLNode *subass_root){

    const char *attribute_text = nullptr;

    for (tinyxml2::XMLElement *subassembly = subass_root->FirstChildElement("subassembly");
            subassembly != nullptr; subassembly = subassembly->NextSiblingElement("subassembly"))
    {
        attribute_text = subassembly->Attribute("name");
        if (attribute_text == NULL){
            std::cerr << "Can't read *name* attribute of subassembly.." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string subassembly_name = attribute_text;

        config::Subassembly subassembly_temp;
        subassembly_temp.name = subassembly_name;

        tinyxml2::XMLElement *reachmap_e = subassembly->FirstChildElement("reachmap");
        if (reachmap_e == nullptr)
        {
            std::cerr << "XML: Could not find reachmap element within subassembly." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        if (parse_reachmap(subassembly_name, reachmap_e, subassembly_temp) == tinyxml2::XML_ERROR_PARSING)
        {
            std::cerr << "XML: Error Parsing Reachmap." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }

        config->subassemblies[subassembly_name] = subassembly_temp;
    }

    return tinyxml2::XML_SUCCESS;
}


/* Parse reachability. 
**/
int InputReader::parse_reachmap(std::string part_name, tinyxml2::XMLNode *reachmap_root, config::Subassembly & subassembly)
{

    const char *attribute_text = nullptr;

    for (tinyxml2::XMLElement *reach = reachmap_root->FirstChildElement("reach");
            reach != nullptr; reach = reach->NextSiblingElement("reach"))
    {

        attribute_text = reach->Attribute("agent");
        if (attribute_text == NULL){
            std::cerr << "Can't read *agent* attribute of reach." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string agent_name = attribute_text;

        attribute_text = reach->Attribute("reachable");
        if (attribute_text == NULL){
            std::cerr << "Can't read *reachable* attribute of reach." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string agent_part_reach = attribute_text;

        attribute_text = reach->Attribute("interaction");
        if (attribute_text == NULL){
            std::cerr << "Can't read *interaction* attribute of reach." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string interaction = attribute_text;

        std::transform(agent_part_reach.begin(), agent_part_reach.end(), agent_part_reach.begin(),
                        [](unsigned char c) { return std::tolower(c); });

        std::transform(interaction.begin(), interaction.end(), interaction.begin(),
                        [](unsigned char c) { return std::tolower(c); });

        if (agent_part_reach == "false")
        {
            subassembly.reachability[agent_name] = std::make_pair(false, interaction); 

            if ( config->actions.find(interaction) == config->actions.end() ) {
                std::cerr << "XML: Wrong name of interaction."
                            << "  Interaction: " << interaction
                            << "  was not provided in CostMap." << std::endl;

                return tinyxml2::XML_ERROR_PARSING;
            }
        }
        else if (agent_part_reach == "true")
        {
            subassembly.reachability[agent_name] = std::make_pair(true, interaction); 
        }
        else
        {
            std::cerr << "XML: Wrong value for cost."
                        << "  Agent: " << agent_name
                        << "  Part: " << part_name << std::endl;

            return tinyxml2::XML_ERROR_PARSING;
        }
    }
    return tinyxml2::XML_SUCCESS;
}

/* Parse costs. 
**/
int InputReader::parse_costmap(std::string action_name, tinyxml2::XMLNode *costmap_e, config::Action & act)
{
    const char *attribute_text = nullptr;

    for (tinyxml2::XMLElement *cost = costmap_e->FirstChildElement("cost");
             cost != nullptr; cost = cost->NextSiblingElement("cost"))
    {

        attribute_text = cost->Attribute("agent");
        if (attribute_text == NULL){
            std::cerr << "Can't read *agent* attribute of cost." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string agent_name = attribute_text;

        attribute_text = cost->Attribute("value");
        if (attribute_text == NULL){
            std::cerr << "Can't read *value* attribute of cost." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string cost_value = attribute_text;

        std::transform(cost_value.begin(), cost_value.end(), cost_value.begin(),
                        [](unsigned char c) { return std::tolower(c); });

        // std::cout     << "  Agent: "  << agent_name
        //   << "  Action: " << action_name << std::endl;



        if (cost_value == "inf")
        {
            act.costs[agent_name] = INT_MAX;
        }
        else if (is_float(cost_value))
        {
            act.costs[agent_name] = std::stod(cost_value);
        }
        else
        {
            std::cerr << "XML: Wrong value for cost."
                        << "  Agent: " << agent_name
                        << "  Action: " << action_name << std::endl;

            return tinyxml2::XML_ERROR_PARSING;
        }
    }

    return tinyxml2::XML_SUCCESS;
}


int InputReader::parse_agents(tinyxml2::XMLNode *agents_root){
    
    const char *attribute_text = nullptr;

    for (tinyxml2::XMLElement *agent = agents_root->FirstChildElement("agent");
            agent != nullptr; agent = agent->NextSiblingElement("agent"))
    {
        attribute_text = agent->Attribute("name");
        if (attribute_text == NULL){
            std::cerr << "Can't read *name* attribute of agent." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string agent_name = attribute_text;

        attribute_text = agent->Attribute("host");
        if (attribute_text == NULL){
            std::cerr << "Can't read *host* attribute of agent." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string host = attribute_text;

        attribute_text = agent->Attribute("port");
        if (attribute_text == NULL){
            std::cerr << "Can't read *port* attribute of agent." << std::endl;
            return tinyxml2::XML_ERROR_PARSING;
        }
        std::string port = attribute_text;

        config::Agent agent_temp;
        agent_temp.name = agent_name;
        agent_temp.hostname = host;
        agent_temp.port = port;

        config->agents[agent_name] = agent_temp;
    }

    return tinyxml2::XML_SUCCESS;
}
