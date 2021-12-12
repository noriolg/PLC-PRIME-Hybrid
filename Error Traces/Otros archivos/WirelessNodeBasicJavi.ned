// Problems with the mobility module


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
// TODO auto-generated type
//

import inet.common.lifecycle.NodeStatus;
import inet.common.packet.recorder.PcapRecorder;
import inet.linklayer.contract.IWirelessInterface;
import inet.mobility.static.StationaryMobility;
import inet.networklayer.common.InterfaceTable;
import inet.physicallayer.wireless.common.contract.packetlevel.IAntenna;


module WirelessNodeBasicJavi2 //extends NodeBase
{
    parameters:
        bool hasStatus = default(false);

        @display("i=misc/sensor2");
        @figure[submodules];
        energyStorage.typename = default("IdealEpEnergyStorage");
        wlan[*].typename = default("Ieee802154NarrowbandInterface");
        wlan[*].radio.energyConsumer.typename = default("SensorStateBasedEpEnergyConsumer"); //FIXME does this consumer exist???


        pcapRecorder[*].pcapFile = default("results/" + fullPath() + ".pcap");
        mobility.typename = default("StationaryMobility");
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        wlan.radio.antenna.mobilityModule = default("^.^.^.mobility");
        @figure[linkLayer](type=rectangle; pos=250,456; size=1000,130; fillColor=#0000ff; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[linkLayer.title](type=text; pos=1245,461; anchor=ne; text="link layer");
        @figure[interfaceLayer](type=rectangle; pos=250,606; size=1000,210; fillColor=#00ffff; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[interfaceLayer.title](type=text; pos=1245,611; anchor=ne; text="interface layer");



        //mobility.typename = default("StationaryMobility");
        antenna.mobilityModule = default("^.^.^.mobility");

        *.mobilityModule = default(absPath(".mobility"));
		//mobility.mobilityModule = default("^.^.mobility");
//        narronarrowbandRadio.an
//        antenna.mobilityModule = default("^.^.^.mobility");

		// Valid for MAC & PHY abstraction
        wlan.radio.antenna.mobilityModule = default("^.^.^.mobility"); // https://inet.omnetpp.org/docs/showcases/mobility/combining/doc/index.html
        *.interfaceTableModule = default(absPath(".interfaceTable"));  // Interface table used in combination with mac interface

        bool sendMsgOnInitWN = default(false);
        int packetByteLength = default(90);

        @display("i=misc/sensor2");
        @display("bgb=821.044,695.492");
        @networkNode();
        
         @networkNode;
        @labels(node,ethernet-node,wireless-node);


    gates:
        input upperLayerIn;   // AquÃ­ conectaremos la MAC de PRIME
        output upperLayerOut; // AquÃ­ conectaremos la MAC de PRIME
        input radioIn @directIn;

    submodules:
        status: NodeStatus if hasStatus {
            parameters:
                @display("p=125,80;is=s");
        }

        mobility: StationaryMobility {
            parameters:
                @display("p=100,200;is=s");
        }

        antenna: <default("IsotropicAntenna")> like IAntenna {
            parameters:
                @display("p=100,200");
        }



        PHYLayerAbstraction: RfPhysicalInterface {
            parameters:
                @display("p=280.84,353.52798");
        }


        pcapRecorder: PcapRecorder {
            parameters:
                @display("p=125,640;is=s");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=125,240;is=s");
        }


//        narrowbandRadio: Ieee802154NarrowbandScalarRadio {
//            parameters:
//
//                antenna.mobilityModule = default("^.^.mobility");
//                @display("p=280.84,120.596");
//                @node;
//        }



// Relevant for MAC & PHY Abstraction
//        interfaceTable: InterfaceTable {
//            parameters:
//                @display("p=125,240;is=s");
//        }

        wlan: <default("Ieee802154NarrowbandInterface")> like IWirelessInterface {
            parameters:
                @display("p=375,766,row,150;q=queue");
        }

      

		// Ieee802154NarrowbandScalarRadio  // Este solo PHY
        //test: Ieee802154NarrowbandInterface;
    connections allowunconnected:

        // upperLayerIn --> PHYLayerAbstraction.upperLayerIn;  //  Gate "upperLayerIn" is not created yet in the RfPhysicalInterface.c file
        PHYLayerAbstraction.rfgateout --> wlan.upperLayerIn;  // rfgateout will be renamed to "lowerLayerOut"


        wlan.upperLayerOut --> PHYLayerAbstraction.rfgatein;  //rfgatein will be renamed to "lowerLayerIn"
        //PHYLayerAbstraction.uperLayerOut --> upperLayerOut; //  Gate "upperLayerOut" is not created yet in the RfPhysicalInterface.c file 

}
