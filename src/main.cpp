#include "Model.h"
#include "Contract.h"

int main ()
{
    std::list<std::pair<unsigned, unsigned>> l = {};
    Model m(10, l);
    m.sample();
	std::cout << m;
}