// public static void CalulateTable_CRC8()
// {
//     const byte generator = 0x1D;
//     crctable = new byte[256];
//     /* iterate over all byte values 0 - 255 */
//     for (int divident = 0; divident < 256; divident++)
//     {
//         byte currByte = (byte)divident;
//         /* calculate the CRC-8 value for current byte */
//         for (byte bit = 0; bit < 8; bit++)
//         {
//             if ((currByte & 0x80) != 0)
//             {
//                 currByte <<= 1;
//                 currByte ^= generator;
//             }
//             else
//             {
//                 currByte <<= 1;
//             }
//         }
//         /* store CRC value in lookup table */
//         crctable[divident] = currByte;
//     }
// } 



// public static byte Compute_CRC8(byte[] bytes)
// {
//     byte crc = 0;
//     foreach (byte b in bytes)
//     {
//         /* XOR-in next input byte */
//         byte data = (byte)(b ^ crc);
//         /* get current CRC value = remainder */
//         crc = (byte)(crctable[data]);
//     }

//     return crc;
// } 

#include <stdio.h>
#include <string.h>
#include <linux/types.h>

unsigned char tabela[256];


void calcula_tabela_crc(char *table){

    __int8_t base = 0x7;
    __int8_t byteAtual;
    int max = 256;
    for(int i = 0 ; i < max ;i++){
        byteAtual = i;
        for (int bit = 0; bit < 8; bit++)
        {            
            if ((byteAtual & 0x80) != 0)
            {        
                byteAtual <<= 1;
                byteAtual ^= base;
            }
            else
            {                
                byteAtual <<= 1;
            }
        }
        table[i] = byteAtual;
    }
}

char calcula_crc(__int8_t *dados,int tamanho){

    __uint8_t crc = 0;

    for(int i = 0 ; i < tamanho; i++){
        __uint8_t data = (dados[i] ^ crc);
        crc = tabela[data];
    }
    return crc;
}
