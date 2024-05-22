#include <gtest/gtest.h>
#include "crud_file_manager.h"
#include "crud_handler.h"
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "config_interpreter.h"

class CrudFileManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = new CrudFileManager();
    }

    void TearDown() override {
        delete manager;
    }

    CrudFileManager* manager;
};


// (1) Testing writeObject 

TEST_F(CrudFileManagerTest, WriteFileTest) {
    CrudPath path;
    path.data_path = "../data";
    path.entity_name = "WriteFileTest";
    path.entity_id = "1"; 

    std::string data = "write file test";
    manager->writeObject(path, data);

    std::string file_path = "../data/WriteFileTest/1";
    std::ofstream file(file_path); 
    bool file_opened = file.is_open();
    file.close();

    std::string test_directory = "../data";
    int deleted = std::filesystem::remove_all(test_directory);
    EXPECT_TRUE(file_opened);
}

// (2) Testing readObject 

TEST_F(CrudFileManagerTest, ReadFileTest) {
    CrudPath path;
    path.data_path = "../data";
    path.entity_name = "ReadFileTest";
    path.entity_id = "1"; 

    std::string data = "read file test";
    manager->writeObject(path, data);

    std::string file_contents;
    bool read_success = manager->readObject(path, file_contents);

    std::string test_directory = "../data";
    int deleted = std::filesystem::remove_all(test_directory);
    EXPECT_EQ(file_contents, data);
}

// (3) Testing existsObject 

TEST_F(CrudFileManagerTest, ExistsObjectTest) {
    CrudPath path;
    path.data_path = "../data";
    path.entity_name = "ExistsObjectTest";
    path.entity_id = "1"; 

    std::string data = "write file test";
    manager->writeObject(path, data);

    // Created new file at file_path
    std::string file_path = "../data/ExistsObjectTest/1";
    std::ofstream file(file_path); 
    bool file_opened = file.is_open();
    file.close();

    // Confirm that this file does exist 
    bool exists = manager->existsObject(path); 
    EXPECT_TRUE(exists);

    // Delete newly created file 
    std::string test_directory = "../data";
    int deleted = std::filesystem::remove_all(test_directory);
}

TEST_F(CrudFileManagerTest, ExistsNoObjectTest) {
    CrudPath path;
    path.data_path = "../data";
    path.entity_name = "ExistsNoObjectTest";
    path.entity_id = "1"; 

    bool exists = manager->existsObject(path); 
    EXPECT_FALSE(exists);
}

// Testing deleteObject 

TEST_F(CrudFileManagerTest, DeleteObject) {
    CrudPath path;
    path.data_path = "../data";
    path.entity_name = "DeleteObject";
    path.entity_id = "1"; 

    std::string data = "delete file test";
    manager->writeObject(path, data);

    // Created new file at file_path
    std::string file_path = "../data/DeleteObjectTest/1";
    std::ofstream file(file_path); 
    bool file_opened = file.is_open();
    file.close();

    // Confirm that this file does exist 
    bool exists = manager->existsObject(path); 
    EXPECT_TRUE(exists);

    // Delete newly created file 
    bool deleted_file = manager->deleteObject(path); 
    EXPECT_TRUE(deleted_file); 
    EXPECT_FALSE(std::filesystem::exists(file_path));

    std::string test_directory = "../data";
    int deleted = std::filesystem::remove_all(test_directory);
}

TEST_F(CrudFileManagerTest, DeleteNonexistentObject) {
    CrudPath path;
    path.data_path = "../data";
    path.entity_name = "DeleteObject";
    path.entity_id = "1"; 

    std::string file_path = "../data/DeleteObjectTest/1";

    // Delete newly created file 
    bool deleted_file = manager->deleteObject(path); 
    EXPECT_FALSE(deleted_file); 
    EXPECT_FALSE(std::filesystem::exists(file_path));

    std::string test_directory = "../data";
    int deleted = std::filesystem::remove_all(test_directory);
}

// Testing listObjects 

TEST_F(CrudFileManagerTest, ListObjectsNonePresentTest) {
    CrudPath path;
    path.data_path = "../data";
    path.entity_name = "ListObject";
    path.entity_id = "1"; 

    std::string dir_path = "../data/ListObject";

    // Directory (and no files) exist! 
    std::vector<std::string> file_list = manager->listObjects(path); 
    EXPECT_EQ(file_list.size(), 0); 
}

TEST_F(CrudFileManagerTest, ListObjectsOnePresentTest) {
    CrudPath path;
    path.data_path = "../data";
    path.entity_name = "ListObject";
    path.entity_id = "1"; 

    bool write_one = manager->writeObject(path, "thing one"); 

    std::vector<std::string> file_list = manager->listObjects(path); 
    EXPECT_EQ(file_list.size(), 1);
    EXPECT_EQ(file_list[0], "1");

    std::string test_directory = "../data";
    int deleted = std::filesystem::remove_all(test_directory);
}

TEST_F(CrudFileManagerTest, ListObjectsMultiplePresentTest) {
    CrudPath path;
    path.data_path = "../data";
    path.entity_name = "ListObject";
    path.entity_id = "1"; 
    bool write_one = manager->writeObject(path, "thing one"); 

    path.entity_id = "2";
    bool write_two = manager->writeObject(path, "thing two"); 

    path.entity_id = "3";
    bool write_three = manager->writeObject(path, "thing three"); 

    std::vector<std::string> file_list = manager->listObjects(path);
    EXPECT_EQ(file_list.size(), 3);
    EXPECT_EQ(file_list[0], "1");
    EXPECT_EQ(file_list[1], "2");
    EXPECT_EQ(file_list[2], "3");

    std::string test_directory = "../data";
    int deleted = std::filesystem::remove_all(test_directory);
}

// Testing generateEntityId

TEST_F(CrudFileManagerTest, generateEntityIdForNonexistentPath) {
    CrudPath path;
    path.data_path = "../data";
    path.entity_name = "Test";

    EXPECT_EQ(manager->generateEntityId(path), "1");
}

TEST_F(CrudFileManagerTest, generateEntityIdAfterPut) {
    CrudPath path;
    path.data_path = "../data";
    path.entity_name = "Test";
    path.entity_id = "10";

    manager->writeObject(path, "dummy");

    EXPECT_EQ(manager->generateEntityId(path), "11");
    std::string test_directory = "../data";
    int deleted = std::filesystem::remove_all(test_directory);
}

TEST_F(CrudFileManagerTest, generateEntityIdAfterPost) {
    CrudPath path;
    path.data_path = "../data";
    path.entity_name = "Test";
    path.entity_id = "1";

    manager->writeObject(path, "dummy");

    EXPECT_EQ(manager->generateEntityId(path), "2");
    std::string test_directory = "../data";
    int deleted = std::filesystem::remove_all(test_directory);
}
