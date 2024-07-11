#include "leveldb/db.h"

#include "gtest/gtest.h"

TEST(LevelDB, Open) {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    EXPECT_TRUE(status.ok());
    delete db;
}