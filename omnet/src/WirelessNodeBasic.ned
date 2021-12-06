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
// TODO auto-generated type
//

import inet.linklayer.contract.IWirelessInterface;
import inet.networklayer.common.InterfaceTable;
import inet.node.base.NodeBase;


module WirelessNodeBasic extends NodeBase
{
    parameters:
        int numPcapRecorders = default(0);
        int numLoInterfaces = default(1);
        int numWlanInterfaces = default(1);
        int numEthInterfaces = default(0);  // minimum number of ethernet interfaces
        int numPppInterfaces = default(0);  // minimum number of PPP interfaces
        int numTunInterfaces = default(0);
        int numVlanInterfaces = default(0);
        mobility.typename = default(numWlanInterfaces > 0 ? "StationaryMobility" : "");
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        wlan.radio.antenna.mobilityModule = default("^.^.^.mobility");
        @figure[linkLayer](type=rectangle; pos=250,456; size=1000,210; fillColor=#0000ff; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[linkLayer.title](type=text; pos=1245,461; anchor=ne; text="link layer");

        bool sendMsgOnInitWN = default(false);
        int packetByteLength = default(90);

    gates:
        input radioIn @directIn;

    submodules:

        interfaceTable: InterfaceTable {
            parameters:
                @display("p=125,240;is=s");
        }

        // TODO: move wlan interfaces after eth interfaces, but it changes IP address assignment and breaks examples/inet/configurator/complex.ini
        // scr/inet/linklayer/ieee802114/
        wlan: <default("Ieee80211Interface")> like IWirelessInterface {
            parameters:
                @display("p=375,616,row,150;q=queue");
        }

        PHYLayerAbstraction: RfPhysicalInterface {
            @display("p=514,134");
            sendMsgOnInit = parent.sendMsgOnInitWN;
            packetByteLength = parent.packetByteLength;
        }



    connections allowunconnected:

        radioIn --> { @display("m=s"); } --> wlan.radioIn;

        PHYLayerAbstraction.rfgatein <-- wlan.upperLayerOut;
        PHYLayerAbstraction.rfgateout --> wlan.upperLayerIn;


}
