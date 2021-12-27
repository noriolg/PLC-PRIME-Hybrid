// No consigo separar los distintos Nodos de la red de manera automática (parámetro display)



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

package hybridmeter.simulations.rfPhysicalAndChannel;

import hybridmeter.WirelessNodeBasic;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.physicallayer.wireless.common.contract.packetlevel.IRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;


network RfCommunication
{
    parameters:

        int numberOfServiceNodes = default(2);
        string BERFilename = default("");
        @display("bgb=391.68997,239.19998");



    submodules:


        visualizer: <default(firstAvailableOrEmpty("IntegratedCanvasVisualizer"))> like IIntegratedVisualizer if typename != "" {
            @display("p=580,125");
        }
//        configurator: Ipv4NetworkConfigurator {
//            @display("p=580,200");
//        }

        RfBaseNode: WirelessNodeBasic {
            @display("p=150,120");
        }

//        RfServiceNode[numberOfServiceNodes]: WirelessNodeBasic {
//
//            @display("p=70,180;row");  // 8.4.3.4 Positioning in Manual
//
//        }

        RfServiceNodeA: WirelessNodeBasic {

            @display("p=50,180");
        }

//        RfServiceNodeB: WirelessNodeBasic {
//
//            @display("p=150,180");
//        }
//
//        RfServiceNodeC: WirelessNodeBasic {
//
//            @display("p=250,180");
//        }


        radioMedium: <default("Ieee802154NarrowbandScalarRadioMedium")> like IRadioMedium {
            @display("p=218,49");
        }

    connections allowunconnected:
//        RfBaseNode.radioIn --> Channel --> RfServiceNode.rfgatein;
//
//        RfServiceNode.rfgateout --> Channel --> RfBaseNode.rfgatein;



}
