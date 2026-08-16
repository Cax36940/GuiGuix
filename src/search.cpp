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
              { return strcmp(a.name, b.name) < 0; });
}