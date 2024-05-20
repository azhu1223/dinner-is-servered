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
