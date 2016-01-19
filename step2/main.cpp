#include <iostream>

#include <alps/params.hpp>

int main(int argc, char** argv)
{
    alps::params p(argc, (const char**)argv);
    p
        .description("Demo ALPSCore program")
        .define<std::string>("name","A name to greet")
        .define<int>("count", 1, "How many times to greet")
        .define("loud", "Should we be \"loud\"?");

    if (p.help_requested(std::cerr))
        return 1;

    std::string ending=p["loud"]?"!!!":".";
    for (int i=0; i<p["count"]; ++i) {
        std::cout << "Hello, " << p["name"].as<std::string>() << ending << "\n";
    }
    return 0;
}
