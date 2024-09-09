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

TEST(LevelDB, PutGet) {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    EXPECT_TRUE(status.ok());

    status = db->Put(leveldb::WriteOptions(), "key", "value");
    EXPECT_TRUE(status.ok());

    std::string value;
    status = db->Get(leveldb::ReadOptions(), "key", &value);
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(value, "value");

    delete db;
}