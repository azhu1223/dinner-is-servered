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

    std::string test_directory = "../data/WriteFileTest";
    int deleted = std::filesystem::remove_all(file_path);
    EXPECT_TRUE(file_opened);
}
