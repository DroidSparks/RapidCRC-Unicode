/*
The Keccak sponge function, designed by Guido Bertoni, Joan Daemen,
Michaël Peeters and Gilles Van Assche. For more information, feedback or
questions, please refer to our website: http://keccak.noekeon.org/

Implementation by the designers,
hereby denoted as "the implementer".

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <string.h>
#include "KeccakNISTInterface.h"
#include "KeccakF-1600-interface.h"

Sha3HashReturn Sha3Init(sha3hashState *state, int hashbitlen)
{
    switch(hashbitlen) {
        case 0: // Default parameters, arbitrary length output
            InitSponge((spongeState*)state, 1024, 576);
            break;
        case 224:
            InitSponge((spongeState*)state, 1152, 448);
            break;
        case 256:
            InitSponge((spongeState*)state, 1088, 512);
            break;
        case 384:
            InitSponge((spongeState*)state, 832, 768);
            break;
        case 512:
            InitSponge((spongeState*)state, 576, 1024);
            break;
        default:
            return BAD_HASHLEN;
    }
    state->fixedOutputLength = hashbitlen;
    return SUCCESS;
}

Sha3HashReturn Sha3Update(sha3hashState *state, const BitSequence *data, DataLength databitlen)
{
    if ((databitlen % 8) == 0)
        return Absorb((spongeState*)state, data, databitlen);
    else {
        Sha3HashReturn ret = Absorb((spongeState*)state, data, databitlen - (databitlen % 8));
        if (ret == SUCCESS) {
            unsigned char lastByte; 
            // Align the last partial byte to the least significant bits
            lastByte = data[databitlen/8] >> (8 - (databitlen % 8));
            return Absorb((spongeState*)state, &lastByte, databitlen % 8);
        }
        else
            return ret;
    }
}

Sha3HashReturn Sha3Final(sha3hashState *state, BitSequence *hashval)
{
    return Squeeze(state, hashval, state->fixedOutputLength);
}

Sha3HashReturn Sha3Hash(int hashbitlen, const BitSequence *data, DataLength databitlen, BitSequence *hashval)
{
    sha3hashState state;
    Sha3HashReturn result;

    if ((hashbitlen != 224) && (hashbitlen != 256) && (hashbitlen != 384) && (hashbitlen != 512))
        return BAD_HASHLEN; // Only the four fixed output lengths available through this API
    result = Sha3Init(&state, hashbitlen);
    if (result != SUCCESS)
        return result;
    result = Sha3Update(&state, data, databitlen);
    if (result != SUCCESS)
        return result;
    result = Sha3Final(&state, hashval);
    return result;
}

