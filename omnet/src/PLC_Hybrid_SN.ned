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

        SNApp: DLMSCOSEMSN {
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

        // Este combiner SOLO combina el tráfico de entrada.
        // Para salida, el nodo solo utilizará una de las dos comunicaciones pero tiene que ser capaz de escuchar ambas
        lanCombiner: PLCRFCombiner {

            @display("p=155,308");
        }
        plclan: PLCPHYSN if hasPRIME {
            parameters:
                @display("p=250,347,row,150;q=txQueue");
        }

        rflan: WirelessNodeBasic if hasRF {

            @display("p=65,347");
        }

    connections allowunconnected:
        SNApp.out --> llc.upperLayerIn;
        SNApp.in <-- llc.upperLayerOut;


        llc.lowerLayerIn <-- mac.upperLayerOut;
        llc.lowerLayerOut --> mac.upperLayerIn;


		// Esto con combiner
        mac.physPLCOut --> plclan.upperLayerIn if hasPRIME;
        mac.physRFOut --> rflan.upperLayerIn if hasRF;

        mac.physPLCIn <-- lanCombiner.upperLayerOut; // Con el caso de combiner el PLCIN está agregando ambos tráficos

        lanCombiner.plcgatein <-- plclan.upperLayerOut if hasPRIME;
        lanCombiner.rfgatein <-- rflan.upperLayerOut if hasRF;
        // Hasta aquí

		// Esto sería sin combiner
        //plclan.upperLayerIn <-- mac.physPLCOut if hasPRIME;
        //plclan.upperLayerOut --> mac.physPLCIn if hasPRIME;

        //mac.physRFIn <-- rflan.upperLayerOut if hasRF;
        //mac.physRFOut --> rflan.upperLayerIn if hasRF;
        // Hasta aquí



        ethg <--> { @display("m=s"); } <--> plclan.phys if hasPRIME;
        rflan.radioIn <-- radioIn if hasRF;
}


