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
#include <inet/common/packet/chunk/FieldsChunk.h>
#include <inet/common/Ptr.h>
#include <inet/common/Units.h>
#include <inet/physicallayer/wireless/common/contract/packetlevel/SignalTag_m.h>
#include <MACMsg_m.h>
#include <omnetpp/ccomponent.h>
#include <omnetpp/cdisplaystring.h>
#include <omnetpp/cexception.h>
#include <omnetpp/clog.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/cmodule.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/cownedobject.h>
#include <omnetpp/cpacket.h>
#include <omnetpp/cpar.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/csimulation.h>
#include <omnetpp/cwatch.h>
#include <omnetpp/regmacros.h>
#include <omnetpp/simtime.h>
#include <PLCMAC.h>
#include <cmath>

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
        radio = getModuleFromPar<inet::physicallayer::IRadio>(par("radioModule"), this);
    }
    else if (stage == INITSTAGE_PHYSICAL_ENVIRONMENT) {

        inicializarPosicionRadio();
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

    // Vemos si podemos obtener el Byte len. del macFrame
    auto tamanoBytes = macFrame -> getByteLength();
    EV << "Tamaño de paquete de MacFrame: "<<  tamanoBytes  <<" B\n";


    payload->setChunkLength(B(tamanoBytes));
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
        //MACMsg *macFrame = (MACMsg*) RFMacMsgContent_object->getMensajeMac();v


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
            //macFrameRecibida -> setBitError(true);
        }

        delete packet;

        send(macFrameRecibida->dup(), "upperLayerOut");

        //cModule *owner = macFrameRecibida -> getOwner();
        //owner -> delete macFrameRecibida;



        //delete macFrameRecibida;

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


    // Esto por ahora es de prueba para ver que accedemos al valor correcto
    float SNR = computeSNR(packet);
    EV<< "SNR calculado: " << SNR << endl;

    float BERleida = obtainBERforSNR(SNR);
    EV << "Ber leida: " << BERleida << endl;


    // Esto es temporal. Debe cambiarse por lo inferior para simulaciones correctas
    if (uniform(0,1) < 0.5){
        EV << "Error in message\n";
        errorInMsg = true;

    }else{
        EV << "No error in message\n";
        errorInMsg = false;
    }

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



void RfPhysicalInterface::inicializarPosicionRadio(){

    //EV << "Mi parent display string es: "<< parentDispStr <<"\n";
    //  Ejemplo de display string de mi grandparent p=1033,290;i=misc/house;q=txQueue;i2=status/red

    // Tengo que encontrar la verdadera posición en la que debería de estar este nodo.
    // Para eso voy a utilizar su MacAddress y los siguientes parámetros
    int nBranchesInt =  (cSimulation::getActiveSimulation())->getSystemModule()->par("nBranches"); // Para decidir si es par o impar lo necesitamos como int
    float nBranches = static_cast< float >(nBranchesInt); // Lo cogemos como float para que miRama se calcule correctamente
    int nhostsPerBranch = (cSimulation::getActiveSimulation())->getSystemModule()->par("nHostsPerBranch");
    int busLengthInMetersInt = (cSimulation::getActiveSimulation())->getSystemModule() ->par("busLengthForRadios");
    float busLengthInMeters = static_cast< float >(busLengthInMetersInt);
    //float total_number_nodes = nBranches * nhostsPerBranch;

    // Comencemos...

    // Cogemos la dirección MAC de la capa mac (que ya ha sido inicialiada)
    // primer padre: WirelessNodeBasic     segundo padre: PLC_Hybrid_SN  que ya tiene un submódulo llamado mac
    cModule *submoduloMAC = this ->getParentModule() ->  getParentModule() ->  getSubmodule("mac");
    PLCMAC *claseMAC = (PLCMAC*) submoduloMAC;
    int miMAC = (int) claseMAC -> getMACAddress();
    EV << "Mi MAC Address: " << miMAC << "\n";


    // En un principio pensaba que las MAC Address empezaban en 0 y todo el codigo está pensado para ello. Por eso se hace este ajuste aquí:
    miMAC = miMAC -1; // Siento la chapuza...
    // El BN es un caso especial. Ver el final de este trozo de código (en la inicialización de posicion_x y posicion_y) para ver cómo se trata a este nodo


    // Esto lo hacemos para que el primer SN tenga de posición el "0". Esto nos servirá para colocarlo en la posición 0 de la primera rama
    float miPosicionEnVectorDeSN = miMAC -1;

    // Porque si no el bucle for que viene a continuación se vuelve loco
    if (miMAC == 0){
        miPosicionEnVectorDeSN++;
    }


    // Mi rama es la parte entera de dividir mi índice SN entre el número de ramas. Los SN se van colocando de manera ordenada "rellenando" estas Branches.
    //float miRamaFloat = miPosicionEnVectorDeSN/nBranches;
    //int miRama = (int)(miRamaFloat-0.01);

    int miRama;
    int miPosicionEnLaRama;
    int counter = 0;
    for(int i = 0; i < nBranches; i++){
        for(int j = 0; j<nhostsPerBranch; j++){

            if(counter==miPosicionEnVectorDeSN){
                miRama = i;
                miPosicionEnLaRama = j;
            }

            counter = counter +1;
        }
    }



    // He asumido de aquí en adelante que la primera branch (la de más arriba) es la branch 0
    //miRama = miRama-1;

    // Mi posición en la rama es la parte decimal de dividir mi índice SN entre el número de ramas multiplicado por el número de objetos en una rama.
    // Mi posición puede ir desde 0 (el primero de la rama) hasta nhostPerBranch -1 (el último de la rama)
    //float miPosicionEnLaRamaFloat = ( (miPosicionEnVectorDeSN/nBranches) - miRama ) * nhostsPerBranch;
    //int miPosicionEnLaRama = round(miPosicionEnLaRamaFloat-0.01); // Para aproximar al entero más cercano porque no coge todos los decimales en el paso anterior


    // Mi posición X se obtiene dividiendo la longitud en "nhostsperBranch" trozos y después multiplicando por mi posición en la rama (ajustado +1 para que la pos 0 empiece a distancia del BN)
    float miX = (busLengthInMeters/nhostsPerBranch) * (miPosicionEnLaRama + 1);


    //EV << "Mi X: " << miX << "\n";
    //EV << "Mi rama: " << miRama << "\n";
    //EV << "Mi posicionenVectorSN: " << miPosicionEnVectorDeSN << "\n";
    //EV << "Mi posicionEnLaRamaFloat: " << miPosicionEnLaRamaFloat << "\n";
    //EV << "Mi posicionEnLaRama: " << miPosicionEnLaRama << "\n";


    // Ahora calculamos el Y de mi rama. Esta posición dependerá de dónde estes relativa al BN y depende de si las ramas son pares o no
    int yDelBaseNodeInt = par("posicionYdelBN");
    float yDelBaseNode = static_cast< float >(yDelBaseNodeInt);
    int verticalDistBetweenBusesInt = (cSimulation::getActiveSimulation())->getSystemModule() ->par("verticalDistanceBetweenBuses");
    float verticalDistBetweenBuses = static_cast< float >(verticalDistBetweenBusesInt);
    int indRama;
    float offsetMiRama;
    float yMiRama;

    // Según si las ramas son pares o no, se hace un cálculo de la Y distinto
    if (nBranchesInt%2 == 0){

        // Número par de ramas
        // miRama   índice_deseado
        //   0        2
        //   1        1
        //   2       -1
        //   3       -2

        if (miRama >= nBranchesInt/2){
            indRama = -miRama  + nBranchesInt/2 - 1; // Primero convertimos al índice. Estos están por debajo del BN y serán negativos
            offsetMiRama =  -( (verticalDistBetweenBuses/2) + (-indRama - 1) * verticalDistBetweenBuses); // Después calculamos el offset respecto al BN
        }
        else{

            indRama = nBranchesInt/2 - miRama; // Primero obtenemos el índice. Estos están por encima del BN
            offsetMiRama = (verticalDistBetweenBuses/2) + (indRama - 1) * verticalDistBetweenBuses; // Ahora el offset respecto al BN
        }

        yMiRama = yDelBaseNode + offsetMiRama;
    }
    else{

        // Número impar de ramas
        // miRama   índice_deseado
        //   0        2
        //   1        1
        //   2        0
        //   3       -1
        //   4       -2
        if (miRama >  nBranchesInt/2){
            indRama = -miRama + nBranchesInt/2;
            offsetMiRama = verticalDistBetweenBuses * indRama;

        }
        else if(miRama < nBranchesInt/2){
            indRama = nBranchesInt/2 - miRama;
            offsetMiRama = verticalDistBetweenBuses * indRama;
        }
        else{
            yMiRama = 0; // Esta rama está a la misma altura que el BN. Es la rama "central"
            indRama = yMiRama;
        }
        offsetMiRama = verticalDistBetweenBuses * indRama;
        yMiRama = yDelBaseNode + offsetMiRama;
    }


    float posicion_x = miX;
    float posicion_y = yMiRama;

    // Si soy el BN, estoy en la posición (0, yDelBaseNode)
    if (miMAC == 0){
        posicion_x = 0;
        posicion_y = yDelBaseNode;
    }

    // Cojo un puntero al display string de mi padre, que es la que va a mirar luego la radio

    cDisplayString& parentDispStr  = getParentModule() -> getDisplayString();


    // Una vez encontrada, cambio sus argumentos de posición
    parentDispStr.setTagArg("p",0,posicion_x);
    parentDispStr.setTagArg("p",1,posicion_y);


    // Y muestro la posición en el string del abuelo
    cDisplayString& abueloDispStr  = getParentModule()-> getParentModule() -> getDisplayString();
    string  strBase = "x: " + to_string(round(posicion_x)) + " y: " + to_string(round(posicion_y));
    abueloDispStr.setTagArg("t",0, strBase.c_str() );


    // Actualizo mi puntero a los mismos valores que ha tocado la capa MAC
    //parentDispStr.parse(grandParentDispStr);
    EV << "Mi parent string actualizado es: "<< parentDispStr <<"\n";



}



void RfPhysicalInterface::finish(){

    recordScalar("#ReceivedMessages", numReceivedMessages);
    recordScalar("#ErroneousMessages", numErroneousMessages);

}

