#include <iostream>
#include "ports.h"

int main()
{
    WritePort<double> one_wport("one", 32, 1);
    ReadPort<double> one_rport("one", 5);
    one_wport.init();
    one_rport.init();
    for (int i = 0; i < 9; ++i)
    {
        std::cout << "Cycle " << i << std::endl;
        if (i == 0)
        {
            double pi = 3.1415;
            one_wport.write( pi, i);
            std::cout << pi << " sended." << std::endl;
        }
        double num;
        one_rport.read( &num, i);
        std::cout << num << " received." << std::endl;
    }
    return 0;
}
