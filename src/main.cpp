#include "Model.h"

int main ()
{
    std::list<std::pair<unsigned, unsigned>> l = {std::pair<unsigned, unsigned>(1, 2), std::pair<unsigned, unsigned>(1, 3)};
    Graph g(10, l);
    std::cout << g;
}