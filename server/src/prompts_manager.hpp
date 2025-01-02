#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>

class PromptsManager
{
public:
    static PromptsManager &getInstance()
    {
        static PromptsManager instance;
        return instance;
    }

    void Init(const std::string &filePath)
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open prompts file");
        }

        std::string line;
        while (std::getline(file, line))
        {
            if (!line.empty())
            {
                words.push_back(line);
            }
        }

        file.close();
    }

    std::vector<std::string> GetPrompts()
    {
        if (words.size() < 3)
        {
            throw std::runtime_error("Not enough words to select from");
        }

        std::vector<std::string> selectedPrompts;
        std::sample(words.begin(), words.end(), std::back_inserter(selectedPrompts),
                    3, rng);

        return selectedPrompts;
    }

private:
    PromptsManager() : rng(std::random_device{}()) {}
    ~PromptsManager() noexcept = default;
    PromptsManager(const PromptsManager &) = delete;
    PromptsManager &operator=(const PromptsManager &) = delete;

    std::vector<std::string> words;
    std::mt19937 rng;
};