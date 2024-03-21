#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

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

void lsr(const string& topologyFile, const string& messageFile, const string& changesFile, const string& outputFile) {
    vector<Link> topology = parseTopologyFile(topologyFile);
    vector<Message> messages = parseMessageFile(messageFile);
    vector<Link> changes = parseChangesFile(changesFile);

    // Continue Link State Routing Protocol




    ofstream outfile(outputFile);
    if (outfile.is_open()) {
        // Write forwarding tables
        // Write messages
        outfile.close();
    } else {
        cerr << "Unable to open file: " << outputFile << endl;
    }
}

int main(int argc, char** argv) {

    cout << "entered main()\n" << endl;

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

    cout << "start opening lsr()\n" << endl;

    lsr(topologyFile, messageFile, changesFile, outputFile);

    return 0;

}