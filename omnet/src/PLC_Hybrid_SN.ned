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

package hybridmeter;

//
// TODO auto-generated module
//


import inet.common.MessageDispatcher;
import inet.linklayer.ieee802154.Ieee802154NarrowbandInterface;
import inet.mobility.static.StationaryMobility;
import inet.networklayer.common.InterfaceTable;
import simprime2.DLMSCOSEMSN;
import simprime2.PLCLLCSN;
import simprime2.PLCMACSN;
import simprime2.PLCPHYSN;


module PLC_Hybrid_SN
{
    parameters:
        @networkNode();
        @labels(node,ethernet-node);
        @display("p=155,120;i=misc/house");

        bool hasRF = default(firstAvailableOrEmpty("RF") != "");

        *.interfaceTableModule = "^.interfaceTable";
        wlan.radio.antenna.mobilityModule = default("^.^.^.mobility");
    gates:
        inout ethg @labels(EtherFrame-conn);
        input radioIn @directIn;

    submodules:
        BNApp: DLMSCOSEMSN {
            parameters:
                registerSAP = true;
                @display("p=155,40,col");
        }

        llc: PLCLLCSN {
            parameters:
                @display("p=155,120;q=txQueue");
                baseNode = true;
        }
        mac: PLCMACSN {
            parameters:
                @display("p=155,200;q=txQueue");
        }


        plclan: PLCPHYSN {
            parameters:
                @display("p=242,362,row,150;q=txQueue");
        }


        wlan: Ieee802154NarrowbandInterface {
            parameters:
                @display("p=108,362;q=queue");
                mgmt.typename = "Ieee80211MgmtStaSimplified";
                agent.typename = "";
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=100,100;is=s");
        }
        mobility: StationaryMobility {
            parameters:
                @display("p=100,200;is=s");
        }



    connections:
        llc.lowerLayerIn <-- mac.upperLayerOut;
        llc.lowerLayerOut --> mac.upperLayerIn;


        BNApp.out --> llc.upperLayerIn;
        BNApp.in <-- llc.upperLayerOut;


        plclan.upperLayerIn <-- mac.physOut++;
        plclan.upperLayerOut --> mac.physIn++;


        mac.physIn++ <-- wlan.upperLayerOut;
        mac.physOut++ --> wlan.upperLayerIn;


        ethg <--> { @display("m=s"); } <--> plclan.phys;
        wlan.radioIn <-- radioIn;
}


