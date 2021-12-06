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
import inet.linklayer.ieee802154.Ieee802154NarrowbandInterface;
import inet.networklayer.common.NetworkInterface;


module WirelessNodeBasic
{
    parameters:

        bool sendMsgOnInitWN = default(false);
        int packetByteLength = default(90);

        @display("bgb=821.044,695.492");
    gates:
        input upperLayerIn;   // Aquí conectaremos la MAC de PRIME
        output upperLayerOut; // Aquí conectaremos la MAC de PRIME

    submodules:


        PHYLayerAbstraction: RfPhysicalInterface {
            parameters:
                @display("p=280.84,353.52798");
        }


        narrowbandInterface: Ieee802154NarrowbandInterface {
            parameters:
                *.interfaceTableModule = default(absPath(".interfaceTable")); // Esto funciona... al menos está corriendo en compilación y runtime
                
                radio.antenna.mobilityModule = default(exists(mobility) ? absPath(".mobility") : "");
                //radio.antenna.mobility.typename = default("StationaryMobility"); // Esto funciona. Encuentra el Stationary Mobility
                // Estas líneas son necesarias para definir el stationary mobility
                //radio.antenna.mobility.constraintAreaMaxX = 10000m;
				//radio.antenna.mobility.constraintAreaMinX = 0m;
				//radio.antenna.mobility.constraintAreaMaxY = 10000m;
				//radio.antenna.mobility.constraintAreaMinY = 0m;
				//radio.antenna.mobility.constraintAreaMaxZ = 0m;
				//radio.antenna.mobility.constraintAreaMinZ = 0m;
                
                @display("p=280.84,120.596");
        }


    connections allowunconnected:


        PHYLayerAbstraction.rfgateout --> narrowbandInterface.upperLayerIn;
        narrowbandInterface.upperLayerOut --> PHYLayerAbstraction.rfgatein;


}
