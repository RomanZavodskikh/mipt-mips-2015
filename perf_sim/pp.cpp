#include <iostream>
#include "ports.h"

int main()
{
    WritePort<bool> one_wport("one", 32, 1);
    ReadPort<bool> one_rport("one", 5);
    one_wport.init();
    one_rport.init();
    for (int i = 0; i < 19; ++i)
    {
        std::cout << "Cycle " << i << std::endl;
        if (i == 10)
        {
            bool pi = true;
            one_wport.write( pi, i);
            std::cout << pi << " sended." << std::endl;
        }
        bool num;
        int rtr_val = one_rport.read( &num, i);
        std::cout << num << " received. " << rtr_val << std::endl;
    }
    return 0;
}
