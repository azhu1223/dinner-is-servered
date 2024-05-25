#include "crud_file_manager.h"
#include "crud_handler.h"
#include "logging_buffer.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <mutex>

CrudFileManager::CrudFileManager(LoggingBuffer* logging_buffer) : logging_buffer_(logging_buffer) {}

bool CrudFileManager::readObject(CrudPath path, std::string& json) {
  std::string file_path = path.data_path + "/" + path.entity_name + "/" + path.entity_id;
  std::ifstream file(file_path, std::ios::in | std::ios::binary); 
  if (!file.is_open()) {
    logging_buffer_->addToBuffer(ERROR, "unable to open file " + file_path);
    return false;
  }
  
  std::ostringstream os;
  os << file.rdbuf();
  std::string object = os.str();
  json = object;

  file.close();
  logging_buffer_->addToBuffer(INFO, "found read success: " + file_path);
  return true;
}

bool CrudFileManager::writeObject(CrudPath path, std::string json) {
  std::string directory = path.data_path + "/" + path.entity_name;
  if (!std::filesystem::exists(directory)) {
    try {
      logging_buffer_->addToBuffer(INFO, "directory " + directory + " missing, attempting to create");
      writeEntityId(path, 1);
      std::filesystem::create_directories(directory);
    } catch (std::filesystem::filesystem_error const& ex) {
      logging_buffer_->addToBuffer(ERROR, "error creating directory " + directory + " for CRUD request");
    }
  }

  std::string file_path = path.data_path + "/" + path.entity_name + "/" + path.entity_id;
  logging_buffer_->addToBuffer(INFO, "creating Entity " + path.entity_name + " at path " + file_path);
  if (path.entity_id != "" && (std::stoull(path.entity_id) >= std::stoull(generateEntityId(path)))) {
    writeEntityId(path, std::stoull(path.entity_id));
  }

  std::ofstream file(file_path); 
  if (!file.is_open()) {
    return false;
  }

  file << json;
  file.close();
  logging_buffer_->addToBuffer(INFO, "file creation success");

  return true;
}

bool CrudFileManager::deleteObject(CrudPath path) {
  std::string file_path = path.data_path + "/" + path.entity_name + "/" + path.entity_id;
  bool exists = existsObject(path); 

  if (!exists) {
    logging_buffer_->addToBuffer(ERROR, "file doesn't exist: " + file_path);
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

std::vector<std::string> CrudFileManager::listObjects(CrudPath path) {
  std::string dir_path = path.data_path + "/" + path.entity_name + "/";
  std::vector<std::string> file_names;

  try {
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
        if (entry.is_regular_file()) { // Ensure it's a file, not a directory
            file_names.push_back(entry.path().filename().string());
        }
    }
  } catch (const std::filesystem::filesystem_error& e) {
      logging_buffer_->addToBuffer(ERROR, "Unable to read directory at: " + dir_path);
  }

  sort(file_names.begin(), file_names.end());

  return file_names; 
}

std::string CrudFileManager::generateEntityId(CrudPath path) {
  entity_id_file_mutex.lock();
  std::string max_entity_ids_file = path.data_path + "/maxEntityIds";
  std::string directory = path.data_path;
  if (!std::filesystem::exists(directory)) {
    try {
      logging_buffer_->addToBuffer(INFO, "directory " + directory + " missing, attempting to create");
      std::filesystem::create_directories(directory);
    } catch (std::filesystem::filesystem_error const& ex) {
      logging_buffer_->addToBuffer(INFO, "error creating directory " + directory + " for CRUD request");
    }
  }

  if (!std::filesystem::exists(max_entity_ids_file)) {
    logging_buffer_->addToBuffer(INFO, "Max file doesn't exist");
    std::ofstream f(max_entity_ids_file);
    f.close();
  }
  std::fstream file(max_entity_ids_file);
  if (!file.is_open()) {
    logging_buffer_->addToBuffer(ERROR, "Could not open " + max_entity_ids_file + " to get max entity IDs");
    entity_id_file_mutex.unlock();
    return "1";
  }

  std::string line, stem;
  while (file >> line) {
    stem = line.substr(0, path.entity_name.length() + 1);
    if (stem == path.entity_name + ":") {
      file.close();
      entity_id_file_mutex.unlock();
      u_int64_t max_id = std::stoull(line.substr(path.entity_name.length() + 1));
      return std::to_string(max_id + 1);
    }
  }
  file.close();
  entity_id_file_mutex.unlock();

  return "1";
}

bool CrudFileManager::writeEntityId(CrudPath path, u_int64_t max_id) {
  entity_id_file_mutex.lock();
  std::string max_entity_ids_file = path.data_path + "/maxEntityIds";
  std::string dir_path = path.data_path + "/" + path.entity_name;
  std::string temp_file = "temp";

  // (1) When we are creating our first entity
  if (!std::filesystem::exists(path.data_path)) {
    try {
      logging_buffer_->addToBuffer(INFO, "directory " + path.data_path + " missing, attempting to create");
      std::filesystem::create_directories(path.data_path);
    } catch (std::filesystem::filesystem_error const& ex) {
      logging_buffer_->addToBuffer(ERROR, "error creating directory " + path.data_path + " for CRUD request");
    }
  }

  if (!std::filesystem::exists(max_entity_ids_file)) {
    std::fstream f(max_entity_ids_file);
    f.close();
  }

  if (!std::filesystem::exists(dir_path)) {
    std::ofstream file(max_entity_ids_file, std::ofstream::app);
    if (!file.is_open()) {
      logging_buffer_->addToBuffer(ERROR, "Could not open " + max_entity_ids_file + " to write to it.");
      entity_id_file_mutex.unlock();
      return false;
    }
    file << "\n" + path.entity_name + ":" + std::to_string(max_id);
    file.close();
    entity_id_file_mutex.unlock();
    return true;
  }

  // (2) Open files
  std::ifstream filein(max_entity_ids_file);
  std::ofstream fileout(temp_file);
  if(!filein || !fileout)
  {
    entity_id_file_mutex.unlock();
    return false;
  }

  // (3) Replace max entity id for entity name
  std::string line, stem;
  while (filein >> line) {
    stem = line.substr(0, path.entity_name.length() + 1);
    if (stem == path.entity_name + ":") { // Looking for exact match, so we can use : as an end token
      fileout << path.entity_name + ":" + std::to_string(max_id) + "\n";
      continue;
    }
    fileout << line + "\n";
  }
  filein.close();
  fileout.close();
  
  // (4) Copy temp file to actual file and delete temp
  std::filesystem::copy_file(temp_file, max_entity_ids_file, std::filesystem::copy_options::overwrite_existing);
  std::filesystem::remove(temp_file);
  entity_id_file_mutex.unlock();
  return true;
}