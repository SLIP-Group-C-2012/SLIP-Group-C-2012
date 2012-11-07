/* Header file for the Audio Decoder for Group C of SLIP 2012 */

/* Initializes the compression parameters.

    src_packet_size - the size of the packet that would be offered as a src to compress
    and will be returned as dst to uncompress. Should be a power of 2.

    compression_packet_size - the required size of the packet after compression. Should be less than src_packet_size.
    The value should be a multiple of 2 (with some limitations) + 1. There is one sync bit used to enable loss tolerance.

    Example values -
        src_packet_size = 64; compressed_packet_size = 17; This is 64/17 = 3.76x compression. I.e. 8 bits compress to 2 bits (terrible quality)
        src_packet_size = 64; compressed_packet_size = 25; This is 64/25 = 2.56x compression. I.e. 8 bits compress to 3 bits (low quality)
        src_packet_size = 64; compressed_packet_size = 33; This is 64/33 = 1.94x compression. I.e. 8 bits compress to 4 bits (moderate quality)
 */
void set_up_compression(int src_packet_size, int compressed_packet_size);

/* Expects src array of PCM data of length specified by src_packet_size during set_up_compression.
Returns dst array of compressed data of length compressed_packet_size specified during set_up_compression.
The resulting array could be stored/transmitted to be decoded via uncompress back to PCM data */
void compress(unsigned char * src, unsigned char * dst);

/* Expects src array of compressed data of length specified by compressed_packet_size during set_up_compression.
Returns dst array of PCM data of length src_packet_size also specified during set_up_compression.
The resulting array could be played back directly as it is. */
void uncompress(unsigned char * src, unsigned char * dst);
