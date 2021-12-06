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

package hybridmeter.simulations.rfPhysicalAndChannel;

import hybridmeter.WirelessNodeBasic;
import inet.physicallayer.wireless.common.contract.packetlevel.IRadioMedium;


network RfCommunication
{
    parameters:
        @display("bgb=295,240");

// Esto con el radiochannel no debería ser necesario
//    types:
//
//        channel Channel extends ned.DatarateChannel
//        {
//            //@signal[messageSent](type=omnetpp::cMessage);
//            //@signal[messageDiscarded](type=omnetpp::cMessage);
//            delay = default(100ms);
//            per = default(0.1);
//        }

    submodules:

//        RfChannel: RfRadioChannel {
//
//            @display("p=300,134;b=63,64,rect;o=#408060");
//        }


        RfBaseNode: WirelessNodeBasic {
            @display("p=218,134");
        }

        RfServiceNode: WirelessNodeBasic {
            @display("p=89,134");
        }

        radioMedium: <default("Ieee802154NarrowbandScalarRadioMedium")> like IRadioMedium {
            @display("p=218,49");
        }

    connections allowunconnected:
//        RfBaseNode.radioIn --> Channel --> RfServiceNode.rfgatein;
//
//        RfServiceNode.rfgateout --> Channel --> RfBaseNode.rfgatein;



}
