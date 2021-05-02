#include <string>

#include "../others/Constants.hpp"

class System {
public:
    System(int _id);
    void run();

private:
    int id;
    int writePipe;
    int readPipe;
    int maxSd;
    fd_set masterSet;
    void handleStdIn(std::string in);
    void handleFrame(char* frame);
    void recvFile(std::string fileName, int fsize);
    void sendFile(int dst, std::string fileName);
    void sendFileReq(int dst, std::string fileName);
    bool isFileReq(char* frame);
};