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
