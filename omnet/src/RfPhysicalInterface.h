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
#include <math.h>
#include <fstream>
#include <string>
#include <map>
#include "global_variables.h"
#include "inet/common/packet/Packet.h"
#include <inet/common/packet/chunk/ByteCountChunk.h>
#include <inet/linklayer/common/MacAddress.h>
#include <inet/linklayer/common/MacAddressTag_m.h>
#include "inet/common/Protocol.h"
#include "inet/common/ProtocolTag_m.h"

#include "inet/physicallayer/wireless/common/contract/packetlevel/IRadio.h"

using namespace inet;


class INET_API RfPhysicalInterface : public cSimpleModule {

    private:

        // For statistics collection
        int numReceivedMessages;
        int numErroneousMessages;

         inet::physicallayer::IRadio * radio;

    public:
        bool soyRF;
        RfPhysicalInterface();
        virtual ~RfPhysicalInterface();

    protected:
        virtual void initialize(int stage) override;
        void inicializarPosicionRadio();

        void loadBERCurveFile();
        virtual void finish() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual int numInitStages() const override { return NUM_INIT_STAGES; }


        void handleSelfMessage(cMessage *msg);
        void sendInitialNetworkMessage();

        void checkArrivalGate(cMessage *msg);
        void processMsgFromUpperLayer(cMessage *msg);
        void processMsgFromNetwork(cMessage *msg);

        void forwardMessage(Packet *packet);

        bool simulateError(Packet *packet);
        float computeSNR(Packet *packet);
        float obtainBERforSNR(float SNR_mensaje);
        float obtenerSNRValidaSuperior(float SNR_a_redondear);
        int roundUp(int numToRound, int multiple);
        float leerBERforSNRfromFile(float SNR_a_leer);

        void changeNodeImage(string  strBase);

};

#endif /* RFPHYSICALINTERFACE_H_ */
