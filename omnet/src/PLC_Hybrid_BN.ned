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
import simprime2.DLMSCOSEMBN;
import simprime2.PLCLLCBN;
import simprime2.PLCMACBN;
import simprime2.PLCPHYBN;


module PLC_Hybrid_BN
{
    parameters:
        @networkNode();
        @labels(node,ethernet-node);
        @display("i=device/server2");

        bool hasRF = default(firstAvailableOrEmpty("RF") != "");

        *.interfaceTableModule = "^.interfaceTable";
        wlan.radio.antenna.mobilityModule = default("^.^.^.mobility");

    gates:
        inout ethg @labels(EtherFrame-conn);
        input radioIn @directIn;

    submodules:
        BNApp: DLMSCOSEMBN {
            parameters:
                registerSAP = true;
                @display("p=155,40,col");
        }

        llc: PLCLLCBN {
            parameters:
                @display("p=155,120;q=txQueue");
                baseNode = true;
        }
        mac: PLCMACBN {
            parameters:

                @display("p=155,200;q=txQueue");
                interPromotionTime = default(20);
        }


        plclan: PLCPHYBN {
            parameters:
                @display("p=242,362,row,150;q=txQueue");
        }



//        PHYLayerAbstraction: RfPhysicalInterface {
//            parameters:
//                @display("p=280.84,353.52798");
//        }

        li: MessageDispatcher {
            parameters:
                @display("p=153,293;b=263,12,,,,1");
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


//        PHYLayerAbstraction.rfgateout --> wlan.upperLayerIn;
//        PHYLayerAbstraction.rfgatein <-- wlan.upperLayerOut;

        li.in++ <-- mac.physOut;
        li.out++ --> mac.physIn;


        li.in++ <-- wlan.upperLayerOut;
        li.out++ --> wlan.upperLayerIn;

        plclan.upperLayerOut --> li.in++;
        plclan.upperLayerIn <-- li.out++;

        ethg <--> { @display("m=s"); } <--> plclan.phys;
        wlan.radioIn <-- radioIn;


}


