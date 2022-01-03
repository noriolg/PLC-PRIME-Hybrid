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

import inet.linklayer.ieee802154.Ieee802154NarrowbandInterface;
import inet.mobility.static.StationaryMobility;
import inet.networklayer.common.InterfaceTable;


module WirelessNodeBasic //extends NodeBase
{
    parameters:

        *.interfaceTableModule = "^.interfaceTable";
        wlan.radio.antenna.mobilityModule = default("^.^.^.mobility");

        bool sendMsgOnInitWN = default(false);
        int packetByteLength = default(90);
		string dummyVar = "This is dummy"; // Delete this variable. It is for testing accessing variables


        @display("i=misc/sensor2");
        @display("bgb=821.044,695.492");
		
		
        @networkNode();


    gates:
        input upperLayerIn;   // AquÃ­ conectaremos la MAC de PRIME
        output upperLayerOut; // AquÃ­ conectaremos la MAC de PRIME
        input radioIn @directIn;

    submodules:

        PHYLayerAbstraction: RfPhysicalInterface {
            parameters:
                @display("p=280.84,353.52798");
        }


        wlan: Ieee802154NarrowbandInterface {
            parameters:
                @display("p=250,200;q=queue");
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

    connections allowunconnected:

        upperLayerIn --> PHYLayerAbstraction.upperLayerIn; // Comment this line when running RfCommunication.ned. This is only for hybrid connectivity
        PHYLayerAbstraction.rfgateout --> wlan.upperLayerIn;  // rfgateout will be renamed to "lowerLayerOut"
        wlan.radioIn <-- radioIn;


        wlan.upperLayerOut --> PHYLayerAbstraction.rfgatein;  //rfgatein will be renamed to "lowerLayerIn"
        PHYLayerAbstraction.upperLayerOut --> upperLayerOut; // Comment this line when running RfCommunication.ned. This is only for hybrid connectivity
		         

}
