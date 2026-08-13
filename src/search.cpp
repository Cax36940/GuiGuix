#include <string>
#include <vector>

struct Package
{
    std::string name;
    std::string version;
    std::string synopsis;
    std::string description;
};

std::vector<Package> all_packages =
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
