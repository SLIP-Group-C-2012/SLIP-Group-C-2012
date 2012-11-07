#include "codec.h"
#include <string.h>

#define ABS(a) (((a) < 0) ? -(a) : (a))

int packet_size; /* incoming packet size */
int output_packet_size; /* outgoing packet size */
int bps; /* how may bits does one sample contain */
char steps[256]; /* storing the differences lookup table. TODO! 256 is only neeeded if packet_size==output_packet_size which may not happen! */
int no_steps; /* no of actual values in the array steps */
int mask; /* bit mask for sieving bits from a byte */
int compressprev; /* running sum for the compression */
int decompressprev; /* running sum for the decompression */

void set_up_compression(int src_packet_size, int compressed_packet_size)
{
    int i;
    int stepso2;

    compressprev = 0;
    decompressprev = 0;
    packet_size = src_packet_size;
    output_packet_size = compressed_packet_size - 1; /* we actually have one control bit */

    bps = (8 * output_packet_size) / packet_size;

    no_steps = 1 << bps; /* 2 to the power of bps */
    mask = no_steps - 1; /* if 2 bps, the mask would be 0xFF, if 8 bps it would be 0xFFF, etc */

    /* Heuristics to come up with values of the steps. This could be tweaked for better results */
    stepso2 = no_steps / 2;
    if (no_steps == 2)
    {
        steps[0] = -1;
        steps[1] = 1;
    } else if (no_steps == 4)
    {
        steps[0] = -8;
        steps[1] = -1;
        steps[2] = 1;
        steps[3] = 8;
    } else if (no_steps == 8)
    {
        steps[0] = -10;
        steps[1] = -6;
        steps[2] = -2;
        steps[3] = -1;
        steps[4] = 0;
        steps[5] = 1;
        steps[6] = 3;
        steps[7] = 7;
    }
    else
        for (i = 0; i < no_steps; i++)
            steps[i] = i - stepso2;

}

void compress(unsigned char * src, unsigned char * dst)
{
    int i;
    int delta;
    unsigned int jid;
    int dstpos;
    int bitpadding;

    /* set the control byte */
    dst[output_packet_size] = compressprev;

    /* set all outputs to 0 */
    memset((void *) dst, 0, output_packet_size);

    /* compress each incoming byte */
    for (i = 0; i < packet_size; i++)
    {
        /* Find the difference */
        delta = src[i] - compressprev;

        /* Find the id of the step that best approximates delta */
        jid = 0;
        if (delta > steps[0])
        {
            while (delta > steps[jid++] && jid < no_steps) {};
            jid--;
        }

        /* Pack the value tight into the outgoing array */
        dstpos = bps * i / 8;
        bitpadding = 8 - bps * i + dstpos * 8 - bps;

        if (bitpadding < 0)
        {
            dst[dstpos] = dst[dstpos] | ( jid >> (-bitpadding));
            dst[dstpos+1] = dst[dstpos+1] | ( jid << (8 + bitpadding));
        }
        else
            dst[dstpos] = dst[dstpos] | ( jid << bitpadding);

        /* Store for finding next delta */
        compressprev += steps[jid];
    }
}

void uncompress(unsigned char * src, unsigned char * dst)
{
    int i;

    /* read the control byte. Helps in case of packet loss */
    decompressprev = src[output_packet_size];

    /* For each byte we want to decompress */
    for (i = 0; i < packet_size; i++)
    {

        /* Find position in the tightly packet incoming stream */
        int srcpos = bps * i / 8;
        int bitpadding = 8 - bps * i + srcpos * 8 - bps;
        int deltaid;

        if (bitpadding > 0)
            deltaid = ( src[srcpos] >> bitpadding ) & mask;
        else
            deltaid=((src[srcpos] << (-bitpadding)) | (src[srcpos+1] >> (8+bitpadding)) ) & mask;

        /* Calculate the expected value */
        int value = decompressprev + steps[deltaid];
        if (value > 255) value = 255;
        if (value < 0) value = 0;

        /* Store it */
        dst[i] = value;

        /* Use for the next one */
        decompressprev += steps[deltaid];
    }

}
