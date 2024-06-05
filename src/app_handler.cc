#include "app_handler.h"
#include "logging_buffer.h"
#include <fstream>
#include <chrono>
#include <thread>
#include <string>



AppHandler::AppHandler(LoggingBuffer* logging_buffer) : RequestHandler(), logging_buffer_(logging_buffer) {}


http::response<http::vector_body<char>> AppHandler::handle_request(const http::request<http::vector_body<char>>& req){
   
    if (req.method() == http::verb::post) {
      return process_post(req);
    } else if (req.method() == http::verb::get) {
      return generate_landing_page (req);;
    } else {
        std::vector<char> body;
        http::response<http::vector_body<char>> response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, body);
        response.prepare_payload();
        return response;
    }
    
}

http::response<http::vector_body<char>> AppHandler::process_post(const http::request<http::vector_body<char>>& req){

    //Parse the relevant info and images
    std::string relevant_info = get_relevant_information(std::string(req.body().begin(), req.body().end()));
    std::vector<std::string> image_data = get_images(std::string(req.body().begin(), req.body().end()));

    //Choose best image and generate caption
    int best_image_index = get_best_image_index(image_data);
    std::string caption = generate_caption(image_data[best_image_index], relevant_info);


    //Create the JSON
    std::string json = "{\"caption\":\"" + caption + "\", \"best_image_index\":" + std::to_string(best_image_index) + "}";

    //Create the response

    std::vector<char> response_body_vector =std::vector<char>(json.begin(), json.end());

    auto response = http::response<http::vector_body<char>>(http::status::ok, 11U, response_body_vector);

    response.set(http::field::content_type, "application/json");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));

    response.prepare_payload();

    logging_buffer_->addToBuffer(INFO, "Going to sleep");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    logging_buffer_->addToBuffer(INFO, "Now awake");

    return response;
}

int AppHandler::get_best_image_index(const std::vector<std::string>& image_data){
    //TODO Implement this function
    return 0;
}

std::string AppHandler::generate_caption(const std::string& image, const std::string& relevant_info){
    //TODO Implement this function
    return "dummy caption";
}


std::string AppHandler::get_relevant_information(const std::string& body){
    std::string target = "Content-Disposition: form-data; name=\"relevant-info\"\r\n\r\n";
    int index = body.find(target) + target.length();
    std::string relevant_info = body.substr(index, body.find("\r\n", index) - index);
    return relevant_info;
}

std::vector<std::string> AppHandler::get_images(std::string body){

    //Determine the number of images
    std::string target = "Content-Disposition: form-data; name=\"number-of-images\"\r\n\r\n";
    int index = body.find(target) + target.length();
    int number_of_images = std::stoi(body.substr(index, body.find("\r\n", index) - index));

    //Determine the image data
    std::vector <std::string > images;

    for (int i=0; i<number_of_images; i++){
        //Find the satrting point
        target = "Content-Disposition: form-data; name=\"file-upload\"";
        index = body.find(target) + target.length();
        index = body.find("\r\n", index) + 2;
        index = body.find("\r\n", index) + 2;
        index = body.find("\r\n", index) + 2;

        //Add the image data to the vector
        std::string image = body.substr(index, body.find("\r\n------WebKitFormBoundary", index) - index);
        images.push_back(image);

        //Update the body
        body = body.substr(body.find("\r\n", index) + 2);

    }

    return images;
}

http::response<http::vector_body<char>> AppHandler::generate_landing_page(const http::request<http::vector_body<char>>& req){
    http::response<http::vector_body<char>> response;

    std::string file_path;
    std::string content_type;

    if (req.target() == "/app"){
        file_path = "../app_pages/index.html";
        content_type = "text/html";
    }
    else if (req.target() == "/app/landing_page.js"){
        file_path = "../app_pages/landing_page.js";
        content_type = "application/javascript";
    }
    else if (req.target() == "/app/styles.css"){
        file_path = "../app_pages/styles.css";
        content_type = "text/css";
    }
    else {
        std::vector<char> body;
        response = http::response<http::vector_body<char>>(http::status::bad_request, 11U, body);
        response.prepare_payload();
        return response;
    }

    std::ifstream file(file_path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        logging_buffer_->addToBuffer(ERROR, "File not found");
        response = http::response<http::vector_body<char>>(http::status::not_found, 11U);
        response.prepare_payload();
        return response;
    }

    // Get file size
    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    logging_buffer_->addToBuffer(INFO, "Obtained file size of " + std::to_string(file_size));

    // Allocate buffer for file
    std::vector<char> res(file_size);

    // Read file content into the response buffer after the header
    if (!file.read(res.data(), file_size)) {
        // Failed to read file
        file.close();
        logging_buffer_->addToBuffer(ERROR, "Failed to read file");
    }

    response = http::response<http::vector_body<char>>(http::status::ok, 11U, res);
    response.set(http::field::content_type, content_type);
    response.set(http::field::content_length, std::to_string(res.size()));

    response.prepare_payload(); 


    return response;
}


RequestHandler* AppHandlerFactory::create(LoggingBuffer* logging_buffer) {
    return new AppHandler(logging_buffer);
}
