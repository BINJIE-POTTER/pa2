/**

@mainpage ELEC 331 Programming Assignment 2

@author Kaiwen Deng  Xinyue Ma  Binjie Ye

Our program works by first co,piling the C++ code and run it through the command line. 
You can first call g++ -std=c++11 lsr.cpp -o lsr and g++ -std=c++11 lsr.cpp -o dvr to
compike the link state routing and distance vector routing and then use 
./lsr <topologyFile> <messageFile> <changesFile> and ./dvr <topologyFile> <messageFile> <changesFile> 
to execute the file. 

The project is relatively easier than the first assignment probably due to the variety of
languages we can choose this time. We like the project and think this is a interesting project.

*/


For lsr.cpp:
1. Parses the input topology file, message file, and changes file with the parseTopologyFile, parseMessageFile, and parseChangesFile functions, storing them in topology, messages, and changes, respectively.

2. Initialise an empty Link State Database (LSDB) and an empty Routing Tables.

3. Populate the LSDB with information from the topology file and add entries in the Routing Tables for each node with a distance of 0 to itself.

4. Calculate the shortest paths and distances from each node to other nodes using Dijkstra's algorithm.

5. Output the information in the Routing Tables to the specified output file.

6. For each change operation in the change file, modify the topology according to the change operation, then recalculate the LSDB and Routing Tables and output the modified Routing Tables information to the output file.

7. For each message in the message file, find the shortest path and distance from the source node to the target node according to the information in the Routing 
Tables, and output the message content in the specified format to the output file.

8. Finally, close the output file and the procedure ends.
