#include <iostream>
#include <unordered_map>
#include <string>
#include <algorithm>

// Sample FAQ dataset
std::unordered_map<std::string, std::string> faq_data = {
    {"what is your name?", "I am a chatbot designed to answer FAQs."},
    {"how can i reset my password?", "To reset your password, go to the settings page and click on 'Reset Password'."},
    {"what are your working hours?", "Our support is available 24/7 to assist you."},
    {"where are you located?", "We are an online service and do not have a physical location."}
};

std::string to_lower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

std::string get_response(const std::string& user_input) {
    std::string lower_input = to_lower(user_input);
    for (const auto& pair : faq_data) {
        std::string lower_key = to_lower(pair.first);  
        if (lower_input == lower_key) {
            return pair.second;
        }
    }
    return "I'm sorry, I don't understand your question.";
}

int main() {
    std::string user_input;
    while (true) {
        std::cout << "You: ";
        std::getline(std::cin, user_input);
        if (to_lower(user_input) == "exit") {
            std::cout << "Chatbot: Goodbye!" << std::endl;
            break;
        }
        std::cout << "Chatbot: " << get_response(user_input) << std::endl;
    }
    return 0;
}

