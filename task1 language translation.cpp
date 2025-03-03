#include <iostream>
#include <curl/curl.h>
#include <json/json.h>

std::string translate_text(const std::string& text, const std::string& target_language) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        std::string url = "https://api.mymemory.translated.net/get?q=" + text + "&langpair=en|" + target_language;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
            ((std::string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    Json::Value jsonData;
    Json::Reader reader;
    if (reader.parse(readBuffer, jsonData)) {
        return jsonData["responseData"]["translatedText"].asString();
    }
    return "Translation failed";
}

int main() {
    std::string text, target_language;
    std::cout << "Enter text to translate: ";
    std::getline(std::cin, text);
    std::cout << "Enter target language code (e.g., fr for French, es for Spanish): ";
    std::cin >> target_language;
    
    std::string translated_text = translate_text(text, target_language);
    std::cout << "Translated text: " << translated_text << std::endl;
    
    return 0;
}
