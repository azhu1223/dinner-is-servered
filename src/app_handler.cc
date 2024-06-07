
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>


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
    std::string json;


    //Parse the relevant info and images
    std::string relevant_info = get_relevant_information(std::string(req.body().begin(), req.body().end()));
    std::vector<std::string> image_data = get_images(std::string(req.body().begin(), req.body().end()));

    //Choose best image and generate caption
    int best_image_index = get_best_image_index(image_data);
    //Check that there was a valid image index
    if (best_image_index < 0 || best_image_index >= image_data.size() ){
        json = "{\"caption\":\"" + std::to_string(best_image_index) + "\"An error occured while selecting an image\",\"best_image_index\":-1}";
    }
    else{

        std::string caption = generate_caption(image_data[best_image_index], relevant_info);


        //Create the JSON
        std::string special_character_escaped_caption;
        for (char c : caption) {
            if (c == '"') {
                special_character_escaped_caption += "\\\"";
            } else if (c == '\\') {
                special_character_escaped_caption += "\\\\";
            } else if (c != '\n' && c!='\r' && c!= '\t') {
                special_character_escaped_caption += c;
            }
        }

        json = "{\"caption\":\"" + special_character_escaped_caption + "\", \"best_image_index\":" + std::to_string(best_image_index) + "}";
    }
    //Create the response

    std::vector<char> response_body_vector =std::vector<char>(json.begin(), json.end());

    auto response = http::response<http::vector_body<char>>(http::status::ok, 11U, response_body_vector);

    response.set(http::field::content_type, "application/json");
    response.set(http::field::content_length, std::to_string(response_body_vector.size()));

    response.prepare_payload();

    return response;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int AppHandler::get_best_image_index(const std::vector<std::string>& image_data){
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    int index = -1;
    int max_score = 0;

    for(int i = 0; i < image_data.size(); i++){
        curl = curl_easy_init();
        std::string index_str;

        if(curl) {
            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "Authorization: Bearer API_KEY_HERE");

            std::string jsonData = R"(
            {
                "model": "gpt-4-turbo",
                "messages": [
                    {
                        "role": "user",
                        "content": [
                            {
                                "type": "text",
                                "text": "On a scale of 0-100 with 0 being the worst and 100 being the best, please score this image on how good it is to be posted on Instagram. Respond only with an integer."
                            },
                            {
                                "type": "image_url",
                                "image_url": {
                                    "url": ")" +  image_data[i] + R"("
                                }
                            }
                        ]
                    }
                ],
                "max_tokens": 300
            }
            )";


            curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                index_str = "-1";
            } else {
                // Parse the JSON response
                try{
                    auto jsonResponse = nlohmann::json::parse(readBuffer);
                    index_str = jsonResponse["choices"][0]["message"]["content"];
                    int score = std::stoi(index_str);
                    if(score > max_score){
                        max_score = score;
                        index = i;
                    }
                }catch(const std::exception& e){
                    index_str = "-1";
                }
            }

            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
        }    
    }
    

    return index;
}


std::string AppHandler::generate_caption(const std::string& image, const std::string& relevant_info){
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::string caption = "";

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Authorization: Bearer API_KEY_HERE");

        std::string jsonData = R"(
        {
            "model": "gpt-4-turbo",
            "messages": [
                {
                    "role": "user",
                    "content": [
                        {
                            "type": "text",
                            "text": "Given the following image and relevant information, please generate an Instagram caption. Relevant information: )" + relevant_info +R"("
                        },
                        {
                            "type": "image_url",
                            "image_url": {
                                "url": ")" + image + R"("
                            }
                        }
                    ]
                }
            ],
            "max_tokens": 300
        }
        )";


        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            caption = "An error occured when generating a caption";
        } else {
            // Parse the JSON response
            try{
                auto jsonResponse = nlohmann::json::parse(readBuffer);
                caption = jsonResponse["choices"][0]["message"]["content"];
            }catch(const std::exception& e){
                caption = "An error occured while generating a caption";
            }
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();

    return caption;

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
        target = "Content-Disposition: form-data; name=\"file-upload\"\r\n\r\n";
        index = body.find(target) + target.length();

        //Add the image data to the vector
        std::string image = body.substr(index, body.find("\r\n", index) - index);
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
