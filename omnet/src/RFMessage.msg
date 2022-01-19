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

import inet.common.INETDefs;
import inet.common.TagBase;

namespace inet;

class RFMessageAddressTagBase extends TagBase
{
    long srcAddress;  
    long destAddress; 
}


//
// Determines the source and destination MAC addresses for an outgoing packet.
//
class RFMacAddressReq extends RFMessageAddressTagBase
{
}

//
// Specifies the source and destination MAC addresses for an incoming packet.
//
class RFMacAddressInd extends RFMessageAddressTagBase
{
}