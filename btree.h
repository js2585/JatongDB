#ifndef BTREE
#define BTREE
struct Node
{
    std::string key;
    std::string val;
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
    Node* rGet(NodeGroup* curr, std::string key);
    //get the node based on key
    Node* get(std::string key);
    //return node that is getting promoted, otherwise null
    Node* rAdd(NodeGroup* curr, std::string key, std::string val);
    //add node and return root
    NodeGroup* add(std::string key, std::string val);
    //remove node and return root
    NodeGroup* remove(std::string key);
    //deletes entire tree
    void clear();
    //returns if empty
    bool isEmpty();
    NodeGroup* getRoot();
};
#endif