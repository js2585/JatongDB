#include <iostream>
#include <vector>
#include <fstream>
#include "db.h"

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

MetaCommandResult doMetaCommand(std::string& input)
{
    if (input == ".exit")
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED;
    }
}

void executeInsert(Statement& statement, std::string_view filename, Table& table)
{
    std::ofstream wf;
    wf.open(filename, std::ios::binary | std::ios::out | std::ios::app);
    if (!wf)
    {
        std::cout << "Cannot open file\n";
        return;
    }
    wf.write(statement.rowToInsert.key.c_str(), KEY_SIZE);
    wf.write(statement.rowToInsert.value.c_str(), VALUE_SIZE);
    wf.close();
    table.numRows++;
}

void executeSelect(Statement& statement, std::string_view filename, Table& table)
{
    std::ifstream rf;
    rf.open(filename, std::ios::binary);
    if (!rf)
    {
        std::cout << "Cannot open file\n";
        return;
    }
    for (int i = 0; i < table.numRows; i++)
    {
        char keyString[KEY_SIZE]{};
        rf.read(keyString, KEY_SIZE);
        char valString[VALUE_SIZE]{};
        rf.read(valString, VALUE_SIZE);
        if (statement.keyToSelect == "*")
        {
            std::cout << "(" << keyString << ": " << valString << ")\n";
        }
        else if (statement.keyToSelect == keyString)
        {
            std::cout << "(" << keyString << ": " << valString << ")\n";
        }
    }
    rf.close();
}

Table& createTable(std::string_view filename)
{
    std::ifstream f;
    f.open(filename, std::ios::binary);
    f.seekg(0, std::ios::end);
    int fileSize = f.tellg();
    f.close();
    Table table = { fileSize / ROW_SIZE };
    return table;
}

int main()
{
    std::string input;
    std::string filename = "data.bin";
    Table table = createTable(filename);
    while (true)
    {
        std::cout << "db > ";
        std::getline(std::cin, input);
        if (input[0] == '.')
        {
            switch (doMetaCommand(input))
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