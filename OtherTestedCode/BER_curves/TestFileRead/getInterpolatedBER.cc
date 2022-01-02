#include <iostream>
#include <fstream>
#include <string>
#include <map>

float obtenerSNRValidaSuperior(float SNR_a_leer);
float leerBERSNR(float SNR_a_leer);
int roundUp(int numToRound, int multiple);

int main()
{
    float SNR_a_leer = 5.70;

    float SNR_limite_superior = obtenerSNRValidaSuperior(SNR_a_leer);
    float SNR_limite_inferior = SNR_limite_superior - 0.25;

    float BER_leida_superior = leerBERSNR(SNR_limite_superior);
    float BER_leida_inferior = leerBERSNR(SNR_limite_inferior);

    float BER_interpolada = BER_leida_inferior + (SNR_a_leer - SNR_limite_inferior) * (BER_leida_superior - BER_leida_inferior) / (SNR_limite_superior - SNR_limite_inferior);

    std::cout << "BER leida " << BER_interpolada << std::endl;
}

float obtenerSNRValidaSuperior(float SNR_a_leer)
{

    int SNR_int = (int)((SNR_a_leer + 0.005) * 100);
    int SNR_redondeada_int = roundUp(SNR_int, 25);
    float SNR_redondeada_float = (float)SNR_redondeada_int / 100;

    return SNR_redondeada_float;
}

float leerBERSNR(float SNR_a_leer)
{
    std::string filename = "NRNSC_M_2.txt";
    std::map<float, float> snrBerMap;

    std::string auxComMode;
    std::string auxFEC;
    std::string auxModulation;

    std::string BER;
    std::string SNR;
    float fBER;
    float fSNR;

    std::ifstream file(filename);
    if (file.is_open())
    {

        getline(file, auxComMode);
        //std::cout << "Leo Communication Mode: " << auxComMode << std::endl;
        //EV << "Leo Communication Mode: " << auxComMode << endl;

        getline(file, auxFEC);
        //std::cout << "Leo FEC: " << auxFEC << std::endl;
        //EV << "Leo FEC: " << auxComMode << endl;

        getline(file, auxModulation);
        //std::cout << "Leo Modulation: " << auxModulation << std::endl;
        //EV << "Leo Modulation: " << auxComMode << endl;

        while (std::getline(file, BER, '|'))
        {
            getline(file, SNR);
            //std::cout << "Leo BER: " << BER << " Leo SNR: " << SNR << std::endl;
            //EV  << "Leo BER: " << BER << " Leo SNR: " << SNR << endl;

            fSNR = std::stof(SNR);
            fBER = std::stof(BER);

            // Anadimos los datos a un map que relaciona SNR con BER
            snrBerMap[fSNR] = fBER;
        }

        //std::cout << "snrBerMap[5.50] is " << snrBerMap[5.50] << '\n' << std:endl;
        //EV << "snrBerMap[5.50] is " << snrBerMap[5.50] << '\n'<< endl;
    }
    else
    {
        std::cout << "Error al abrir el archivo de datos de la curva BER" << std::endl;
        //EV << "Error al abrir el archivo de datos de la curva BER\n"  << endl;
    }
    file.close();

    return snrBerMap[SNR_a_leer];
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