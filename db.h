#ifndef DB_H
#define DB_H
#include <iostream>

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
    META_COMMAND_UNRECOGNIZED
};

struct Table
{
    int numRows;
};
#endif