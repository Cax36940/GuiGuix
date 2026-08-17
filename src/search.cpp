#include <cstring>
#include <string>
#include <string_view>
#include <vector>

static inline char lower_char(char c)
{
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}
struct Package
{
    std::string_view name;
    std::string_view version;
    std::string_view synopsis;
    std::string_view description;

    std::string_view name_lower;
    std::string_view synopsis_lower;
    std::string_view description_lower;
};

std::vector<Package> all_packages;
std::string all_packages_str;
std::string all_packages_lower_str;

const char *next_string(const char *&ptr)
{
    const char *result = ptr;
    ptr += std::strlen(ptr) + 1;
    return result;
}

void init_packages()
{
    FILE *pipe = popen("guix repl script/dump_packages.scm", "r");

    char buffer[4096];

    while (fgets(buffer, sizeof(buffer), pipe))
    {
        all_packages_str += buffer;
    }

    pclose(pipe);

    for (char &c : all_packages_str)
    {
        if (c == '\n')
            c = '\0';
    }

    const char *ptr = all_packages_str.c_str();

    all_packages_lower_str.reserve(all_packages_str.size());
    for (size_t i = 0; i < all_packages_str.size(); ++i)
    {
        all_packages_lower_str.push_back(lower_char(all_packages_str[i]));
    }
    const char *lower_ptr = all_packages_lower_str.c_str();

    while (*ptr)
    {
        std::string_view name = next_string(ptr);
        std::string_view version = next_string(ptr);
        std::string_view synopsis = next_string(ptr);
        std::string_view description = next_string(ptr);

        std::string_view name_lower(lower_ptr + (name.data() - all_packages_str.data()), name.size());
        std::string_view synopsis_lower(lower_ptr + (synopsis.data() - all_packages_str.data()), synopsis.size());
        std::string_view description_lower(lower_ptr + (description.data() - all_packages_str.data()), description.size());

        all_packages.push_back({name, version, synopsis, description, name_lower, synopsis_lower, description_lower});
    }
    std::sort(all_packages.begin(), all_packages.end(), [](const Package &a, const Package &b)
              {
                if (a.name == b.name){
                    return a.version > b.version;
                }
                return a.name < b.name; });
}

struct SearchResult
{
    const Package *package;
    int score;
};

int score_string(std::string_view pattern, std::string_view str)
{
    if (pattern.empty() || str.empty() || pattern.size() > str.size())
    {
        return 0;
    }

    int score = 0;
    size_t pos = 0;

    while ((pos = str.find(pattern, pos)) != std::string_view::npos)
    {
        score += (pos == 0 && pattern.size() == str.size()) ? 5 : 1;
        pos += pattern.size();
    }

    return score;
}

std::vector<SearchResult>
search(const std::vector<Package> &packages,
       const std::vector<std::string> &patterns)
{
    std::vector<SearchResult> results;
    results.reserve(packages.size());

    if (patterns.empty())
    {
        for (const Package &package : packages)
            results.push_back({&package, 0});

        return results;
    }

    std::vector<std::string> small_patterns = patterns;
    for (std::string &pattern : small_patterns)
    {
        for (char &c : pattern)
        {
            c = lower_char(c);
        }
    }

    for (const Package &package : packages)
    {
        int score = 0;

        for (const std::string &small_pattern : small_patterns)
        {
            int sub_score = 0;
            // Name: weight 4
            sub_score += 6 * score_string(small_pattern, package.name_lower);

            // Synopsis: weight 3
            sub_score += 3 * score_string(small_pattern, package.synopsis_lower);

            // Description: weight 2
            sub_score += 2 * score_string(small_pattern, package.description_lower);

            if (sub_score == 0) // all patterns should be found
            {
                score = 0;
                break;
            }

            score += sub_score;
        }

        if (score != 0)
        {
            results.push_back({&package, score});
        }
    }

    std::sort(
        results.begin(),
        results.end(),
        [](const SearchResult &a, const SearchResult &b)
        {
            return a.score > b.score;
        });

    return results;
}
