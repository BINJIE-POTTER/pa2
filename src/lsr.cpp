/**
 * @file link_state_routing_simulation.cpp
 * @brief Simulation of Link State Routing (LSR) protocol for network routing.
 *
 * Implements a basic simulation of the Link State Routing protocol. The program reads network topology,
 * messages, and possible changes to the topology from files, computes routing paths using Dijkstra's algorithm,
 * and outputs the routing paths and messages traversal details. It demonstrates the dynamic adjustment of
 * routing paths in response to changes in network topology.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <limits>


using namespace std;

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
    int cost;
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
    int source;
    int destination;
    string content;
};

/**
 * @brief Parses the topology file to create a vector of Link structures.
 *
 * Reads a file specifying the network topology, with each line representing a link between two nodes
 * and the associated cost. Creates a vector of Link structures to represent the network topology for
 * use in routing simulations.
 *
 * @param filename The path to the topology file.
 * @return Vector of Link structures representing the network's links and their costs.
 */
vector<Link> parseTopologyFile(const string& filename) {
    vector<Link> links;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            int node1, node2, cost;
            ss >> node1 >> node2 >> cost;
            links.push_back({node1, node2, cost});
        }
        file.close();
    } else {
        cerr << "Unable to open file: " << filename << endl;
    }
    return links;
}

/**
 * @brief Parses the message file to create a vector of Message structures.
 *
 * Reads a file containing messages to be sent across the network. Each line specifies a message's
 * source node, destination node, and content. Creates a vector of Message structures for the simulation
 * to process and route.
 *
 * @param filename The path to the message file.
 * @return Vector of Message structures representing the messages to be routed.
 */
vector<Message> parseMessageFile(const string& filename) {
    vector<Message> messages;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            int source, destination;
            string content;
            ss >> source >> destination;
            getline(ss, content);
            messages.push_back({source, destination, content});
        }
        file.close();
    } else {
        cerr << "Unable to open file: " << filename << endl;
    }
    return messages;
}

/**
 * @brief Parses the changes file to create a vector of Link structures representing changes to the topology.
 *
 * Reads a file containing potential changes to the network topology, such as updated costs for existing links
 * or the addition/removal of links. Creates a vector of Link structures representing these changes for the
 * simulation to apply.
 *
 * @param filename The path to the changes file.
 * @return Vector of Link structures representing changes to the network's topology.
 */
vector<Link> parseChangesFile(const string& filename) {
    vector<Link> changes;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            int node1, node2, cost;
            ss >> node1 >> node2 >> cost;
            changes.push_back({node1, node2, cost});
        }
        file.close();
    } else {
        cerr << "Unable to open file: " << filename << endl;
    }
    return changes;
}

/**
 * @brief Executes the Link State Routing simulation.
 *
 * Orchestrates the entire simulation process, including parsing input files for network topology, messages,
 * and topology changes. It computes routing paths using Dijkstra's algorithm and simulates the routing of
 * messages according to these paths. Outputs the results to a specified file, detailing the paths taken
 * for messages and the effects of any applied topology changes.
 *
 * @param topologyFile Path to the file containing the network topology.
 * @param messageFile Path to the file containing messages to be routed.
 * @param changesFile Path to the file containing changes to the network topology.
 * @param outputFile Path to the file where the simulation results will be written.
 */
void lsr(const string& topologyFile, const string& messageFile, const string& changesFile, const string& outputFile) {
    vector<Link> topology = parseTopologyFile(topologyFile);
    vector<Message> messages = parseMessageFile(messageFile);
    vector<Link> changes = parseChangesFile(changesFile);

    cout << "Topology File Contents:" << endl;
    for (const auto& link : topology) {
        cout << link.node1 << " " << link.node2 << " " << link.cost << endl;
    }
    cout << endl;

    cout << "Message File Contents:" << endl;
    for (const auto& message : messages) {
        cout << message.source << " " << message.destination << " " << message.content << endl;
    }
    cout << endl;

    cout << "Changes File Contents:" << endl;
    for (const auto& change : changes) {
        cout << change.node1 << " " << change.node2 << " " << change.cost << endl;
    }
    cout << endl;
    
    map<int, map<int, int>> lsdb; // Link State Database: key(node) -> value(neighbor, cost)
    for (const Link& link : topology) {
        lsdb[link.node1][link.node2] = link.cost;
        lsdb[link.node2][link.node1] = link.cost; // Add reverse link for undirected graph
    }

    map<int, map<int, pair<int, int>>> routingTables; // Routing Tables: key(node) -> value(destination, (nextHop, cost))
    for (const auto& entry : lsdb) {
        int node = entry.first;
        routingTables[node][node] = make_pair(node, 0); // Node's entry for itself
        for (const auto& neighbor : entry.second) {
            routingTables[node][neighbor.first] = make_pair(neighbor.first, neighbor.second);
        }
    }

    // Dijkstra's algorithm to compute shortest paths
    for (const auto& entry : lsdb) {
        int source = entry.first;
        map<int, int> distance; // Key: destination, Value: distance
        map<int, bool> visited; // Key: node, Value: visited

        for (const auto& neighbor : entry.second) {
            distance[neighbor.first] = neighbor.second;
            visited[neighbor.first] = false;
        }

        distance[source] = 0;
        visited[source] = true;

        for (size_t i = 0; i < lsdb.size(); ++i) {
            int minDistance = numeric_limits<int>::max();
            int minNode = -1;

            for (const auto& neighbor : entry.second) {
                int node = neighbor.first;
                if (!visited[node] && distance[node] < minDistance) {
                    minDistance = distance[node];
                    minNode = node;
                }
            }

            if (minNode == -1) {
                break;
            }

            visited[minNode] = true;

            for (const auto& neighbor : lsdb[minNode]) {
                int node = neighbor.first;
                int cost = neighbor.second;
                if (!visited[node] && minDistance != numeric_limits<int>::max() && 
                    minDistance + cost < distance[node]) {
                    distance[node] = minDistance + cost;
                    routingTables[source][node] = make_pair(minNode, distance[node]);
                }
            }
        }
    }

    ofstream outfile(outputFile);
    if (outfile.is_open()) {
        for (const auto& routingTable : routingTables) {
            int node = routingTable.first;
            outfile << "Topology entries for node " << node << ":" << endl;
            for (const auto& entry : routingTable.second) {
                int destination = entry.first;
                int nextHop = entry.second.first;
                int pathCost = entry.second.second;
                outfile << destination << " " << nextHop << " " << pathCost << endl;
            }
            outfile << endl;
        }

        for (const auto& message : messages) {
            outfile << "from " << message.source << " to " << message.destination << " cost ";
            int totalCost = routingTables[message.source][message.destination].second;
            vector<int> hops;
            int dest = message.destination;
            while (dest != routingTables[message.source][dest].first) {
                hops.push_back(dest);
                dest = routingTables[message.source][dest].first;
            }
            outfile << totalCost << " hops ";
            for (int i = hops.size() - 1; i >= 0; --i) {
                outfile << hops[i] << " ";
            }
            outfile << "message " << message.content << endl;
        }

        if (!changes.empty()) {
            outfile << "--- At this point, the 1st change is applied (and this line does not appear)" << endl;
        }

        for (size_t i = 0; i < changes.size(); ++i) {
            const auto& change = changes[i];
            lsdb[change.node1][change.node2] = change.cost;
            lsdb[change.node2][change.node1] = change.cost;

            for (const auto& entry : lsdb) {
                int node = entry.first;
                map<int, int> distance;
                map<int, bool> visited;

                for (const auto& neighbor : entry.second) {
                    distance[neighbor.first] = neighbor.second;
                    visited[neighbor.first] = false;
                }

                distance[node] = 0;
                visited[node] = true;

                for (size_t i = 0; i < lsdb.size(); ++i) {
                    int minDistance = numeric_limits<int>::max();
                    int minNode = -1;

                    for (const auto& neighbor : entry.second) {
                        int node = neighbor.first;
                        if (!visited[node] && distance[node] < minDistance) {
                            minDistance = distance[node];
                            minNode = node;
                        }
                    }

                    if (minNode == -1) {
                        break;
                    }

                    visited[minNode] = true;

                    for (const auto& neighbor : lsdb[minNode]) {
                        int node = neighbor.first;
                        int cost = neighbor.second;
                        if (!visited[node] && minDistance != numeric_limits<int>::max() && 
                            minDistance + cost < distance[node]) {
                            distance[node] = minDistance + cost;
                            routingTables[node][node] = make_pair(minNode, distance[node]);
                        }
                    }
                }
            }

            for (const auto& routingTable : routingTables) {
                int node = routingTable.first;
                outfile << "Topology entries for node " << node << ":" << endl;
                for (const auto& entry : routingTable.second) {
                    int destination = entry.first;
                    int nextHop = entry.second.first;
                    int pathCost = entry.second.second;
                    outfile << destination << " " << nextHop << " " << pathCost << endl;
                }
                outfile << endl;
            }

            for (const auto& message : messages) {
                outfile << "from " << message.source << " to " << message.destination << " cost ";
                int totalCost = routingTables[message.source][message.destination].second;
                vector<int> hops;
                int dest = message.destination;
                while (dest != routingTables[message.source][dest].first) {
                    hops.push_back(dest);
                    dest = routingTables[message.source][dest].first;
                }
                outfile << totalCost << " hops ";
                for (int i = hops.size() - 1; i >= 0; --i) {
                    outfile << hops[i] << " ";
                }
                outfile << "message " << message.content << endl;
            }

            if (i < changes.size() - 1) {
                outfile << "--- At this point, the " << (i + 2) << "nd change is applied (and this line does not appear)" << endl;
            }
        }
        outfile.close();
    } else {
        cerr << "Unable to open output file." << endl;
    }
}

/**
 * @brief Entry point for the Link State Routing (LSR) simulation.
 *
 * Accepts command-line arguments for the simulation's configuration, including paths to the topology, message, 
 * and changes files, with an optional output file path. If no output file is specified, "output.txt" is used by default.
 * Executes the LSR simulation based on the provided files.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array containing the command-line arguments.
 * @return Returns 0 on successful execution, 1 on incorrect usage.
 */
int main(int argc, char** argv) {
    if (argc != 4 && argc != 5) {
        cerr << "Usage: " << argv[0] << " <topologyFile> <messageFile> <changesFile> [<outputFile>]" << endl;
        return 1;
    }

    string topologyFile = argv[1];
    string messageFile = argv[2];
    string changesFile = argv[3];
    string outputFile;

    if (argc == 5) {
        outputFile = argv[4];
    } else {
        outputFile = "output.txt";
    }

    lsr(topologyFile, messageFile, changesFile, outputFile);

    return 0;

}