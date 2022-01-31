//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "RfPhysicalInterface.h"

#include <inet/common/InitStages.h>
#include <inet/common/ModuleAccess.h>
#include <inet/common/Ptr.h>
#include <inet/common/Units.h>
#include <inet/physicallayer/wireless/common/contract/packetlevel/SignalTag_m.h>
#include <MACMsg_m.h>
#include <omnetpp/ccomponent.h>
#include <omnetpp/cexception.h>
#include <omnetpp/clog.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/cmodule.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/cpacket.h>
#include <omnetpp/cpar.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/csimulation.h>
#include <omnetpp/cwatch.h>
#include <omnetpp/regmacros.h>
#include <omnetpp/simtime.h>

#include "RFMessage_m.h"
#include "RFMessageChunk_m.h"


Define_Module(RfPhysicalInterface);
Register_Class(RfPhysicalInterface);

RfPhysicalInterface::RfPhysicalInterface() {
    // TODO Auto-generated constructor stub

}

RfPhysicalInterface::~RfPhysicalInterface() {
    // TODO Auto-generated destructor stub
}


void RfPhysicalInterface::initialize(int stage){



        if (stage == INITSTAGE_LOCAL) {

    EV << "I am initialized\n";


    // Load BER Curve Data
    loadBERCurveFile();

    // Statistics collection
    numReceivedMessages = 0;
    numErroneousMessages = 0;

    WATCH(numReceivedMessages);
    WATCH(numErroneousMessages);


    if( par("sendMsgOnInit").boolValue() == true) {

        // We schedule an initial auto-message. When it arrives, we will send the first network message
        EV << "Creating INITSELFMSG\n";
        cMessage * msg = new cMessage("InitialSelfMessage", INITSELFMSG);
        scheduleAt(simTime()+ 1.0,msg );

    }


    //cModule *radioReceiver = getParentModule() -> getSubmodule("wlan") -> getSubmodule("radio") -> getSubmodule("receiver");
    //std::string radioName = radioReceiver -> getName();
    ////double radioVar= radioReceiver -> par("power");
    //EV << "This is my receiver: " << parentName << "\n";
    //EV << "This is his variable: " << radioVar << "\n";

    radio = getModuleFromPar<inet::physicallayer::IRadio>(par("radioModule"), this);
        }
        else if (stage == INITSTAGE_LINK_LAYER) {

    radio->setRadioMode(inet::physicallayer::IRadio::RADIO_MODE_TRANSCEIVER);
        }

}


void RfPhysicalInterface::handleMessage(cMessage *msg){
    EV << "Some message was received\n";

    if (msg->isSelfMessage()){
        handleSelfMessage(msg);
    }
    else{

        // We add to the counter of received messages
        numReceivedMessages++;


        checkArrivalGate(msg); // To determine if the message is from upper layers or radio
        //forwardMessage(msg);
    }

}

/**
            Describes the treatment of all self messages
*/
void RfPhysicalInterface::handleSelfMessage(cMessage *msg){

    switch (msg->getKind()){
        case INITSELFMSG:

            // This is our cue to send the first real message to the network
            EV << "Received the initialization self message\n";
            cancelAndDelete(msg);


            // Calling the following function to start with the network messages
            sendInitialNetworkMessage();

            break;

        default:
            throw cRuntimeError("Unknown self message: kind=%d", msg->getKind());
    }

}

/**
    When a node receives the initialization self message (INITSELFMSG) it creates and sends an initial network message.
*/

void RfPhysicalInterface::sendInitialNetworkMessage(){


    if( par("standaloneRFNetwork").boolValue() == true) {
        // Utilizado cuando probamos RfCommuncation de manera individual
        EV << "We are now about to create the initial network message";

        //auto rfMsg= makeShared<RfMsg>();                  //I will be able to create by own messages at some point
        long packetByteLength = long(par("packetByteLength"));
        auto data =  makeShared<ByteCountChunk>(B(packetByteLength));

        Packet *packet = new Packet("RFPHYPacket", data);   // I create a packet with the "data" defined above
        packet->addTagIfAbsent<MacAddressReq>()->setDestAddress(MacAddress::BROADCAST_ADDRESS);
        packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::ieee8021ae);

        EV << "Creating packet with a length of " << packetByteLength << " bytes\n";

        send(packet, "rfgateout");
    }

}


/**
    For not self messages, we check if they are from upper layers or network
*/
void RfPhysicalInterface::checkArrivalGate(cMessage *msg){

    if (msg->getArrivalGate() == gate("upperLayerIn"))
        processMsgFromUpperLayer(msg);
    else // Viene de Ieee802154NarrowbandInterface
        processMsgFromNetwork(msg);
}


void RfPhysicalInterface::processMsgFromUpperLayer(cMessage *msg){

    EV << "Message received from upper layer at RfPhysicalInterface\n";


    cPacket *omnetPacket = (cPacket * )msg;
    MACMsg *macFrame= (MACMsg *) omnetPacket;

    //Packet *packet = dynamic_cast<Packet*>(msg);

    switch( macFrame -> getHDR_HT() ){

        case 0: // This message has a specific MAC address
            EV << "This message has a specific MAC address";
          break;
        default:
            EV << "This message has generic MAC address\n";


            break;
    }

    // We will send this newly created INET packet
    Packet *packet = new Packet("RFPHYPacket");


    // How to add a tag to a packet
    //auto RFMacMsgContent_object = packet->addTag<RFMacMsgContent>(); // add tag for dispatch
    //RFMacMsgContent_object->setMensajeMac(macFrame); // set designated interface


    // We add the macFrame that comes from the upper layer
    packet->addTag<RFMacMsgContent>()->setMensajeMac(macFrame);


    // Because it is a generic MAC Address
    packet->addTagIfAbsent<MacAddressReq>()->setDestAddress(MacAddress::BROADCAST_ADDRESS);
    packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::ieee8021ae); // Es posible que haya que quitar el "If absent" y editar un tag ya creado

    //auto macFrameRecv = packet-> getTag<RFMacMsgContent>()->getMensajeMac();
    //auto macFrame_aux = const_cast<cPacket*>(packet-> getTag<RFMacMsgContent>()->getMensajeMac());

    // Test1 del macFrame
    //EV << "Sending macFrame as self message\n";
    //scheduleAt(simTime()+ 1.0,macFrame_aux );
    // Esto lo coge bien!!
    // Pero he leído que los tags son para comunicarse entre capas de un mismo host. No se envían por la red

    // Test2 del macFrame
    const auto& payload = makeShared<RFMessageChunk>();
    //payload->setChunkLength(B(par("messageLength")));
    payload->setChunkLength(B(5));
    payload->setMensajeMac(macFrame);
    packet->insertAtBack(payload);

    EV << "Sending new message\n";
    send(packet, "rfgateout");

}

void RfPhysicalInterface::processMsgFromNetwork(cMessage *msg){

    EV << "Message received from lower layer at RfPhysicalInterface\n";
    Packet *packet = dynamic_cast<Packet*>(msg);

    EV << "Simulating error for received message\n";
    bool hasBitError = simulateError(packet);


    if (hasBitError){
        // We add to the counter of erroneous messages
        numErroneousMessages++;
        packet->setBitError(true);
    }


    if( par("standaloneRFNetwork").boolValue() == true) {
        // Utilizado cuando probamos RfCommuncation de manera individual
        forwardMessage(packet);

    }else{

        // Utilizado para la integración hybrid

        // Desencapsulamos test 1
        //auto RFMacMsgContent_object = packet-> getTag<RFMacMsgContent>();
        //MACMsg *macFrame = (MACMsg*) RFMacMsgContent_object->getMensajeMac();


        // Desencapsulamos test 2
        //auto macFrame = const_cast<cPacket*>(packet-> getTag<RFMacMsgContent>()->getMensajeMac());


        // Desencapsulamos test 3
        const auto& payload = packet->peekData<RFMessageChunk>();
        MACMsg *macFrameRecibida= (MACMsg *) payload->getMensajeMac();
        EV_INFO << "packet content1: " << macFrameRecibida->getHDR_HT() << endl;

        EV << "Desencapsulación hecha\n";


        // Aquí falta poner el error a la macFrameRecibida
        // Así está en MAC.cpp
        //if (dblrand() < 1.0 - pow(1.0 - BER, (double) frame->getBitLength()))
        //    frame->setBitError(true);
        if(hasBitError){
            macFrameRecibida -> setBitError(true);
        }



        send(macFrameRecibida->dup(), "upperLayerOut");
        // Lo enviamos hacia arriba YA HABIENDO CALCULADO EL ERROR
    }
}

// Esto ahora mismo solo está en uso para cuando corremos RfCommunication de manera aislada
void RfPhysicalInterface::forwardMessage(Packet *packet)
{


    //Packet *packet = dynamic_cast<Packet*>(msg);
    bool hasBitError = packet->hasBitError();

    if (hasBitError){


        EV << "Packet has bit error. Deleting received message and creating a new one\n";
        delete packet;

        long packetByteLength = long(par("packetByteLength"));
        auto data =  makeShared<ByteCountChunk>(B(packetByteLength));

        Packet *packet = new Packet("RFPHYPacket_alt", data);   // I create a packet with the "data" defined above
        packet->addTagIfAbsent<MacAddressReq>()->setDestAddress(MacAddress::STP_MULTICAST_ADDRESS);
        packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::ieee8021ae);


        EV << "Sending new message\n";
        send(packet, "rfgateout");
    }else{

        EV << "Packet does not have bit error. Returning same message\n";

        //Packet *packet = dynamic_cast<Packet*>(msg);
        packet->addTagIfAbsent<MacAddressReq>()->setDestAddress(MacAddress::STP_MULTICAST_ADDRESS);
        packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::ieee8021ae);

        EV << "Sending same message again\n";
        send(packet, "rfgateout");
    }

}

/**
 * Simulates whether message has error or not by looking at
 * the corresponding BER curve. Returns true if there is an error.
 *
 * @param msg the message for which the error is simulated
 */
bool RfPhysicalInterface::simulateError(Packet *packet){

    bool errorInMsg;

    if (uniform(0,1) < 0.5){
        EV << "Error in message\n";
        errorInMsg = true;

    }else{
        EV << "No error in message\n";
        errorInMsg = false;
    }

    // Esto por ahora es de prueba para ver que accedemos al valor correcto
    float SNR = computeSNR(packet);
    EV<< "SNR calculado: " << SNR << endl;

    float BERleida = obtainBERforSNR(SNR);
    EV << "Ber leida: " << BERleida << endl;


    //Considerar esto:
    //auto length = packet->getTotalLength();
    //auto hasErrors = hasProbabilisticError(length, ber); // decide randomly
    // https://inet.omnetpp.org/docs/developers-guide/ch-packets.html#:~:text=raw%20chunks%20(accurate)-,The,-first%20example%20shows

    return errorInMsg;
}

/**
 * Calculates the SNR of the received message by looking at
 * Px and noise power.
 *
 * @param msg the message for which SNR is calculated
 */
float RfPhysicalInterface::computeSNR(Packet *packet)
{


    // Intento 1
    auto signalPowerInd = packet-> getTag<SignalPowerInd>();
    auto rxPower = signalPowerInd->getPower().get();

    // Intento 2 - No funciona
    //Packet *packet = dynamic_cast<Packet*>(msg);
    //auto rxPower_aux = packet -> par("reception");


    // Intento 3 - No funciona
    //cModule *receptor = this -> getSubmodule("RFPHYPacket") -> getSubmodule("reception");
    //std::string receptorName = receptor -> getName();

    // Intento 4 - No funciona
    //cObject *obj  = msg -> getObject("reception");
    //EV << "Obtenido" << endl;
    //std::string receptorName = obj -> getName();

    // Intento 5 - No funciona
    //cModule *modulo = currentSimulation -> getModule("RFPHYPacket");
    //Packet *packeteo = dynamic_cast<Packet*>(modulo);

    // Intento 6 - Funciona pero no conssigo llegar más allá de la simulación
    // cSimulation *currentSimulation = getSimulation();
    // int  dummyDeleteMe = currentSimulation ->  getContextType();
    //cModule *modulo = currentSimulation -> findModuleByPath("RPHYPacket"); // Esto ya no funciona
    //EV<< "Simulation= " << dummyDeleteMe << endl;

    // Intento 7 - No funciona
    //cModule *modulo = findModuleByPath("simulation.scheduled-events.RFPHYPacket"); // obtenido de la simulación directamente
    //int indice = modulo -> getIndex();
    //EV<< "Indice= " << indice << endl;

    // Mirar línea 445 de Radio.cc  -> Hay que buscar en la clase Reception...

    // We compute received power
    //float rxPower_aux = 1.15; // Esto falta conseguirlo - Only for testing purposes

    EV<< "RX power= " << rxPower << "W" << endl;

    //float rxPowerdB = 10 * log10(rxPower* pow(10, -12)); // Esto sería con el auxiliar y convirtiendo a mW
    float rxPowerdB = 10 * log10(rxPower);
    EV<< "RX power= " << rxPowerdB << "dBW" << endl;


    // We compute background noise level

    // The background noise level in dB is obtained from wireless model
    cModule *ruidoFondo;
    if (par("standaloneRFNetwork")){
        // En caso de ser standalone no hay que mirar tan arriba para encontrar el radioMedium
        ruidoFondo = getParentModule() -> getParentModule() -> getSubmodule("radioMedium") -> getSubmodule("backgroundNoise");
    }else{
        // En caso de NO ser standalone
        ruidoFondo = getParentModule() -> getParentModule() -> getParentModule() -> getSubmodule("radioMedium") -> getSubmodule("backgroundNoise");
    }

    // std::string backgroundNoisePower = ruidoFondo -> getName(); // Para comprobar que estoy cogiendo el módulo correcto

    double backgroundNoisePowerdBm = ruidoFondo -> par("power");

    double backgroundNoisePowerdB = backgroundNoisePowerdBm - 30; // We transform to dB
    EV<< "Noise power= " << backgroundNoisePowerdB << "dBW" << endl;


    float SNR = rxPowerdB - backgroundNoisePowerdB;

    return SNR;
}



/**
 * Returns a BER value for a specific SNR
 * The BER curve is specified in the .ini file
 * If the SNR value is not a point in the SNR/BER curve,
 * the corresponding BER is obteined by interpolation
 *
 * @param msg the message for which the BER is obtained
 */
float RfPhysicalInterface::obtainBERforSNR(float SNR_mensaje)
{
    // Esto estaría bien crearlo en la inicialización y dejar el mapa creado como variable global

    // We will now interpolate the message's SNR between to valid points in the curve. The points in the curve are separated .25


    float SNR_limite_superior = obtenerSNRValidaSuperior(SNR_mensaje);


    float SNR_limite_inferior = SNR_limite_superior - 0.25;

    float BER_leida_superior = leerBERforSNRfromFile(SNR_limite_superior);
    float BER_leida_inferior = leerBERforSNRfromFile(SNR_limite_inferior);

    float BER_interpolada = BER_leida_inferior + (SNR_mensaje - SNR_limite_inferior) * (BER_leida_superior - BER_leida_inferior) / (SNR_limite_superior - SNR_limite_inferior);

    return BER_interpolada;
}

/**
 * Returns an SNR value that is defined in the SNR/BER curve.
 * This value is the closest higher point to the introduced SNR.
 *
 * @param SNR_a_redondear SNR to round up to the nearest valid point
 */
float RfPhysicalInterface::obtenerSNRValidaSuperior(float SNR_a_redondear)
{
    // We round upwards to required decimal precision (two positions) and multiply by 100 to get integers - necessary for roundUp()
    int SNR_int = (int)((SNR_a_redondear + 0.005) * 100);

    // Now we round to the closest "25". This is because SNR/BER pairs have been obtained to a precision of .25
    int SNR_redondeada_int = roundUp(SNR_int, 25);

    // Finally, we divide by 100 to get back to the true SNR value
    float SNR_redondeada_float = (float)SNR_redondeada_int / 100;


    // The returned value is a valid value of SNR (in intervals of .25) rounded upwards from the read value
    return SNR_redondeada_float;
}


/**
 * Simple rounding function that rounds a given integer to the
 * closest specified multiple.
 *
 * @param numToRound number to round
 * @param multiple the rounding will be done to a multiple of this parameter
 */
int RfPhysicalInterface::roundUp(int numToRound, int multiple)
{
    if (multiple == 0)

        return numToRound;

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}


/**
 * It loads the data from the SNR BER curve into a global
 * variable for later lookup
 *
 */
void RfPhysicalInterface::loadBERCurveFile(){
    //std::string filename = "../BERDATA/RSC_M_2.txt";
    std::string filename  = par("BERFilename");

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
        EV << "Leo Communication Mode: " << auxComMode << endl;

        getline(file, auxFEC);
        //std::cout << "Leo FEC: " << auxFEC << std::endl;
        EV << "Leo FEC: " << auxFEC << endl;

        getline(file, auxModulation);
        //std::cout << "Leo Modulation: " << auxModulation << std::endl;
        EV << "Leo Modulation: " << auxModulation << endl;

        while (std::getline(file, BER, '|'))
        {
            getline(file, SNR);
            //std::cout << "Leo BER: " << BER << " Leo SNR: " << SNR << std::endl;
            //EV  << "Leo BER: " << BER << " Leo SNR: " << SNR << endl;finish

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
        //std::cout << "Error al abrir el archivo de datos de la curva BER" << std::endl;
        EV << "Error al abrir el archivo de datos de la curva BER\n"  << endl;
    }
    file.close();

}


/**
 * Looks up BER for given SNR value within the BER
 * curve specified in the .ini file
 *
 * @param SNR_a_leer SNR value to look ip in the BER curve values
 */
float RfPhysicalInterface::leerBERforSNRfromFile(float SNR_a_leer){

    float BER_leida = snrBerMap[SNR_a_leer];

    return BER_leida;
}

void RfPhysicalInterface::finish(){

    recordScalar("#ReceivedMessages", numReceivedMessages);
    recordScalar("#ErroneousMessages", numErroneousMessages);

}
