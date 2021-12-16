using namespace std;

#include <iostream>
#include <fstream>
#include <string>

int main()
{

    std::ifstream file("NRNSC_M_2.txt");
    //std::ifstream inf(fileName.c_str());

    // Leo primera linea
    std::string auxComMode;
    getline(file, auxComMode);
    std::cout << auxComMode << std::endl;
    //EV << "Leo Communication Mode: " << auxComMode << endl;

    // Leo segunda linea
    std::string auxFEC;
    getline(file, auxFEC);
    std::cout << auxFEC << std::endl;
    //EV << "Leo FEC: " << auxComMode << endl;

    std::string linea;
    while (std::getline(file, linea))
    {
        // Process str
    }

    std::cout << "Hello World" << std::endl;
}

if (inf.is_open())
{

    std::string BER;
    std::string SNR;

    getline(inf, auxComMode);
    EV << "Leo Communication Mode: " << auxComMode << endl;

    getline(inf, auxFEC);
    EV << "Leo FEC: " << auxFEC << endl;
    if (auxFEC.compare("false") == 0)
    {
        FEC = false;
    }

    if (auxFEC.compare("true") == 0)
    {
        FEC = true;
    }

    getline(inf, auxModulation);
    EV << "Leo Modulation: " << auxModulation << endl;

    while (!std::getline(inf, BER, '|').eof())
    //while(!inf.eof())
    {
        getline(inf, SNR);

        EV << "Leo BER: " << BER << "Leo SNR: " << SNR << endl;

        from_string<float>(auxBER, BER, std::dec);
        from_string<float>(auxSNR, SNR, std::dec);
        BERvector.push_back(auxBER);
        SNRvector.push_back(auxSNR);
    }
    /*
		ev << "vector length: "<< BERvector.size()<< endl;

		int i = 0;
		while (i<BERvector.size()){
			ev << BERvector.at(i) << endl;
			i++;
		}
         */
    EV << "Inserting data in berData list" << endl;
    berData->insert(new BERData(auxComMode, FEC, auxModulation, SNRvector, BERvector));
}
else
{
    error("File not found");
}