/*
 *  sha1.h
 *
 *  Description:
 *      This is the header file for code which implements the Secure
 *      Hashing Algorithm 1 as defined in FIPS PUB 180-1 published
 *      April 17, 1995.
 *
 *      Many of the variable names in this code, especially the
 *      single character names, were used because those were the names
 *      used in the publication.
 *
 *      Please read the file sha1.c for more information.
 *
 */

/*
 * If you do not have the ISO standard stdint.h header file, then you
 * must typdef the following:
 *    name              meaning
 *  uint32_t         unsigned 32 bit integer
 *  uint8_t          unsigned 8 bit integer (i.e., unsigned char)
 *  int_least16_t    integer of >= 16 bits
 *
 */

#define SHA1HashSize	20
/*
 *  This structure will hold context information for the SHA-1
 *  hashing operation
 */
typedef struct SHA1Context
{
    ULONG Intermediate_Hash[SHA1HashSize/4]; /* Message Digest  */

    ULONG Length_Low;            /* Message length in bits      */

                               /* Index into message block array   */
    USHORT Message_Block_Index;
    UCHAR Message_Block[64];      /* 512-bit message blocks      */
} SHA1Context;

/*
 *  Function Prototypes
 */

void SHA1Reset(SHA1Context * context);
void SHA1Input(SHA1Context * context, PCHAR message_array, UCHAR length);
void SHA1Result(SHA1Context * context, PCHAR Message_Digest);

void memxor(PCHAR dst, PCHAR src, UCHAR len);
void hmac_sha1(PCHAR key, UCHAR keylen, PCHAR in, UCHAR inlen, PCHAR resbuf);
void pbkdf2_sha1(PCHAR P, PCHAR S, USHORT c, PCHAR DK, UCHAR dkLen);