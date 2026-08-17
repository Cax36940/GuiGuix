#include <cstring>
#include <string>
#include <vector>

struct Package
{
    const char *name;
    const char *version;
    const char *synopsis;
    const char *description;
};

std::vector<Package> all_packages;
std::string all_packages_str;

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

    while (*ptr)
    {
        const char *name = next_string(ptr);
        const char *version = next_string(ptr);
        const char *synopsis = next_string(ptr);
        const char *description = next_string(ptr);

        all_packages.push_back({name, version, synopsis, description});
    }
    std::sort(all_packages.begin(), all_packages.end(), [](const Package &a, const Package &b)
              {
                const int name_cmp = strcmp(a.name, b.name);
                if (name_cmp != 0){
                    return name_cmp < 0;
                }
                return strcmp(a.version,b.version) > 0; });
}

struct SearchResult
{
    const Package *package;
    int score;
};

static inline char lower_char(char c)
{
    return static_cast<char>(
        std::tolower(static_cast<unsigned char>(c)));
}

int score_string(const std::string &pattern, const char *str)
{
    if (pattern.empty() || !str[0])
        return 0;

    const size_t pattern_len = pattern.size();
    const size_t str_len = std::strlen(str);

    if (pattern_len > str_len)
    {
        return 0;
    }

    int score = 0;

    for (size_t pos = 0; pos <= str_len - pattern_len;)
    {
        bool match = true;

        for (size_t i = 0; i < pattern_len; ++i)
        {
            if (lower_char(str[pos + i]) != lower_char(pattern[i]))
            {
                match = false;
                break;
            }
        }

        if (match)
        {
            // Exact match of the entire field.
            if (pos == 0 && pattern_len == str_len)
                score += 5;
            else
                score += 1;

            // Don't count overlapping matches.
            pos += pattern_len;
        }
        else
        {
            ++pos;
        }
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

    for (const Package &package : packages)
    {
        int score = 0;

        for (const std::string &pattern : patterns)
        {
            int sub_score = 0;
            // Name: weight 4
            sub_score += 6 * score_string(pattern, package.name);

            // Synopsis: weight 3
            sub_score += 3 * score_string(pattern, package.synopsis);

            // Description: weight 2
            sub_score += 2 * score_string(pattern, package.description);

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
