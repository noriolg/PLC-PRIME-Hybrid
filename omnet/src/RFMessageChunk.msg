import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace inet;

//
// Generic application packet
//
// TODO rename to ApplicationHeader and remove ByteCountChunk from ApplicationHeaderSerializer
class RFMessageChunk extends FieldsChunk
{
    cPacket *mensajeMac;
}