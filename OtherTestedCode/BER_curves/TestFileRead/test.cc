#include <iostream>
#include <fstream>
#include <string>
#include <map>

float leerBERSNR(float SNR_a_leer);

int main()
{
    float SNR_a_leer = 5.50;
    float BER_leida = leerBERSNR(SNR_a_leer);
    std::cout << "BER leida " << BER_leida << std::endl;
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
        std::cout << "Leo Communication Mode: " << auxComMode << std::endl;
        //EV << "Leo Communication Mode: " << auxComMode << endl;

        getline(file, auxFEC);
        std::cout << "Leo FEC: " << auxFEC << std::endl;
        //EV << "Leo FEC: " << auxComMode << endl;

        getline(file, auxModulation);
        std::cout << "Leo Modulation: " << auxModulation << std::endl;
        //EV << "Leo Modulation: " << auxComMode << endl;

        while (std::getline(file, BER, '|'))
        {
            getline(file, SNR);
            std::cout << "Leo BER: " << BER << " Leo SNR: " << SNR << std::endl;
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