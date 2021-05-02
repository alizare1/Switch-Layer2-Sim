#include <map>
#include <vector>
#include <string>

#include "../others/Constants.hpp"

class Switch {
public:
    Switch(int _id, int _ports);
    void run();

private:
    int id;
    int portsCount;
    int maxSd;
    fd_set masterSet;
    std::map<int, int> lookupTable; // system_id -> port_number
    std::map<int, int> writePorts; // port_number -> write_pipe
    std::map<int, int> readPorts; // read_pipe -> port_number
    void handleStdIn(std::string in);
    void handleFrame(char* frame, int readPortFD);
    void updateLookupTable(int src, int readPortFD);
    void forwardFrame(char* frame, int src, int dst);

};