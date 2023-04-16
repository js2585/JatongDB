#include <iostream>
#include <vector>
#include <fstream>
#include "db.h"
#include "btree.h"

std::vector<std::string> getArgs(std::string& input, int numArgs)
{
    std::vector<std::string> args;
    char* token = std::strtok(const_cast<char*>(input.c_str()), " ");
    token = std::strtok(nullptr, " ");
    while (token != nullptr)
    {
        args.push_back(std::string(token));
        token = std::strtok(nullptr, " ");
        if (args.size() == numArgs)
        {
            break;
        }
    }
    return args;
}
PrepareResult prepareStatement(std::string& input, Statement& statement)
{
    if (input.substr(0, 6) == "insert")
    {
        statement.type = STATEMENT_INSERT;
        std::vector<std::string> args = getArgs(input, 2);
        if (args.size() < 2)
        {
            return PREPARE_SYNTAX_ERROR;
        }
        Row row = { args[0], args[1] };
        statement.rowToInsert = row;
        return PREPARE_SUCCESS;
    }
    if (input.substr(0, 6) == "select")
    {
        statement.type = STATEMENT_SELECT;
        std::vector<std::string> args = getArgs(input, 1);
        if (args.size() < 1)
        {
            return PREPARE_SYNTAX_ERROR;
        }
        statement.keyToSelect = args[0];
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void writeEntireBtree(NodeGroup* curr, std::ofstream& wf)
{
    for (int i = 0; i < curr->nodes.size(); i++)
    {
        wf.write(curr->nodes[i]->key.data(), KEY_SIZE);
        wf.write(curr->nodes[i]->val.data(), KEY_SIZE);
    }
    for (int i = 0; i < curr->children.size(); i++)
    {
        writeEntireBtree(curr->children[i], wf);
    }
}

void freeBtree(NodeGroup* curr)
{
    for (int i = 0; i < curr->children.size(); i++)
    {
        freeBtree(curr->children[i]);
    }
    for (int i = 0; i < curr->nodes.size(); i++)
    {
        delete(curr->nodes[i]);
    }
    delete(curr);
}

MetaCommandResult doMetaCommand(std::string& input, std::string_view filename, Table* table)
{
    if (input == ".exit")
    {
        std::ofstream wf;
        wf.open(filename, std::ios::binary | std::ios::out | std::ios::app);
        if (!wf)
        {
            std::cout << "Cannot open file\n";
            return;
        }
        writeEntireBtree((*(table->btree)).getRoot(), wf);
        wf.close();
        freeBtree(table->btree->getRoot());
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED;
    }
}

void executeInsert(Statement& statement, std::string_view filename, Table* table)
{
    (*(table->btree)).add(statement.rowToInsert.key, statement.rowToInsert.value);
}

void printEntireBtree(NodeGroup* curr)
{
    for (int i = 0; i < curr->nodes.size(); i++)
    {
        std::cout << "(" << curr->nodes[i]->key << ": " << curr->nodes[i]->val << ")\n";
    }
    for (int i = 0; i < curr->children.size(); i++)
    {
        printEntireBtree(curr->children[i]);
    }
}

void executeSelect(Statement& statement, std::string_view filename, Table* table)
{
    if (statement.keyToSelect == "*")
    {
        printEntireBtree((*(table->btree)).getRoot());
    }
    else
    {
        Node* node = (*(table->btree)).get(statement.keyToSelect);
        if (node != nullptr)
        {
            std::cout << "(" << node->key << ": " << node->val << ")\n";
        }
    }
}

Table* createTable(std::string_view filename)
{
    std::ifstream f;
    f.open(filename, std::ios::binary);
    f.seekg(0, std::ios::end);
    int fileSize = f.tellg();
    f.close();
    Table* table = new Table{};
    int numRows = fileSize / ROW_SIZE;
    table->btree = new Btree();
    for (int i = 0; i < numRows; i++)
    {
        char keyString[KEY_SIZE]{};
        f.read(keyString, KEY_SIZE);
        char valString[VALUE_SIZE]{};
        f.read(valString, VALUE_SIZE);
        (*(table->btree)).add(keyString, valString);
    }
    return table;
}

int main()
{
    std::string input;
    std::string filename = "data.bin";
    Table* table = createTable(filename);
    while (true)
    {
        std::cout << "db > ";
        std::getline(std::cin, input);
        if (input[0] == '.')
        {
            switch (doMetaCommand(input, filename, table))
            {
            case META_COMMAND_SUCCESS:
                continue;
            case META_COMMAND_UNRECOGNIZED:
                std::cout << "Unrecognized command " << input << "\n";
                continue;
            }
        }
        Statement statement;
        switch (prepareStatement(input, statement))
        {
        case PREPARE_SUCCESS:
            break;
        case PREPARE_SYNTAX_ERROR:
            std::cout << "Syntax Error\n";
            continue;
        case PREPARE_UNRECOGNIZED_STATEMENT:
            std::cout << "Unrecognized Statement\n";
            continue;
        }
        switch (statement.type)
        {
        case STATEMENT_INSERT:
            executeInsert(statement, filename, table);
            break;
        case STATEMENT_SELECT:
            executeSelect(statement, filename, table);
            break;
        }
    }
}