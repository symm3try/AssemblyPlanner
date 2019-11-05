#include <string>

enum class NodeType{AND, OR};

class NodeData{
public:
    double cost = 0;
    NodeType type;

    std::string name = "";
    std::string worker = "";

    bool terminal = true;
    bool marked = false;
    bool solved = false;;
    bool solution = false;

    void reset();
};

void NodeData::reset(){
    // marked = false;
    solved = false;
    solution = false;
}

typedef std::size_t EdgeData;
