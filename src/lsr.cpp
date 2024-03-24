#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <limits>
#include <climits>
#include <set>
#include <algorithm>

using namespace std;

struct Link {
    string node1;
    string node2;
    int cost;
};

struct Message {
    string source;
    string destination;
    string content;
};

// Parse the topology file and store links in a vector
vector<Link> parseTopologyFile(const string& filename) {
    vector<Link> links;
    ifstream file(filename);

    if (file.is_open()) {

        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            string node1, node2;
            int cost;
            ss >> node1 >> node2 >> cost;
            links.push_back({node1, node2, cost});
        }

        file.close();

    } else {
        cerr << "Unable to open file: " << filename << endl;
    }

    return links;

}

// Parse the message file and store messages in a vector
vector<Message> parseMessageFile(const string& filename) {
    vector<Message> messages;
    ifstream file(filename);

    if (file.is_open()) {

        string line;

        while (getline(file, line)) {

            stringstream ss(line);
            string source, destination, content;
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

// Parse the changes file and store changes in a vector
vector<Link> parseChangesFile(const string& filename) {
    vector<Link> changes;
    ifstream file(filename);

    if (file.is_open()) {

        string line;

        while (getline(file, line)) {

            stringstream ss(line);
            string node1, node2;
            int cost;
            ss >> node1 >> node2 >> cost;
            changes.push_back({node1, node2, cost});

        }

        file.close();

    } else {
        cerr << "Unable to open file: " << filename << endl;
    }

    return changes;
}

// Perform Dijsktra algorithm
void updateRoutingTables(map<string, map<string, int>>& lsdb, map<string, map<string, pair<string, int>>>& routingTables) {

    for (const auto& entry : lsdb) {

        string source = entry.first;

        // Initialize distances to infinity for all nodes except the source
        map<string, int> distance;

        for (const auto& pair : lsdb) {

            string node = pair.first;
            distance[node] = (node == source) ? 0 : INT_MAX;

        }

        // Use a set to keep track of visited nodes
        set<string> visited;

        while (visited.size() < lsdb.size()) {

            // Find the node with the minimum distance from the source among unvisited nodes
            string current_node;
            int min_distance = INT_MAX;

            for (const auto& pair : distance) {

                string node = pair.first;

                if (visited.find(node) == visited.end() && pair.second < min_distance) {
                    min_distance = pair.second;
                    current_node = node;
                }

            }

            // Add the current node to visited set
            visited.insert(current_node);

            // Update distances for all neighbors of the current node
            for (const auto& neighbor : lsdb[current_node]) {

                string neighbor_node = neighbor.first;
                int neighbor_distance = neighbor.second;

                if (distance[current_node] != INT_MAX && distance[current_node] + neighbor_distance < distance[neighbor_node]) {

                    distance[neighbor_node] = distance[current_node] + neighbor_distance;
                    routingTables[source][neighbor_node] = make_pair(current_node, distance[neighbor_node]);

                } else if (distance[current_node] != INT_MAX && distance[current_node] + neighbor_distance == distance[neighbor_node]) {

                    // Check if there's a shorter path to neighbor_node
                    if (routingTables[source][neighbor_node].second > distance[neighbor_node]) {

                        routingTables[source][neighbor_node] = make_pair(current_node, distance[neighbor_node]);

                    }

                }

            }

        }

    }
    
}


// Perform Link State Routing (LSR)
void lsr(const string& topologyFile, const string& messageFile, const string& changesFile, const string& outputFile) {

    vector<Link> topology = parseTopologyFile(topologyFile);
    vector<Message> messages = parseMessageFile(messageFile);
    vector<Link> changes = parseChangesFile(changesFile);

    /* cout << "Topology File Contents:" << endl;
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
    cout << endl; */
    
    map<string, map<string, int>> lsdb; // Link State Database: key(node) -> value(neighbor, cost)
    for (const Link& link : topology) {
        lsdb[link.node1][link.node2] = link.cost;
        lsdb[link.node2][link.node1] = link.cost; // Add reverse link for undirected graph
    }

    // Routing Tables: key(node) -> value(destination, (nextHop, cost))
    map<string, map<string, pair<string, int>>> routingTables;

    // Fill routingTables based on LSDB
    for (const auto& entry : lsdb) {

        string node = entry.first;
        routingTables[node][node] = make_pair(node, 0); // Node's entry for itself

        for (const auto& neighbor : entry.second) {
            routingTables[node][neighbor.first] = make_pair(neighbor.first, neighbor.second);
        }

    }

    // Set default cost for nodes not present in LSDB
    for (const auto& entry : lsdb) {

        string node = entry.first;

        for (const auto& pair : lsdb) {

            string neighbor = pair.first;
            if (node != neighbor && routingTables[node].find(neighbor) == routingTables[node].end()) {
                routingTables[node][neighbor] = make_pair("", INT_MAX); // Set cost to infinity
            }

        }

    }

    updateRoutingTables(lsdb, routingTables);

    /* 

    for (const auto& routingTable : routingTables) {
        std::cout << "Routing table for node " << routingTable.first << ":\n";
        for (const auto& entry : routingTable.second) {
            std::cout << "" << entry.first << " " << entry.second.first << " " << entry.second.second << "\n";
        }
        std::cout << "\n";
    }
    
    */
    
    ofstream outfile(outputFile);
    if (outfile.is_open()) {

        for (const auto& routingTable : routingTables) {

            for (const auto& entry : routingTable.second) {
                outfile << entry.first << " " << entry.second.first << " " << entry.second.second << "\n";
            }

            outfile << "\n";

        }  

        for (const auto& message : messages) {

            // Find shortest path from source to destination
            string shortestPath;
            int totalCost = 0; // Initialize total cost to zero

            if (routingTables.find(message.source) != routingTables.end() && routingTables[message.source].find(message.destination) != routingTables[message.source].end()) {
                
                totalCost = routingTables[message.source][message.destination].second; // Use cost from routing table
                string currentNode = message.destination; // Start from destination

                while (currentNode != message.source) {
                    if (currentNode != message.destination) {
                        shortestPath = currentNode + " " + shortestPath;
                    }
                    currentNode = routingTables[message.source][currentNode].first;
                }
            }

            outfile << "from " << message.source << " to " << message.destination << " cost " << totalCost << " hops " << message.source << " " << shortestPath << message.content << "\n";
        }

        outfile << "\n";

        // Apply changes
        for (const auto& change : changes) {

            // Check if the link should be added or removed
            auto it = find_if(topology.begin(), topology.end(), [&](const Link& l) { return l.node1 == change.node1 && l.node2 == change.node2; });
            
            if (it != topology.end()) {
                // Link exists, remove it
                topology.erase(it);
            } else {
                // Link does not exist, add it
                topology.push_back(change);
            }

            // Update routing tables based on modified topology
            lsdb.clear();
            routingTables.clear();

            for (const auto& link : topology) {
                lsdb[link.node1][link.node2] = link.cost;
                lsdb[link.node2][link.node1] = link.cost; // Add reverse link for undirected graph
            }

            // Add entry for each node to itself with distance 0
            for (const auto& entry : lsdb) {
                string node = entry.first;
                routingTables[node][node] = make_pair(node, 0); // Node's entry for itself
            }

            // Update routing tables based on the modified topology
            updateRoutingTables(lsdb, routingTables);

            vector<string> nodeIds;

            for (const auto& entry : routingTables) {
                nodeIds.push_back(entry.first);
            }
            sort(nodeIds.begin(), nodeIds.end());

            for (const auto& nodeId : nodeIds) {

                for (const auto& entry : routingTables[nodeId]) {
                    outfile << entry.first << " " << entry.second.first << " " << entry.second.second << "\n";
                }

                outfile << "\n";
            }

            // Output messages based on the modified topology
            for (const auto& message : messages) {

                // Find shortest path from source to destination
                string shortestPath;
                int totalCost = 0; // Initialize total cost to zero

                if (routingTables.find(message.source) != routingTables.end() && routingTables[message.source].find(message.destination) != routingTables[message.source].end()) {
                    totalCost = routingTables[message.source][message.destination].second; // Use cost from routing table
                    string currentNode = message.destination; // Start from destination

                    while (currentNode != message.source) {

                        if (currentNode != message.destination) {
                            shortestPath = currentNode + " " + shortestPath;
                        }

                        currentNode = routingTables[message.source][currentNode].first;

                    }
                }

                outfile << "from " << message.source << " to " << message.destination << " cost " << totalCost << " hops " << message.source << " " << shortestPath << message.content << "\n";
                outfile << endl;

            }

        }

        outfile.close();
    } else {
        cerr << "Unable to open output file." << endl;
    }

}

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

