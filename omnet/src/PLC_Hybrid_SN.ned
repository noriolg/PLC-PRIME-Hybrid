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


import inet.physicallayer.wireless.common.contract.packetlevel.IRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;
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
        bool hasPRIME = default(firstAvailableOrEmpty("PRIME") != "");

        //*.interfaceTableModule = "^.interfaceTable"; // Already defined in WirelessNodeBasic
        //wlan.radio.antenna.mobilityModule = default("^.^.^.mobility"); // Already defined in WirelessNodeBasic
    gates:
        inout ethg @labels(EtherFrame-conn);
        input radioIn @directIn;

    submodules:

        visualizer: <default(firstAvailableOrEmpty("IntegratedCanvasVisualizer"))> like IIntegratedVisualizer if typename != "" {
            parameters:
                @display("p=45,128;is=s");
        }


        radioMedium: <default("Ieee802154NarrowbandScalarRadioMedium")> like IRadioMedium {
            @display("p=218,49");
        }




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


        plclan: PLCPHYSN if hasPRIME {
            parameters:
                @display("p=242,362,row,150;q=txQueue");
        }


		// Podemos sustituir todo esto por un WirelessNodeBasic
//        wlan: Ieee802154NarrowbandInterface if hasRF {
//            parameters:
//                @display("p=108,362;q=queue");
//                mgmt.typename = "Ieee80211MgmtStaSimplified";
//                agent.typename = "";
//        }
//        interfaceTable: InterfaceTable {
//            parameters:
//                @display("p=100,100;is=s");
//        }
//        mobility: StationaryMobility if hasRF {
//            parameters:
//                @display("p=100,200;is=s");
//        }

        rflan: WirelessNodeBasic if hasRF {
            @display("p=100,362");
        }
    connections allowunconnected:
        llc.lowerLayerIn <-- mac.upperLayerOut;
        llc.lowerLayerOut --> mac.upperLayerIn;


        BNApp.out --> llc.upperLayerIn;
        BNApp.in <-- llc.upperLayerOut;


		// Cambios aquí porque las puertas de mac NO son vectoriales.
		// No he querido cambiar el código de simPRIME2 

        //        plclan.upperLayerIn <-- mac.physOut++ if hasPRIME;
        //        plclan.upperLayerOut --> mac.physIn++ if hasPRIME;
        plclan.upperLayerIn <-- mac.physOut if hasPRIME;
        plclan.upperLayerOut --> mac.physIn if hasPRIME;



        //        mac.physIn++ <-- rflan.upperLayerOut if hasRF;
        //        mac.physOut++ --> rflan.upperLayerIn if hasRF;
        mac.physIn <-- rflan.upperLayerOut if hasRF;
        mac.physOut --> rflan.upperLayerIn if hasRF;



        ethg <--> { @display("m=s"); } <--> plclan.phys if hasPRIME;
        rflan.radioIn <-- radioIn if hasRF;
}


