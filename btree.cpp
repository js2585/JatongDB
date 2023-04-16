#include <iostream>
#include <vector>
#include "btree.h"

Btree::Btree(): root{ nullptr } {}

Node* Btree::rGet(NodeGroup* curr, std::string_view key)
{
    Node* prev = nullptr;
    int numNodes = curr->nodes.size();
    int numChildren = curr->children.size();
    if (curr->nodes[numNodes - 1]->key.compare(key) < 0)
    {
        if (numChildren == 0)
        {
            return nullptr;
        }
        return rGet(curr->children[numChildren - 1], key);
    }
    for (int i = 0; i < numNodes; i++)
    {
        if (curr->nodes[i]->key.compare(key) == 0)
        {
            return curr->nodes[i];
        }
        if (curr->nodes[i]->key.compare(key) > 0 && (prev == nullptr || prev->key.compare(key) < 0))
        {
            if (numChildren == 0)
            {
                return nullptr;
            }
            return rGet(curr->children[i], key);
        }
        prev = curr->nodes[i];
    }
}

Node* Btree::get(std::string_view key)
{
    if (isEmpty())
    {
        return nullptr;
    }
    return rGet(root, key);
}

Node* Btree::rAdd(NodeGroup* curr, std::string_view key, std::string_view val)
{
    if (root == nullptr)
    {
        Node* node = new Node{ key, val };
        NodeGroup* nodeGroup = new NodeGroup{ {node}, {} };
        root = nodeGroup;
        return nullptr;
    }
    Node* prev = nullptr;
    int numNodes = curr->nodes.size();
    int numChildren = curr->children.size();
    int idx = -1;
    if (curr->nodes[numNodes - 1]->key.compare(key) < 0)
    {
        idx = numNodes;
    }
    for (int i = 0; i < numNodes; i++)
    {
        if (curr->nodes[i]->key.compare(key) == 0)
        {
            curr->nodes[i]->val = val;
            return nullptr;
        }
        if (curr->nodes[i]->key.compare(key) > 0 && (prev == nullptr || prev->key.compare(key) < 0))
        {
            idx = i;
            break;
        }
        prev = curr->nodes[i];
    }
    if (numChildren == 0)
    {
        curr->nodes.insert(curr->nodes.begin() + idx, new Node{ key, val });
        if (curr->nodes.size() >= 4)
        {
            curr->nodes.erase(curr->nodes.begin() + 1);
            return curr->nodes[1];
        }
        return nullptr;
    }
    Node* promoted = rAdd(curr->children[idx], key, val);
    if (promoted == nullptr)
    {
        return nullptr;
    }
    curr->nodes.insert(curr->nodes.begin() + idx, promoted);
    NodeGroup* toSplit = curr->children[idx];
    NodeGroup* leftSplit = new NodeGroup{ {}, {} };
    NodeGroup* rightSplit = new NodeGroup{ {}, {} };
    leftSplit->nodes.push_back(toSplit->nodes[0]);
    rightSplit->nodes.push_back(toSplit->nodes[1]);
    rightSplit->nodes.push_back(toSplit->nodes[2]);
    if (toSplit->children.size() > 0)
    {
        leftSplit->children.push_back(toSplit->children[0]);
        leftSplit->children.push_back(toSplit->children[1]);
        rightSplit->children.push_back(toSplit->children[2]);
        rightSplit->children.push_back(toSplit->children[3]);
        rightSplit->children.push_back(toSplit->children[4]);
    }
    curr->children.erase(curr->children.begin() + idx);
    curr->children.insert(curr->children.begin() + idx, leftSplit);
    curr->children.insert(curr->children.begin() + idx + 1, rightSplit);
    if (curr->nodes.size() >= 4)
    {
        curr->nodes.erase(curr->nodes.begin() + 1);
        return curr->nodes[1];
    }
    return nullptr;
}

NodeGroup* Btree::add(std::string_view key, std::string_view val)
{
    Node* promoted = rAdd(root, key, val);
    if (promoted != nullptr)
    {
        NodeGroup* curr = new NodeGroup{ {promoted}, {root} };
        NodeGroup* toSplit = root;
        NodeGroup* leftSplit = new NodeGroup{ {}, {} };
        NodeGroup* rightSplit = new NodeGroup{ {}, {} };
        leftSplit->nodes.push_back(toSplit->nodes[0]);
        rightSplit->nodes.push_back(toSplit->nodes[1]);
        rightSplit->nodes.push_back(toSplit->nodes[2]);
        if (toSplit->children.size() > 0)
        {
            leftSplit->children.push_back(toSplit->children[0]);
            leftSplit->children.push_back(toSplit->children[1]);
            rightSplit->children.push_back(toSplit->children[2]);
            rightSplit->children.push_back(toSplit->children[3]);
            rightSplit->children.push_back(toSplit->children[4]);
        }
        curr->children.pop_back();
        curr->children.push_back(leftSplit);
        curr->children.push_back(rightSplit);
        root = curr;
    }
    return root;
}

void Btree::clear()
{
    root = nullptr;
}

bool Btree::isEmpty()
{
    return root == nullptr;
}

NodeGroup* Btree::getRoot()
{
    return root;
}