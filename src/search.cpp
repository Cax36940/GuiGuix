#include <string>
#include <vector>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

struct Package
{
    std::string name;
    std::string version;
    std::string synopsis;
    std::string description;
};

std::vector<Package> all_packages;
/*
    {
        {.name = "hello",
         .version = "2.12.3",
         .synopsis = "Example GNU package",
         .description = "GNU Hello prints the message \"Hello, world!\" and then exits."
                        "It serves as an example of standard GNU coding practices."
                        "As such, it supports command-line arguments, multiple languages, and so on."},

        {.name = "grep",
         .version = "3.11",
         .synopsis = "Print lines matching a pattern",
         .description = "grep is a tool for finding text inside files."
                        "Text is found by matching a pattern provided by the user in one or many files."
                        "The pattern may be provided as a basic or extended regular expression, or as fixed strings."
                        "By default, the matching text is simply printed to the screen, however the output can be greatly customized to include, for example, line numbers."
                        "GNU grep offers many extensions over the standard utility, including, for example, recursive directory searching."},

        {.name = "sed",
         .version = "4.9",
         .synopsis = "Stream editor",
         .description = "Sed is a non-interactive, text stream editor."
                        "It receives a text input from a file or from standard input and it then applies a series of text editing commands to the stream and prints its output to standard output."
                        "It is often used for substituting text patterns in a stream."
                        "The GNU implementation offers several extensions over the standard utility."},
};
*/

struct PackageLocation
{
    std::string file;
    size_t line;
};

std::vector<PackageLocation> compute_package_locations()
{
    const std::string list_available_str = runCommand("guix package --list-available");
    const char *list_available_raw = list_available_str.data();
    size_t cursor = 0;

    std::vector<PackageLocation> locations;
    while (list_available_raw[cursor])
    {
        size_t line_begin = cursor;
        while (list_available_raw[cursor] != '\n' && list_available_raw[cursor] != '\0')
        {
            ++cursor;
        }

        std::string line = list_available_str.substr(line_begin, cursor - line_begin);
        ++cursor; // Go to after '/n' to avoid being stuck on the next loop

        // Package line format from available list command:
        // name  vestion   out(,doc)   file_path:line:column

        const size_t colon1 = line.find(':');
        const size_t colon2 = line.find(':', colon1 + 1);

        size_t begin_file = colon1;
        while (line[--begin_file] != '\t')
        {
        }
        ++begin_file;

        PackageLocation package;
        package.file = line.substr(begin_file, colon1 - begin_file);
        package.line = std::stoul(line.substr(colon1 + 1, colon2 - colon1 - 1));
        locations.push_back(package);
    }

    return locations;
}

std::string read_file(const std::string &filepath)
{
    std::ifstream file("/gnu/store/q7sp4v4w7sddfkgb35qnjlv6sdhs0vyc-guix-module-union/share/guile/site/3.0/" + filepath);
    if (!file)
    {
        printf("Could not open file : %s\n", filepath.c_str());
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void init_packages()
{
    std::vector<PackageLocation> locations = compute_package_locations();
    printf("Total number of packages : %lu\n", locations.size());
    all_packages.reserve(locations.size());

    for (const PackageLocation &location : locations)
    {
        // Get file content
        const std::string file_str = read_file(location.file);
        printf("%s:%lu\n", location.file.c_str(), location.line);
        if (file_str.empty())
        {
            continue;
        }
        const char *file_raw = file_str.data();

        // Go to the package definition line
        size_t pos = 0;
        size_t count_line = 1;

        while (count_line < location.line)
        {
            if (file_raw[pos] == '\n')
            {
                ++count_line;
            }
            ++pos;
        }

        size_t current_line = location.line + 1;

        // Go until the first (
        while (file_raw[pos] != '(')
        {
            ++pos;
        }
        ++pos; // <- we're approximately on the p of "package"

        // Go until next (
        while (file_raw[pos] != '(')
        {
            ++pos;
        }
        ++pos; // <- we're approximately on the first char of the first package field

        Package package;
        int filled_field = 0;
        while (filled_field < 0b1111 && pos != std::string::npos)
        {
            size_t end_field_name = pos;
            while (!std::isspace(file_raw[end_field_name]))
            {
                if (file_raw[end_field_name] == '\n')
                {
                    ++current_line;
                }
                ++end_field_name;
            }
            std::string field_name(&file_raw[pos], end_field_name - pos);

            size_t debug_package_index = 7893;
            if (all_packages.size() == debug_package_index)
            {
                printf("Field name : %s\n", field_name.c_str());
            }

            size_t start_field_content = end_field_name;
            while (std::isspace(file_raw[start_field_content]))
            {
                if (file_raw[start_field_content] == '\n')
                {
                    ++current_line;
                }
                ++start_field_content;
            }

            size_t end_field_content = start_field_content;
            int depth = 0;
            bool is_in_string = false;
            bool escaped = false;
            bool is_in_comment = false;

            size_t debug_count = 0;

            while (true)
            {
                const char c = file_raw[end_field_content];

                if (c == '\n')
                {
                    ++current_line;
                }

                if (is_in_comment)
                {
                    if (c == '\n')
                        is_in_comment = false;
                }
                else if (is_in_string)
                {
                    if (escaped)
                        escaped = false;
                    else if (c == '\\')
                        escaped = true;
                    else if (c == '"')
                        is_in_string = false;
                }
                else
                {
                    if (c == ';' && file_raw[end_field_content - 1] != '#')
                    {
                        is_in_comment = true;
                    }
                    else if (c == '"')
                    {
                        is_in_string = true;
                    }
                    else if (c == '(')
                    {
                        ++depth;
                        /*
                        if (all_packages.size() == debug_package_index && debug_count < 500)
                        {
                            printf("depth = %u : %lu\n", depth, current_line);
                            ++debug_count;
                        }
                        */
                    }
                    else if (c == ')')
                    {
                        if (depth == 0)
                            break;

                        --depth;
                        /*
                        if (all_packages.size() == debug_package_index && debug_count < 500)
                        {
                            printf("depth = %u : %lu\n", depth, current_line);
                            ++debug_count;
                        }
                        */
                    }
                }

                ++end_field_content;
            }

            std::string field_content(&file_raw[start_field_content], end_field_content - start_field_content); // <- only right for the right field

            if (all_packages.size() == debug_package_index)
            {
                printf("Content and line : %lu\n", current_line);
                printf("Content : %s ... %s\n", field_content.substr(0, 10).c_str(), field_content.substr(field_content.size() > 10 ? field_content.size() - 10 : 0, 10).c_str());
            }

            // Compare text with field
            if (!(filled_field & 0b1) && (field_name == "name"))
            {
                package.name = field_content;
                filled_field |= 0b1;
            }
            else if (!(filled_field & 0b10) && (field_name == "version"))
            {
                package.version = field_content;
                filled_field |= 0b10;
            }
            else if (!(filled_field & 0b100) && (field_name == "synopsis"))
            {
                package.synopsis = field_content;
                filled_field |= 0b100;
            }
            else if (!(filled_field & 0b1000) && (field_name == "description"))
            {
                package.description = field_content;
                filled_field |= 0b1000;
            }

            // Go to next field
            pos = end_field_content + 1;
            bool is_end_package_def = false;

            is_in_comment = false;
            while (true)
            {
                if (file_raw[pos] == '\n')
                {
                    is_in_comment = false;
                    ++current_line;
                }

                else if (file_raw[pos] == ';' && file_raw[pos - 1] != '#')
                {
                    is_in_comment = true;
                }

                if (!is_in_comment)
                {

                    if (file_raw[pos] == ')')
                    {
                        is_end_package_def = true;
                        break;
                    }
                    else if (file_raw[pos] == '(')
                    {
                        break;
                    }
                }
                ++pos;
            }
            if (is_end_package_def)
            {
                break;
            }

            ++pos;
        }

        all_packages.push_back(package);

        printf("%lu/%lu\n", all_packages.size(), all_packages.capacity());
    }
}
