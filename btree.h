#ifndef BTREE
#define BTREE
struct Node
{
    std::string_view key;
    std::string_view val;
};

struct NodeGroup
{
    std::vector<Node*> nodes;
    std::vector<NodeGroup*> children;
};

class Btree
{
private:
    NodeGroup* root;
public:
    Btree();
    Node* rGet(NodeGroup* curr, std::string_view key);
    //get the node based on key
    Node* get(std::string_view key);
    //return node that is getting promoted, otherwise null
    Node* rAdd(NodeGroup* curr, std::string_view key, std::string_view val);
    //add node and return root
    NodeGroup* add(std::string_view key, std::string_view val);
    //remove node and return root
    NodeGroup* remove(std::string_view key);
    //deletes entire tree
    void clear();
    //returns if empty
    bool isEmpty();
    NodeGroup* getRoot();
};
#endif