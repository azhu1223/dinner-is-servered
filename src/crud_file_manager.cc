#include "crud_file_manager.h"
#include "crud_handler.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <boost/log/trivial.hpp>

bool CrudFileManager::readObject(CrudPath path, std::string& json) {
  std::string file_path = path.data_path + "/" + path.entity_name + "/" + path.entity_id;
  std::ifstream file(file_path, std::ios::in | std::ios::binary); 
  if (!file.is_open()) {
    BOOST_LOG_TRIVIAL(info) << "unable to open file " << file_path;
    return false;
  }
  
  std::ostringstream os;
  os << file.rdbuf();
  std::string object = os.str();
  json = object;

  file.close();
  BOOST_LOG_TRIVIAL(info) << "found read success: " << file_path;
  return true;
}

bool CrudFileManager::writeObject(CrudPath path, std::string json) {
  std::string directory = path.data_path + "/" + path.entity_name;
  if (!std::filesystem::exists(directory)) {
    try {
      BOOST_LOG_TRIVIAL(info) << "directory " << directory << " missing, attempting to create";
      std::filesystem::create_directories(directory);
    } catch (std::filesystem::filesystem_error const& ex) {
      BOOST_LOG_TRIVIAL(error) << "error creating directory " << directory << " for CRUD request";
    }
  }
  std::string file_path = path.data_path + "/" + path.entity_name + "/" + path.entity_id;
  BOOST_LOG_TRIVIAL(info) << "creating Entity " << path.entity_name << " at path " << file_path;

  std::ofstream file(file_path); 
  if (!file.is_open()) {
    return false;
  }

  file << json;
  file.close();
  BOOST_LOG_TRIVIAL(info) << "file creation success";

  return true;
}

bool CrudFileManager::deleteObject(CrudPath path) {
  std::string file_path = path.data_path + "/" + path.entity_name + "/" + path.entity_id;
  bool exists = existsObject(path); 

  if (!exists) {
    BOOST_LOG_TRIVIAL(info) << "file doesn't exist: " << file_path;
    return false; // This object doesn't exist 
  }

  try {
    return std::filesystem::remove(file_path);
  } catch (const std::filesystem::filesystem_error& e) {
    return false;
  }
}

bool CrudFileManager::existsObject(CrudPath path) {
  std::string file_path = path.data_path + "/" + path.entity_name + "/" + path.entity_id;
  
  try {
    return std::filesystem::exists(file_path);
  } catch (const std::filesystem::filesystem_error& e) {
    return false;
  }
}