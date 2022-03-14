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

#include "PLCRFCombiner.h"

#include <global_variables.h>
#include <MACMsg_m.h>
#include <omnetpp/clog.h>
#include <omnetpp/cmodule.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/regmacros.h>
#include <PLCMAC.h>
#include <iostream>



Define_Module(PLCRFCombiner);
Register_Class(PLCRFCombiner);


PLCRFCombiner::PLCRFCombiner() {
    // TODO Auto-generated constructor stub

}

PLCRFCombiner::~PLCRFCombiner() {
    // TODO Auto-generated destructor stub
}

void PLCRFCombiner::handleMessage(cMessage *msg){
    EV << "Combiner has received the message\n";

    cModule *submoduloMAC = this ->getParentModule() -> getSubmodule("mac");
    PLCMAC *claseMAC = (PLCMAC*) submoduloMAC;

    int tipoNodo = claseMAC -> getNodeType();
    EV << "Combiner has determined that I am a type "<< tipoNodo <<"node. (0 PLC, 1 RF, 2 HYB)\n";
    bool puedoRecibir = canIHandleThisMessage(msg, tipoNodo);

    if (puedoRecibir){
        EV << "Combiner has determined that I can receive. Sending to MAC layer\n";
        send(msg, "upperLayerOut");
    }
    else{
        EV << "Combiner has determined that I cannot receive. Deleting message\n";
        delete(msg);
    }


}



bool PLCRFCombiner::canIHandleThisMessage(cMessage *msg, int nodeType){

    // Primero veo si este nodo debería haber recibido este mensaje dependiendo de qué tipo de mensaje es:
    //
    // Y dependiendom de qué tipo de nodo soy:
    // nodeType --> NODE_TYPE_PLC, NODE_TYPE_RF o NODE_TYPE_HYBRID
    // (MACMsg *) msg

    bool deberiaHaberRecibido;
    MACMsg *frame = (MACMsg *) msg;
    int interfaz_entrada = frame -> getHybridTransmitMode();


    if(nodeType == NODE_TYPE_HYBRID){
        EV << "Soy un nodo híbrido\n";
        deberiaHaberRecibido = true;
    }else if(nodeType == NODE_TYPE_PLC){
        EV << "Soy un nodo no híbrido PLC\n";
        if (interfaz_entrada == PLC_PREFERENCE){
            EV << "La interfaz de entrada de este mensaje es PLC\n";
            deberiaHaberRecibido = true;
        }else{
            EV << "La interfaz de entrada de este mensaje es RF\n";
            deberiaHaberRecibido = false;
        }

    }else{
        EV << "Soy un nodo no híbrido RF\n";
        if (interfaz_entrada == RF_PREFERENCE){
            EV << "La interfaz de entrada de este mensaje es RF\n";
            deberiaHaberRecibido = true;
        }else{
            EV << "La interfaz de entrada de este mensaje es PLC\n";
            deberiaHaberRecibido = false;
        }
    }

    return deberiaHaberRecibido;
}

