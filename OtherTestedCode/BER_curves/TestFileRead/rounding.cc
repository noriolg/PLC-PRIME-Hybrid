#include <iostream>
#include <fstream>
#include <string>
#include <map>

int roundUp(int numToRound, int multiple);

int main()
{
    float SNR_a_leer = 5.551;

    int SNR_int = (int)((SNR_a_leer + 0.005) * 100);

    int SNR_redondeada_int = roundUp(SNR_int, 25);

    float SNR_redondeada_float = (float)SNR_redondeada_int / 100;

    std::cout << "BER leida " << SNR_redondeada_float << std::endl;
}

int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)

        return numToRound;

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}
