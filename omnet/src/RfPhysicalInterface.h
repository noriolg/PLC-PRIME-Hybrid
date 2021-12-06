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

#ifndef RFPHYSICALINTERFACE_H_
#define RFPHYSICALINTERFACE_H_

#include <omnetpp.h>
#include "global_variables.h"
#include "inet/common/packet/Packet.h"

using namespace inet;


class INET_API RfPhysicalInterface : public cSimpleModule {
public:
    int address;

    public:
        RfPhysicalInterface();
        virtual ~RfPhysicalInterface();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        void forwardMessage(cMessage *msg);

        /**
            Describes the treatment of all handle

            @param msg The received self message
        */
        void handleSelfMessage(cMessage *msg);


        /**
            When a node receives the inisitalization self message (INITSELFMSG) it creates and sends an initial network message.
        */
        void sendInitialNetworkMessage();


        //cPacket RfPhysicalInterface::createPacket();
};

#endif /* RFPHYSICALINTERFACE_H_ */
