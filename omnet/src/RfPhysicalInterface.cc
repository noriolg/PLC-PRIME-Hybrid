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


            // Calling the folowing function to start with the network messages
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
    auto data =  makeShared<ByteCountChunk>(B(10));    // Creating a chunk with 10 bytes

    //data -> setChunkLength(B(packetByteLength))       // I can edit several data properties before inserting in packet

    Packet *packet = new Packet("RFPHYPacket", data);   // I create a packet with the "data" defined above

    long packetByteLength = long(par("packetByteLength"));
    EV << "Creating packet with a length of " << packetByteLength << " bytes (not really, still to be implemented)\n";

    send(packet, "rfgateout");
    //sendPacket(packet, "rfgateout")


}

void RfPhysicalInterface::forwardMessage(cMessage *msg)
{
    EV << "Sending message again\n";
    send(msg, "rfgateout");

    //    cPacket* packet = dynamic_cast<cPacket*>(msg);
    //
    //    if (packet -> hasBitError()){
    //        EV << "Packet has bit error\n";
    //
    //
    //        delete packet;
    //        // Esto habr�a que ser capaz de meterlo en una funci�n separada
    //        cPacket *packet_new = new cPacket("RFPacket_alt");
    //        long packetByteLength = long(par("packetByteLength"));
    //        packet_new->setByteLength(packetByteLength);
    //        EV << "New packet created\n";
    //
    //        forwardMessage(packet_new);
    //
    //    }else{
    //        EV << "Packet has arrived correctly\n";
    //        forwardMessage(packet);
    //    }

}

