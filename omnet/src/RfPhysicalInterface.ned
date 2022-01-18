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
simple RfPhysicalInterface
{
    parameters:
        @class(RfPhysicalInterface);
        
        int packetByteLength = default(80);
        bool sendMsgOnInit = default(false);
		
		// Esto es un poco lío pero no sé cómo acceder a parámetros del .ini directamente        
        string BERFilename = default("");
        bool standaloneRFNetwork = default(true);
        
       string radioModule = default("^.wlan.radio");   // The path to the Radio module  //FIXME remove default value
        
        
    gates:
        input rfgatein; //lowerLayerIn
		output rfgateout;//lowerLayerOut
		input upperLayerIn;
		output upperLayerOut;
}
