#include <iostream>
#include <algorithm>


int main(int argc, char **argv)
{
    if (argc <= 3)
        return 0;
    std::string cmd = argv[1];
    std::string str = argv[2];
    for (int i = 3; i < argc; i++)
    {
        str.append(" ");
        str.append(argv[i]);
    }
    if (cmd == "down")
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    if (cmd == "up")
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    std::cout << str << std::endl;
    return (0);
}