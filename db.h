#ifndef DB_H
#define DB_H
#include <iostream>
#include "btree.h"

enum StatementType
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
};

struct Row
{
    std::string key;
    std::string value;
};

const int KEY_SIZE = 50;
const int VALUE_SIZE = 50;
const int ROW_SIZE = KEY_SIZE + VALUE_SIZE;

struct Statement
{
    StatementType type;
    Row rowToInsert;
    std::string keyToSelect;
};

enum PrepareResult
{
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
};

enum MetaCommandResult
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED,
    META_COMMAND_FAIL
};

struct Table
{
    Btree* btree;
};

std::vector<std::string> getArgs(std::string& input, int numArgs);
PrepareResult prepareStatement(std::string& input, Statement& statement);
void writeEntireBtree(NodeGroup* curr, std::ofstream& wf);
void freeBtree(NodeGroup* curr);
MetaCommandResult doMetaCommand(std::string& input, std::string_view filename, Table* table);
void executeInsert(Statement& statement, std::string_view filename, Table* table);
void printEntireBtree(NodeGroup* curr);
void executeSelect(Statement& statement, std::string_view filename, Table* table);
Table* createTable(std::string_view filename);

#endif