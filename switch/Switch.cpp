#include "Switch.hpp"
#include "../others/Utils.cpp"


Switch::Switch(int _id, int _ports): id(_id), portsCount(_ports) {
    FD_ZERO(&masterSet);
	FD_SET(0, &masterSet);
    for (int i = 0; i < portsCount; i++) {
        string inName = "fifos/s" + to_string(id) + "-" + to_string(i+1) + "-" + "in";
        string outName = "fifos/s" + to_string(id) + "-" + to_string(i+1) + "-" + "out";
        mkfifo(inName.c_str(), 0666);
        mkfifo(outName.c_str(), 0666);

        int in = open(inName.c_str(), O_RDWR);
        int out = open(outName.c_str(), O_RDWR);

        if (in < 0 || out < 0)
            cout << "Switch " << id << ": Error in openning pipe" << endl;

        readPorts[in] = i+1;
        writePorts[i+1] = out;
        FD_SET(in, &masterSet);
        maxSd = out;
    }
}

void Switch::run() {
    char in[100];
    char data[FRAME_SIZE];

    fd_set working_set;

	while(true) {
		working_set = masterSet;
        select(1020, &working_set, NULL, NULL, NULL); 

        for (int i = 0; i <= 1020; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i == 0) { // stdin
                    read(0, in, 100);
					handleStdIn(in);
                }
                else { // ports
					read(i, data, FRAME_SIZE);
                    handleFrame(data, i);
                }
            }
        }
	}
}

void Switch::handleStdIn(string in) {
    vector<string> tokenizedInput = tokenizeByChar(in, '#');
    if (tokenizedInput[0] == "connects") {
        close(writePorts[stoi(tokenizedInput[2])]);
        int fifo = open(("fifos/" + tokenizedInput[1]).c_str(), O_RDWR);

        if (fifo < 0)
            cout << "System " << id << ": Error in openning pipe" << endl;

        writePorts[stoi(tokenizedInput[2])] = fifo;
        maxSd = fifo;
    }
}

void Switch::handleFrame(char* frame, int readPortFD) {
    int dst = readNumber(frame, 0, 3);
    int src = readNumber(frame, 3, 3);

    if (dst == STP) { // STP
        ; // TODO
    } else { // Data to forward
        cout << "SWITCH" << id << " RECV FRAME FROM " << src << " TO " << dst << " ON PORT " << readPorts[readPortFD] << endl;
        updateLookupTable(src, readPortFD);
        forwardFrame(frame, src, dst);
    }

}

void Switch::updateLookupTable(int srcId, int readPortFD) {
    if (lookupTable.count(srcId))
        return;

    lookupTable[srcId] = readPorts[readPortFD];
}

void Switch::forwardFrame(char* frame, int src, int dst) {
    if (lookupTable.count(dst)) {
        write(writePorts[lookupTable[dst]], frame, FRAME_SIZE);
    } 
    else { // forward to all ports
        for (int port = 1; port <= portsCount; port++) {
            if (lookupTable[src] == port) // dont resend to src
                continue;

            write(writePorts[port], frame, FRAME_SIZE);
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "missing args" << endl;
        exit(EXIT_FAILURE);
    }

    Switch sw = Switch(atoi(argv[1]), atoi(argv[2]));
    sw.run();

    return 0;
}