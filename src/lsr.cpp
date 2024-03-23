#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <limits>


using namespace std;

struct Link {
    int node1;
    int node2;
    int cost;
};

struct Message {
    int source;
    int destination;
    string content;
};

// parse the topology file and store links in a vector
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

// parse the message file and store messages in a vector
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

// parse the changes file and store changes in a vector
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

// Perform Link State Routing (LSR)
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