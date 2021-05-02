#include "Interface.hpp"

#include <iostream>
#include <iterator>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

constexpr char ADD_SWITCH[] = "add_switch"; // add_switch <id> <portCount>
constexpr char ADD_SYSTEM[] = "add_system"; // add_system <id>
constexpr char CONNECT[] = "connect"; // connect <system_id> <switch_id> <port_number>
constexpr char SEND[] = "send"; // send <src system> <dst system> <file name>
constexpr char RECEIVE[] = "recv"; // recv <src system> <dst system> <file name>
constexpr char CONNECT_SWITCH[] = "connect_switch"; // connect_switch <s1_id> <s1_port> <s2_id> <s2_port>

// TODO: STP

void Interface::run() {
    mkdir("fifos", 0777);

    string input;
    while (getline(cin, input)) {
        const string commandType = tokenizeInput(input)[0];

        if (commandType == ADD_SWITCH) {
            addSwitch(input);
        }
        else if (commandType == ADD_SYSTEM) {
            addSystem(input);
        }
        else if (commandType == CONNECT) {
            connect(input);
        }
        else if (commandType == SEND) {
            sendFile(input);
        }
        else if (commandType == RECEIVE) {
            recvFile(input);
        }
        else if (commandType == CONNECT_SWITCH) {
            connectSwitch(input);
        }
        
        usleep(20000);
    }
}



vector<string> Interface::tokenizeInput(string input) {
    stringstream inputStringStream(input);
    return vector<string>(istream_iterator<string>(inputStringStream),
                          istream_iterator<string>());
}



void Interface::addSwitch(string input) {
    vector<string> tokenizedInput = tokenizeInput(input);
    int id = stoi(tokenizedInput[1]);
    int portsCount = stoi(tokenizedInput[2]);

    int p[2];
    pipe(p);
    if (fork() == 0) {
        close(p[1]);
        dup2(p[0], STDIN_FILENO);
        close(p[0]);
        execl("./switch.out", "switch", to_string(id).c_str(), to_string(portsCount).c_str(), NULL);
    }
    close(p[0]);
    switches[id] = p[1];
}

void Interface::addSystem(string input) {
    vector<string> tokenizedInput = tokenizeInput(input);
    int id = stoi(tokenizedInput[1]);

    int p[2];
    pipe(p);
    if (fork() == 0) {
        close(p[1]);
        dup2(p[0], STDIN_FILENO);
        close(p[0]);
        execl("./system.out", "system", to_string(id).c_str(), NULL);
    }
    close(p[0]);
    systems[id] = p[1];
}

void Interface::connect(string input) {
    vector<string> tokenizedInput = tokenizeInput(input);
    int systemId = stoi(tokenizedInput[1]);
    int switchId = stoi(tokenizedInput[2]);
    int portNumber = stoi(tokenizedInput[3]);

    string systemReadPipe = "s" + to_string(switchId) + "-" + to_string(portNumber) + "-out";
    string systemWritePipe = "s" + to_string(switchId) + "-" + to_string(portNumber) + "-in";
    string systemCommand = "connect#" + systemWritePipe + "#" + systemReadPipe + "#";
    
    write(systems[systemId], systemCommand.c_str(), systemCommand.size());
}

void Interface::connectSwitch(string input) {
    vector<string> tokenizedInput = tokenizeInput(input);
    int s1 = stoi(tokenizedInput[1]);
    int s1Port = stoi(tokenizedInput[2]);
    int s2 = stoi(tokenizedInput[3]);
    int s2Port = stoi(tokenizedInput[4]);

    string s1Pipe = "s" + to_string(s1) + "-" + to_string(s1Port) + "-in";
    string s2Pipe = "s" + to_string(s2) + "-" + to_string(s2Port) + "-in";

    string s1Command = "connects#" + s2Pipe + "#" + to_string(s1Port) + "#";
    string s2Command = "connects#" + s1Pipe + "#" + to_string(s2Port) + "#";

    write(switches[s1], s1Command.c_str(), s1Command.size());
    write(switches[s2], s2Command.c_str(), s2Command.size());
}

void Interface::sendFile(string input) {
    vector<string> tokenizedInput = tokenizeInput(input);
    int srcId = stoi(tokenizedInput[1]);
    int dstId = stoi(tokenizedInput[2]);
    string fileName = tokenizedInput[3];

    string command = "send#" + to_string(dstId) + "#" + fileName + "#";

    write(systems[srcId], command.c_str(), command.size());
}

void Interface::recvFile(string input) {
    vector<string> tokenizedInput = tokenizeInput(input);
    int srcId = stoi(tokenizedInput[1]);
    int dstId = stoi(tokenizedInput[2]);
    string fileName = tokenizedInput[3];

    string command = "recv#" + to_string(dstId) + "#" + fileName + "#";

    write(systems[srcId], command.c_str(), command.size());
}



int main(int argc, char *argv[]) {
    Interface interface = Interface();
    interface.run(); 

    return 0;
}