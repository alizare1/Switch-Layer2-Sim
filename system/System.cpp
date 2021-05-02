#include "System.hpp"
#include "../others/Utils.cpp"


System::System(int _id): id(_id) {
    FD_ZERO(&masterSet);
	FD_SET(0, &masterSet);
    maxSd = 5;
}


void System::run() {
    char in[100];
    char data[FRAME_SIZE];

    fd_set working_set;

	while(true) {
		working_set = masterSet;
        select(maxSd, &working_set, NULL, NULL, NULL); 

        for (int i = 0; i <= maxSd; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i == 0) { // stdin
                    read(0, in, 100);
					handleStdIn(in);
                }
                else if (i == readPipe) { // switch
					read(i, data, FRAME_SIZE);
                    handleFrame(data);
                }
            }
        }
	}
}

void System::handleStdIn(string in) {
    vector<string> tokenizedInput = tokenizeByChar(in, '#');
    if (tokenizedInput[0] == "connect") {
        string wPipe = "fifos/" + tokenizedInput[1];
        string rPipe = "fifos/" + tokenizedInput[2];
        writePipe = open(wPipe.c_str(), O_RDWR);
        readPipe = open(rPipe.c_str(), O_RDWR);

        if (writePipe < 0 || readPipe < 0)
            cout << "System " << id << ": Error in openning pipe" << endl;

        FD_SET(readPipe, &masterSet);
        maxSd = readPipe + 1;
    } 
    else if (tokenizedInput[0] == "send") {
        sendFile(stoi(tokenizedInput[1]), tokenizedInput[2]);
    }
    else if (tokenizedInput[0] == "recv") {
        sendFileReq(stoi(tokenizedInput[1]), tokenizedInput[2]);
    }
}

bool System::isFileReq(char* frame) {
    return getFileSizeFromFrame(frame) == FILE_REQ;
}

void System::handleFrame(char* frame) {
    int dst = readNumber(frame, 0, 3);
    int src = readNumber(frame, 3, 3);

    if (dst != id) // ignore frames targeted for other systems
        return;

    if (isFileReq(frame)) { // src is requesting a file
        sendFile(src, getFileNameFromFrame(frame));
    } 
    else { // src wants to send a file
        recvFile(getFileNameFromFrame(frame), getFileSizeFromFrame(frame));
    }
}

void System::recvFile(string fileName, int fsize) {
    char data[FRAME_SIZE];
    string sysDir = "system" + to_string(id);
    mkdir(sysDir.c_str(), 0777);
    fileName = "./" + sysDir + "/" + fileName;
    int f = open(fileName.c_str(), O_CREAT | O_RDWR, 0777);

    int totalRead = 0;
    while (totalRead < fsize) {
        read(readPipe, data, FRAME_SIZE);
        int recvSize = readNumber(data, 6, 4);
        totalRead += recvSize;
        write(f, data+10, recvSize);
    }
    cout << "System " + to_string(id) + ": File received" << endl;
    close(f);
}

void System::sendFile(int dst, string fileName) {
    if (writePipe == 0) {
        cout << "System is not connected to any switch!" << endl;
        return;
    }

    int f = open(fileName.c_str(), O_RDONLY);
    struct stat st;
    fstat(f, &st);
    int fsize = st.st_size;

    char data[FRAME_SIZE];

    // send file info
    writeNumber(data, dst, 0, 3); // set dst
    writeNumber(data, id, 3, 3); // set src
    int s = writeFileInfo(data, fileName, fsize); // set file info
    writeNumber(data, s, 6, 4); // set payload size
    write(writePipe, data, FRAME_SIZE);

    // send file
    char fdata[FRAME_SIZE];
    int written = 0;
    while (written < fsize) {
        int last_read = 0;
        last_read = read(f, fdata, FRAME_SIZE - 10);
        writeNumber(data, last_read, 6, 4);
        for (int i = 0; i < last_read; i++)
            data[10+i] = fdata[i];
        written += last_read;
        write(writePipe, data, FRAME_SIZE);
    }

    cout << "System " + to_string(id) + ": File sent" << endl;
    close(f);
}

void System::sendFileReq(int dst, string fileName) {
    if (writePipe == 0) {
        cout << "System is not connected to any switch!" << endl;
        return;
    }

    char data[FRAME_SIZE];

    // send req info
    writeNumber(data, dst, 0, 3); // set dst
    writeNumber(data, id, 3, 3); // set src
    int s = writeFileInfo(data, fileName, FILE_REQ); // set file info
    writeNumber(data, s, 6, 4); // set payload size
    write(writePipe, data, FRAME_SIZE);

}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "missing args" << endl;
        exit(EXIT_FAILURE);
    }

    System sys = System(atoi(argv[1]));
    sys.run();
    
    return 0;
}