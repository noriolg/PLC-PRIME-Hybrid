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


Define_Module(RfPhysicalInterface);
Register_Class(RfPhysicalInterface);

RfPhysicalInterface::RfPhysicalInterface() {
    // TODO Auto-generated constructor stub

}

RfPhysicalInterface::~RfPhysicalInterface() {
    // TODO Auto-generated destructor stub
}


void RfPhysicalInterface::initialize(){

    EV << "I am initialized\n";
    if( par("sendMsgOnInit").boolValue() == true) {

        // We schedule an initial auto-message. When it arrives, we will send the first network message
        EV << "Creating INITSELFMSG\n";
        cMessage * msg = new cMessage("InitialSelfMessage", INITSELFMSG);
        scheduleAt(simTime()+ 1.0,msg );

    }

}


void RfPhysicalInterface::handleMessage(cMessage *msg){
    EV << "Some message was received\n";

    if (msg->isSelfMessage()){
        handleSelfMessage(msg);
    }
    else{
        forwardMessage(msg);
    }

}


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


void RfPhysicalInterface::sendInitialNetworkMessage(){

    // Tested option 1
    // cMessage *msg = new cMessage("RFMessage", TESTPHYMSG);
    // Packet *packet = dynamic_cast<Packet*>(msg);

    // Tested option 2
    //Packet *packet = new Packet("RFPHYPacket");
    //long packetByteLength = long(par("packetByteLength"));
    //packet->setByteLength(packetByteLength);

    //Tested option 3
    EV << "We are now about to create the initial network message";

    //auto rfMsg= makeShared<RfMsg>();                  //I will be able to create by own messages at some point
    long packetByteLength = long(par("packetByteLength"));
    auto data =  makeShared<ByteCountChunk>(B(packetByteLength));

    Packet *packet = new Packet("RFPHYPacket", data);   // I create a packet with the "data" defined above
    packet->addTagIfAbsent<MacAddressReq>()->setDestAddress(MacAddress::BROADCAST_ADDRESS);
    packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::ieee8021ae);

    EV << "Creating packet with a length of " << packetByteLength << " bytes\n";

    send(packet, "rfgateout");
    //sendPacket(packet, "rfgateout")


}

void RfPhysicalInterface::forwardMessage(cMessage *msg)
{

    EV << "Simulating error for received message\n";
    bool hasBitError = simulateBER(msg);


    if (hasBitError){


        EV << "Packet has bit error. Deleting received message and creating a new one\n";
        delete msg;

        long packetByteLength = long(par("packetByteLength"));
        auto data =  makeShared<ByteCountChunk>(B(packetByteLength));

        Packet *packet = new Packet("RFPHYPacket_alt", data);   // I create a packet with the "data" defined above
        packet->addTagIfAbsent<MacAddressReq>()->setDestAddress(MacAddress::BROADCAST_ADDRESS);
        packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::ieee8021ae);


        EV << "Sending new message\n";
        send(packet, "rfgateout");
    }else{

        EV << "Packet does not have bit error. Returning same message\n";

        Packet *packet = dynamic_cast<Packet*>(msg);
        packet->addTagIfAbsent<MacAddressReq>()->setDestAddress(MacAddress::BROADCAST_ADDRESS);
        packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::ieee8021ae);

        EV << "Sending new message\n";
        send(packet, "rfgateout");
    }

}

bool RfPhysicalInterface::simulateBER(cMessage *msg){

    bool errorInMsg;

    if (uniform(0,1) < 0.5){
        EV << "Error in message\n";
        errorInMsg = true;

    }else{
        EV << "No error in message\n";
        errorInMsg = false;
    }

    return errorInMsg;
}


//long double RfPhysicalInterface::readBERDataFromFile(){
//
//    std::string fileName = par("BERFilename"); // Esto hay que ver el path donde lo buscamos
//
//    // Estos son los datos leídos
//    bool FEC;
//
//
//    std::ifstream inf(fileName.c_str());
//
//    if (inf.is_open()){
//
//        std::string BER;
//        std::string SNR;
//
//        std::string aucComMode;
//        getline(inf,auxComMode);
//        EV << "Leo Communication Mode: " << auxComMode << "\n";
//
//        std::string auxFEC;
//        getline(inf,auxFEC);
//        EV << "Leo FEC: " << auxFEC<< "\n";
//
//        if (auxFEC.compare("false")==0){
//            FEC=false;
//        }
//
//        if (auxFEC.compare("true")==0){
//            FEC=true;
//        }
//
//        std::string auxModulation;
//        getline(inf,auxModulation);
//        EV << "Leo Modulation: " << auxModulation<< endl;
//
//        // Aquí empiezo a leer todos los puntos de BER para esta modulación
//
//        // Falta terminar de ajustar esta parte de la lectura
//        while(!std::getline(inf,BER,'|').eof())
//            //while(!inf.eof())
//        {
//            getline(inf,SNR);
//
//            EV << "Leo BER: " << BER << "Leo SNR: " << SNR << endl;
//
//            from_string<float>(auxBER, BER, std::dec);
//            from_string<float>(auxSNR, SNR, std::dec);
//            //BERvector.push_back(auxBER);
//            //SNRvector.push_back(auxSNR);
//        }
//
//        //EV <<"Inserting data in berData list" <<endl;
//        //berData->insert(new BERData(auxComMode, FEC,auxModulation, SNRvector, BERvector ));
//    }
//    else {
//        error("File not found");
//    }
//
//}
