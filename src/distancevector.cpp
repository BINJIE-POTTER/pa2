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

struct Link {
    int node1;
    int node2;
    int pathCost;
};

struct Message {
    int sourceID;
    int destinationID;
    std::string message;
};

class RoutingTable {
public:

    RoutingTable(int myID, const std::set<int> &nodes) {

        for (const int &id : nodes) {

            if (id == myID) {

                table[id] = std::make_pair(id, 0);

            } else {

                table[id] = std::make_pair(-1, 9999);

            }
        }
    }

    // this function can also be used to update the existing entry
    void 
    addRoute(int destinationID, int nextHopID, int pathCost) {

        table[destinationID] = std::make_pair(nextHopID, pathCost);

    }

    bool 
    contains(int destinationID) const {

        return table.find(destinationID) != table.end();

    }

    int 
    getNextHop(int destinationID) const {

        if (contains(destinationID)) {

            return table.at(destinationID).first;

        }

        return -1;

    }

    int 
    getPathCost(int destinationID) const {

        if (contains(destinationID)) {

            return table.at(destinationID).second;

        }

        return -1;

    }

    const std::map<int, std::pair<int, int>>&
    getRoutingTable() const {

        return table;

    }

private:
    // destination_ID : (next_hop_ID, cost)
    std::map<int, std::pair<int, int>> table;
};

class Router {
public:

    Router(int id, const std::set<int> &nodes) : ID(id), RT(id, nodes) {}

    void 
    addRoute(int destinationID, int nextHopID, int pathCost) {

        RT.addRoute(destinationID, nextHopID, pathCost);

    }

    int 
    getNextHop(int destinationID) const {

        return RT.getNextHop(destinationID);

    }

    int 
    getPathCost(int destinationID) const {

        return RT.getPathCost(destinationID);

    }

    const std::map<int, std::pair<int, int>>&
    getRoutingTable() const {

        return RT.getRoutingTable();
        
    }

    const int
    getID() const {

        return ID;

    }

private:
    int ID;
    RoutingTable RT;
};

Router&
getRouterByID (std::vector<Router> &routers, int ID) {

    for (auto &router : routers) {

        if (router.getID() == ID) return router;

    }

    throw std::runtime_error("Router with the specified ID not found.");

}

void 
removeLink(std::vector<Link> &links, const Link &linkToRemove) {

    auto it = std::remove_if(links.begin(), links.end(), [&linkToRemove](const Link &link) {
                                return (link.node1 == linkToRemove.node1 && link.node2 == linkToRemove.node2) ||
                                       (link.node1 == linkToRemove.node2 && link.node2 == linkToRemove.node1);
                            });

    links.erase(it, links.end());

}


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