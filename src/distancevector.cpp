/**
 * @file network_routing_simulation.cpp
 * @brief Simulation of distance vector routing protocol for network routing.
 *
 * This program simulates the operation of a distance vector routing protocol,
 * demonstrating how routers compute the shortest paths to all other routers using
 * the Bellman-Ford algorithm and adapt to changes in the network topology.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <climits>
#include <set>
#include <map>
#include <ios>

/**
 * @struct Link
 * @brief Represents a link between two network nodes.
 *
 * This struct holds information about a link in the network, including
 * the IDs of the connected nodes and the cost of the path between them.
 */
struct Link {
    int node1;
    int node2;
    int pathCost;
};

/**
 * @struct Message
 * @brief Represents a message to be routed through the network.
 *
 * This struct holds information about a message that needs to be sent from a source node
 * to a destination node in the network. It includes the IDs of the source and destination nodes,
 * as well as the content of the message itself.
 */
struct Message {
    int sourceID;
    int destinationID;
    std::string message;
};

/**
 * @class RoutingTable
 * @brief Manages routing information for a router.
 *
 * The RoutingTable class stores and manages the routing information for a router, 
 * including the next hop and path cost for reaching other nodes in the network. 
 * It provides functionality to add routes, check if a route exists, and retrieve the
 * next hop and path cost for a given destination.
 */
class RoutingTable {
public:

    /**
     * Constructs a RoutingTable for a specific router.
     * Initializes routing table entries for all nodes in the network with default values.
     * @param myID The ID of the router this table belongs to.
     * @param nodes A set containing the IDs of all nodes in the network.
     */
    RoutingTable(int myID, const std::set<int> &nodes) {

        for (const int &id : nodes) {

            if (id == myID) {

                table[id] = std::make_pair(id, 0);

            } else {

                table[id] = std::make_pair(-1, 9999);

            }
        }
    }

    /**
     * Adds or updates a route in the routing table.
     * If the route already exists, it updates the next hop and path cost.
     * @param destinationID The destination node ID of the route.
     * @param nextHopID The next hop node ID towards the destination.
     * @param pathCost The cost of the path to the destination.
     */
    // this function can also be used to update the existing entry
    void 
    addRoute(int destinationID, int nextHopID, int pathCost) {

        table[destinationID] = std::make_pair(nextHopID, pathCost);

    }

    /**
     * Checks if the table contains a route to the specified destination.
     * @param destinationID The ID of the destination node.
     * @return True if the table contains a route to the destination, false otherwise.
     */
    bool 
    contains(int destinationID) const {

        return table.find(destinationID) != table.end();

    }

    /**
     * Retrieves the next hop node ID for a given destination.
     * @param destinationID The ID of the destination node.
     * @return The next hop node ID towards the destination, or -1 if no route exists.
     */
    int 
    getNextHop(int destinationID) const {

        if (contains(destinationID)) {

            return table.at(destinationID).first;

        }

        return -1;

    }

    /**
     * Retrieves the path cost to a given destination.
     * @param destinationID The ID of the destination node.
     * @return The cost of the path to the destination, or -1 if no route exists.
     */
    int
    getPathCost(int destinationID) const {

        if (contains(destinationID)) {

            return table.at(destinationID).second;

        }

        return -1;

    }

     /**
     * Gets the entire routing table.
     * @return A const reference to the internal map representing the routing table.
     *         Each entry maps a destination node ID to a pair of next hop node ID and path cost.
     */
    const std::map<int, std::pair<int, int>>&
    getRoutingTable() const {

        return table;

    }

private:
    // destination_ID : (next_hop_ID, cost)
    std::map<int, std::pair<int, int>> table;
};

/**
 * @class Router
 * @brief Represents a router in a network, managing a routing table for distance vector routing.
 *
 * This class encapsulates a network router's functionalities, including maintaining a routing
 * table, adding routes, and determining the next hop and path cost to various destinations within
 * the network. It acts as an interface for interacting with the router's routing table.
 */
class Router {
public:

    /**
     * Constructs a Router with a given ID and initializes its routing table.
     * @param id The unique identifier of the router.
     * @param nodes A set containing the IDs of all nodes within the network.
     */
    Router(int id, const std::set<int> &nodes) : ID(id), RT(id, nodes) {}

    /**
     * Adds or updates a route in the router's routing table.
     * @param destinationID The ID of the destination node.
     * @param nextHopID The ID of the next hop node towards the destination.
     * @param pathCost The cost of the path to the destination.
     */
    void 
    addRoute(int destinationID, int nextHopID, int pathCost) {

        RT.addRoute(destinationID, nextHopID, pathCost);

    }

    /**
     * Retrieves the next hop ID for a given destination from the router's routing table.
     * @param destinationID The ID of the destination node.
     * @return The ID of the next hop node towards the destination.
     */
    int 
    getNextHop(int destinationID) const {

        return RT.getNextHop(destinationID);

    }

    /**
     * Retrieves the path cost to a given destination from the router's routing table.
     * @param destinationID The ID of the destination node.
     * @return The cost of the path to the destination.
     */
    int 
    getPathCost(int destinationID) const {

        return RT.getPathCost(destinationID);

    }

    /**
     * Gets the entire routing table of the router.
     * @return A constant reference to the router's routing table.
     */
    const std::map<int, std::pair<int, int>>&
    getRoutingTable() const {

        return RT.getRoutingTable();
        
    }

    /**
     * Retrieves the router's ID.
     * @return The ID of the router.
     */
    const int
    getID() const {

        return ID;

    }

private:
    int ID;     ///< The unique identifier of the router.
    RoutingTable RT;        ///< The routing table managed by the router.
};

/**
 * Retrieves a reference to a router from a list of routers by its ID.
 * @param routers A vector containing all routers within the network.
 * @param ID The unique identifier of the desired router.
 * @return A reference to the specified router.
 * @throws std::runtime_error if a router with the specified ID is not found.
 */
Router&
getRouterByID (std::vector<Router> &routers, int ID) {

    for (auto &router : routers) {

        if (router.getID() == ID) return router;

    }

    throw std::runtime_error("Router with the specified ID not found.");

}

/**
 * Removes a link between two nodes from the list of links.
 * @param links A vector of all links within the network.
 * @param linkToRemove The link to be removed, specified by the nodes it connects.
 * This function checks for and removes the link in both directions, as links are bidirectional.
 */
void 
removeLink(std::vector<Link> &links, const Link &linkToRemove) {

    auto it = std::remove_if(links.begin(), links.end(), [&linkToRemove](const Link &link) {
                                return (link.node1 == linkToRemove.node1 && link.node2 == linkToRemove.node2) ||
                                       (link.node1 == linkToRemove.node2 && link.node2 == linkToRemove.node1);
                            });

    links.erase(it, links.end());

}

/**
 * Initializes the network topology from a given file.
 * 
 * This function reads a topology file specifying links between nodes and their path costs,
 * then initializes the list of links, the set of nodes, and the vector of routers based on
 * this information. It also establishes direct links in the routers' routing tables.
 *
 * @param topologyFile The path to the file containing the network topology.
 * @param links A reference to a vector where the read links will be stored.
 * @param nodes A reference to a set where the unique node IDs will be stored.
 * @param routers A reference to a vector of Router objects to be initialized based on the topology.
 */
void
initTopology (const std::string &topologyFile, std::vector<Link> &links, std::set<int> &nodes, std::vector<Router> &routers) {

    std::ifstream file(topologyFile);

    if (!file.is_open()) {
        std::cerr << "Cannot open topology file: " << topologyFile << std::endl;
        exit(EXIT_FAILURE);
    }

    // read all links from the topology file into a vector
    int node1, node2, pathCost;
    while (file >> node1 >> node2 >> pathCost) {
        links.push_back({node1, node2, pathCost});
        nodes.insert(node1);
        nodes.insert(node2);
    }

    file.close();

    // initialize all routers and their forwarding table
    for (const int &id : nodes) {

        routers.emplace_back(id, nodes);

    }

    // add direct link(s) to the involved routers
    for (const auto &link : links) {
        getRouterByID(routers, link.node1).addRoute(link.node2, link.node2, link.pathCost);
        getRouterByID(routers, link.node2).addRoute(link.node1, link.node1, link.pathCost);
    }

}

/**
 * Reads network topology changes from a given file.
 * 
 * This function processes a file specifying changes to the network topology, which may include
 * adding or removing links, as well as changing path costs. Each change is stored in the provided
 * vector for later application.
 *
 * @param changesFile The path to the file containing topology changes.
 * @param changes A reference to a vector where the topology changes will be stored.
 * @param nodes A reference to a set of node IDs; this set may be updated with new node IDs found in the changes file.
 */
void
readChangesFile (const std::string &changesFile, std::vector<Link> &changes, std::set<int> &nodes) {

    std::ifstream file(changesFile);

    if (!file.is_open()) {
        std::cerr << "Cannot open changes file: " << changesFile << std::endl;
        exit(EXIT_FAILURE);
    }

    int node1, node2, pathCost;
    while (file >> node1 >> node2 >> pathCost) {

        changes.push_back({node1, node2, pathCost});
      
    }

    file.close();

}

/**
 * Applies a single topology change to the network.
 * 
 * This function applies a change to the network topology, which may involve adding a new link,
 * updating an existing link's path cost, or removing a link. The function updates the list of links,
 * the set of nodes, and re-initializes routers to reflect the change.
 *
 * @param change The change to apply, represented as a Link struct. A pathCost of -999 indicates the link should be removed.
 * @param routers A reference to a vector of Router objects; this vector will be cleared and re-initialized based on the updated topology.
 * @param nodes A reference to a set of node IDs; this set will be updated to include any new nodes introduced by the change.
 * @param links A reference to a vector of existing links; this vector will be updated to reflect the applied change.
 */
void
applyChange(const Link &change, std::vector<Router> &routers, std::set<int> &nodes, std::vector<Link> &links) {

    nodes.insert(change.node1);
    nodes.insert(change.node2);

    if (change.pathCost == -999) {

        removeLink(links, change);

    } else {

        links.push_back({change.node1, change.node2, change.pathCost});

    }

    routers.clear();

    for (const int &id : nodes) {

        routers.emplace_back(id, nodes);

    }

    for (const auto &link : links) {

        getRouterByID(routers, link.node1).addRoute(link.node2, link.node2, link.pathCost);
        getRouterByID(routers, link.node2).addRoute(link.node1, link.node1, link.pathCost);

    }

}

/**
 * Executes the Bellman-Ford algorithm to compute the shortest paths in the network.
 *
 * This function iteratively updates the routing tables of all routers in the network
 * based on the Bellman-Ford algorithm. It ensures that each router has the most efficient
 * path to every other router by minimizing the path cost. The algorithm runs until no more
 * updates are made to the routing tables.
 *
 * @param routers A reference to a vector of Router objects representing all routers in the network.
 * @param nodes A constant reference to a set containing the IDs of all nodes in the network.
 * @param links A constant reference to a vector of Link objects representing all the links between nodes.
 */
void
doBellmanFordAlg (std::vector<Router> &routers, const std::set<int> &nodes, const std::vector<Link> &links) {

    bool updated = true;

    while (updated) {

        updated = false;

        for (auto &router : routers) {

            for (const int &destinationID : nodes) {

                int curPathCost = router.getPathCost(destinationID);

                int newNextHop = -1;

                for (const auto &link : links) {

                    int neighbourID = (link.node1 == router.getID()) ? link.node2 : (link.node2 == router.getID()) ? link.node1 : -1;

                    if (neighbourID == -1 || neighbourID == destinationID) continue; // only get related link
                    if (getRouterByID(routers, neighbourID).getNextHop(destinationID) == router.getID()) continue; // split horizon
                    
                    int NeighbourPathCost = router.getPathCost(neighbourID);
                    int NeighbourToDestPathCost = getRouterByID(routers, neighbourID).getPathCost(destinationID);

                    if ((NeighbourPathCost + NeighbourToDestPathCost < curPathCost) || 
                        (NeighbourPathCost + NeighbourToDestPathCost == curPathCost && neighbourID < newNextHop)) {

                        curPathCost = NeighbourPathCost + NeighbourToDestPathCost;
                        newNextHop = neighbourID;

                        router.addRoute(destinationID, newNextHop, curPathCost);
                        updated = true;

                    }

                }

            }

        }

    }

}

/**
 * Writes the routing tables of all routers to an output file.
 *
 * This function iterates over each router in the network and writes its routing table
 * to the specified output file. Each entry in the routing table is written in the format:
 * destination nextHop pathCost, where each value is separated by a space.
 *
 * @param outputFile The path to the file where the routing tables will be written.
 * @param routers A constant reference to a vector of Router objects representing all routers in the network.
 */
void
writeFT (const std::string outputFile, const std::vector<Router> &routers) {

    std::ofstream outFile(outputFile, std::ios::app);
    
    if (!outFile.is_open()) {
        std::cerr << "Cannot open output file: " << outputFile << std::endl;
        exit(EXIT_FAILURE);
    }

    for (const auto &router : routers) {
        
        for (const auto &entry : router.getRoutingTable()) {

            int destination = entry.first;
            int nextHop = entry.second.first;
            int pathCost = entry.second.second;

            outFile << destination << " " << nextHop << " " << pathCost << "\n";
        }

        outFile << "\n";

    }

    outFile.close();

}

/**
 * Reads messages to be routed from a specified file.
 *
 * This function opens a file containing messages, where each message is specified with
 * a source ID, destination ID, and the message content. It reads these messages and stores
 * them in a vector for later processing. Each message is represented by a Message struct.
 *
 * @param messageFile The path to the file containing the messages.
 * @param messages A reference to a vector of Message structs where the read messages will be stored.
 */
void
readMessagesFile (const std::string messageFile, std::vector<Message> &messages) {

    std::ifstream file(messageFile);

    if (!file.is_open()) {
        std::cerr << "Cannot open messages file: " << messageFile << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(file, line)) {

        std::istringstream iss(line);
        int sourceID, destinationID;
        std::string original, message;

        iss >> sourceID >> destinationID;
        std::getline(iss, original);

        message = original.substr(original.find_first_not_of(" "));

        messages.push_back({sourceID, destinationID, message});

    }

    file.close();

}

/**
 * Forwards messages based on the computed routing tables and writes the results to an output file.
 *
 * This function iterates over a list of messages, each containing a source and destination ID,
 * and routes them according to the shortest path determined by the routing tables of the routers.
 * The path and total cost are written to the specified output file. If a path cannot be found,
 * an "unreachable" message is recorded.
 *
 * @param outputFile The path to the file where the message routes will be written.
 * @param routers A reference to a vector of Router objects representing all routers in the network.
 * @param messages A constant reference to a vector of Message structs representing all messages to be sent.
 */
void
sendMessages (const std::string outputFile, std::vector<Router> &routers, const std::vector<Message> &messages) {

    std::ofstream outFile(outputFile, std::ios::app);
    
    if (!outFile.is_open()) {
        std::cerr << "Cannot open output file: " << outputFile << std::endl;
        exit(EXIT_FAILURE);
    }

    for (const auto &message : messages) {

        std::string outMessage;

        int pathCost = getRouterByID(routers, message.sourceID).getPathCost(message.destinationID);

        if (pathCost == 9999) {

            outMessage = "from " + std::to_string(message.sourceID) + " to " + std::to_string(message.destinationID) +
                         " cost infinite hops unreachable message " + message.message;

            outFile << outMessage << "\n";
            outFile << "\n";

            continue;

        }

        int currentID = message.sourceID;

        outMessage = "from " + std::to_string(message.sourceID) + " to " + std::to_string(message.destinationID) +
                     " cost " + std::to_string(pathCost) + " hops ";

        while (currentID != message.destinationID) {

            outMessage += std::to_string(currentID) + " ";

            currentID = getRouterByID(routers, currentID).getNextHop(message.destinationID);

        }

        outMessage += "message " + message.message;

        outFile << outMessage << "\n";
        outFile << "\n";

    }

    outFile.close();

}

/**
 * Executes the distance vector routing simulation.
 *
 * This function orchestrates the entire simulation process. It initializes the network topology
 * from a file, performs the Bellman-Ford algorithm to compute shortest paths, handles message
 * forwarding based on the computed paths, applies any network changes from a separate file, and
 * repeats the computation and message forwarding after each change. Results are written to the
 * specified output file.
 *
 * @param topologyFile The path to the file containing the initial network topology.
 * @param messageFile The path to the file containing messages to be routed.
 * @param changesFile The path to the file containing network topology changes.
 * @param outputFile The path to the file where the simulation results will be written.
 */
void
dvr (const std::string topologyFile, const std::string messageFile, const std::string changesFile, const std::string outputFile) {

    std::vector<Link> links;
    std::vector<Link> changes;
    std::vector<Message> messages;
    std::set<int> nodes;
    std::vector<Router> routers;

    std::ofstream outFile(outputFile, std::ofstream::out);
    if (!outFile.is_open()) {
        std::cerr << "Cannot open output file: " << outputFile << std::endl;
        exit(EXIT_FAILURE);
    }
    outFile.close();

    initTopology(topologyFile, links, nodes, routers);

    doBellmanFordAlg(routers, nodes, links);

    writeFT(outputFile, routers);

    readMessagesFile(messageFile, messages);

    sendMessages(outputFile, routers, messages);

    readChangesFile(changesFile, changes, nodes);

    for (const auto &change : changes) {

        applyChange(change, routers, nodes, links);

        doBellmanFordAlg(routers, nodes, links);

        writeFT(outputFile, routers);

        sendMessages(outputFile, routers, messages);

    }

}

/**
 * The entry point of the distance vector routing simulation program.
 *
 * This function parses command-line arguments for the topology, messages, changes files,
 * and an optional output file. It then initiates the routing simulation.
 *
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments, including the program name and file paths.
 * @return Returns 0 on successful execution, 1 on incorrect usage.
 */
int 
main(int argc, char** argv) {

    std::cout << "entered main()\n" << std::endl;

    if (argc != 4 && argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <topologyFile> <messageFile> <changesFile> [<outputFile>]" << std::endl;
        return 1;
    }

    std::string topologyFile = argv[1];
    std::string messageFile = argv[2];
    std::string changesFile = argv[3];
    std::string outputFile;

    if (argc == 5) {
        outputFile = argv[4];
    } else {
        outputFile = "output.txt";
    }

    dvr(topologyFile, messageFile, changesFile, outputFile);

    return 0;

}