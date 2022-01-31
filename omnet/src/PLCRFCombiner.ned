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
// Esta clase se crea para recibir mensajes de la capa física  RF y  PLC y pasarlos a la capa MAC híbrida
// De esta manera resolvemos el problema de dos puertas de entrada para la capa MAC y lo hacemos 'transparente'
//
module PLCRFCombiner
{

    
	gates:
		
		// Pueden entrar mensajes de RF o de PLC
		input rfgatein; 
		input plcgatein;
		
		output upperLayerOut;
    
    connections allowunconnected:
        
        //rfgatein --> upperLayerOut;
        //plcgatein --> upperLayerOut;
    	
}
