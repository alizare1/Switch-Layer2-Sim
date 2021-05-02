#include <map>
#include <vector>
#include <string>

class Interface {
public:
    void run();

private:
    std::map<int, int> switches; // id -> pipe
    std::map<int, int> systems; // id -> pipe
    std::vector<std::string> tokenizeInput(std::string input);
    void addSwitch(std::string input);
    void addSystem(std::string input);
    void connect(std::string input);
    void sendFile(std::string input);
    void recvFile(std::string input);
    void connectSwitch(std::string input);
    void runStp();

};