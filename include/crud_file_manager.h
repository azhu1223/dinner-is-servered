#ifndef CRUD_FILE_MANAGER_H
#define CRUD_FILE_MANAGER_H

#include <string>

struct CrudPath {
  std::string data_path;
  std::string entity_name;
  std::string entity_id;
};
class CrudFileManager {
    public:
        // bool readObject(CrudPath file_path, std::string& json);
        bool writeObject(CrudPath file_path, std::string json);
};

#endif