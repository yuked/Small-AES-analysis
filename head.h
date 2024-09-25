#pragma once
#include <string.h> 
#include <iostream>
#include <stdio.h>;

using namespace std;

#define Nb 4        //列数
#define Nk 4        // key中32bit字的数量
#define Nr 4       // 轮数

#define AES_KEYLEN 16   // Key length in bytes
#define AES_keyExpSize 80

// state - array holding the intermediate results during decryption.
typedef uint8_t state_t[4][4];

// const防止s盒查找表被修改
static const uint8_t sbox[16] = {
    //0   1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x6, 0xB, 0x5, 0x4, 0x2, 0xE, 0x7, 0xA, 0x9, 0xD, 0xF, 0xC, 0x3, 0x1, 0x0, 0x8 };

static const uint8_t rsbox[16] = {
    0xE, 0xD, 0x4, 0xC, 0x3, 0x2, 0x0, 0x6, 0xF, 0x8, 0x7, 0x1, 0xB, 0x9, 0x5, 0xA };

static const uint8_t Rcon[8] = {
  0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x06, 0xC };

static const int s_xor[16][16] = {
    {16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,2,2,2,2,0,0,0,2,0,0,0,2,4,0,0},
    {0,2,0,4,2,2,2,0,0,2,0,0,0,0,0,2},
    {0,2,4,0,0,2,0,0,2,2,0,2,0,0,2,0},
    {0,0,2,0,4,2,0,0,0,0,2,0,2,0,2,2},
    {0,0,0,2,0,0,0,2,4,2,0,0,2,0,2,2},
    {0,4,0,0,0,2,0,2,0,2,2,0,2,2,0,0},
    {0,2,0,0,0,0,2,0,0,0,0,2,4,2,2,2},
    {0,2,2,0,0,0,2,2,2,0,2,0,0,0,0,4},
    {0,0,2,2,0,0,0,0,0,2,2,4,0,2,0,2},
    {0,0,2,0,2,0,2,2,0,4,0,2,2,0,0,0},
    {0,0,0,0,2,0,2,0,2,2,4,0,0,2,2,0},
    {0,0,0,2,0,4,2,0,2,0,2,2,2,0,0,0},
    {0,0,0,0,2,2,0,4,2,0,0,2,0,2,0,2},
    {0,0,2,2,0,2,4,2,0,0,0,0,0,2,2,0},
    {0,2,0,2,2,0,0,2,0,0,2,2,0,0,4,0}
};

#define getSBoxValue(num) (sbox[(num)])
void test(uint8_t w0);

#define xtime(x)    ((x << 1) ^ (((x >> 3) & 1) * 0x13))

#define Multiply(x, y)                                \
      (  ((y & 1) * x) ^                              \
      ((y>>1 & 1) * xtime(x)) ^                       \
      ((y>>2 & 1) * xtime(xtime(x))) ^                \
      ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^         \
      ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))   \


#define getSBoxInvert(num) (rsbox[(num)])




/*原始测x3
void test(uint8_t w4) {
    uint8_t x2_0, x2_1, x2_2, x2_3;     //x2[0,1,2,3]
    uint8_t z1;                         //Δw4_0
    uint8_t z4_0, z4_1, z4_2, z4_3;     //z4[0,1,2,3]

    uint8_t t1, t2, t3, t4;
    t1 = Multiply(w4, 0x0e);
    t2 = Multiply(w4, 0x09);
    t3 = Multiply(w4, 0x0d);
    t4 = Multiply(w4, 0x0b);
    for (z4_0 = 0; z4_0 < 16; z4_0++) {
        uint8_t s1 = rsbox[t1 ^ z4_0] ^ rsbox[z4_0];      //s1,s2,s3,s4表示ΔX4的[0,5,10,15]的值
        for (z4_1 = 0; z4_1 < 16; z4_1++) {
            uint8_t s2 = rsbox[t2 ^ z4_1] ^ rsbox[z4_1];
            for (z4_2 = 0; z4_2 < 16; z4_2++) {
                uint8_t s3 = rsbox[t3 ^ z4_2] ^ rsbox[z4_2];
                for (z4_3 = 0; z4_3 < 16; z4_3++) {
                    uint8_t s4 = rsbox[t4 ^ z4_3] ^ rsbox[z4_3];

                    uint8_t p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15;    //状态矩阵

                    //ΔY3 <—逆SR— ΔW3 <—逆MC— ΔX4
                    p0 = Multiply(s4, 0x09); p4 = Multiply(s1, 0x0e); p8 = Multiply(s2, 0x0b); p12 = Multiply(s3, 0x0d);
                    p5 = Multiply(s4, 0x0d); p9 = Multiply(s1, 0x09); p13 = Multiply(s2, 0x0e); p1 = Multiply(s3, 0x0b);
                    p10 = Multiply(s4, 0x0b); p14 = Multiply(s1, 0x0d); p2 = Multiply(s2, 0x09); p6 = Multiply(s3, 0x0e);
                    p15 = Multiply(s4, 0x0e); p3 = Multiply(s1, 0x0b); p7 = Multiply(s2, 0x0d); p11 = Multiply(s3, 0x09);



                    uint8_t c1, c2;
                    uint8_t s5, s6, s7, s8;
                    for (z1 = 1; z1 < 16; z1++) {   //Δz1_0
                        c1 = xtime(z1);            //Z1在第一行，不需要该 2 1 1 3
                        c2 = c1 ^ z1;
                        for (x2_0 = 0; x2_0 < 16; x2_0++) {
                            s5 = sbox[c1 ^ x2_0] ^ sbox[x2_0];   //s5,s6,s7,s8为Δy2[0,1,2,3]
                            for (x2_1 = 0; x2_1 < 16; x2_1++) {
                                s6 = sbox[z1 ^ x2_1] ^ sbox[x2_1];
                                for (x2_2 = 0; x2_2 < 16; x2_2++) {
                                    s7 = sbox[z1 ^ x2_2] ^ sbox[x2_2];
                                    for (x2_3 = 0; x2_3 < 16; x2_3++) {
                                        s8 = sbox[c2 ^ x2_3] ^ sbox[x2_3];   //得到ΔY2，s5,s6,s7,s8为非零nibble

                                        uint8_t  m1, m2, m4, m5, m8, m11, m14, m15;     //状态矩阵

                                        //m0 = m3 = s7;     //ΔY2 —SR —MC—> ΔX3
                                        m2 = xtime(s7);   //只需要考虑Z2处在MC的每一列即可
                                        m1 = m2 ^ s7;

                                        //m6 = m7 = s6;
                                        m5 = xtime(s6);
                                        m4 = m5 ^ s6;

                                        //m9 = m10 = s5;
                                        m8 = xtime(s5);
                                        m11 = m8 ^ s5;

                                        //m12 = m13 = s8;
                                        m15 = xtime(s8);
                                        m14 = m15 ^ s8;


                                        if (s_xor[s7][p0] != 0)   //找ΔX3-ΔY3
                                            if (s_xor[m1][p1] != 0)
                                                if (s_xor[m2][p2] != 0)
                                                    if (s_xor[s7][p3] != 0)
                                                        if (s_xor[m4][p4] != 0)
                                                            if (s_xor[m5][p5] != 0)
                                                                if (s_xor[s6][p6] != 0)
                                                                    if (s_xor[s6][p7] != 0)
                                                                        if (s_xor[m8][p8] != 0)
                                                                            if (s_xor[s5][p9] != 0)
                                                                                if (s_xor[s5][p10] != 0)
                                                                                    if (s_xor[m11][p11] != 0)
                                                                                        if (s_xor[s8][p12] != 0)
                                                                                            if (s_xor[s8][p13] != 0)
                                                                                                if (s_xor[m14][p14] != 0)
                                                                                                    if (s_xor[m15][p15] != 0) {


                                                                                                        for (int x0 = 0; x0 < 16; x0++)     //x0
                                                                                                            if ((sbox[x0] ^ sbox[x0 ^ s7]) == p0) {
                                                                                                                for (int x1 = 0; x1 < 16; x1++)     //x1
                                                                                                                    if ((sbox[x1] ^ sbox[x1 ^ m1]) == p1) {
                                                                                                                        for (int x2 = 0; x2 < 16; x2++)     //x2
                                                                                                                            if ((sbox[x2] ^ sbox[x2 ^ m2]) == p2) {
                                                                                                                                for (int x3 = 0; x3 < 16; x3++)     //x3
                                                                                                                                    if ((sbox[x3] ^ sbox[x3 ^ s7]) == p3) {
                                                                                                                                        for (int x4 = 0; x4 < 16; x4++)     //x4
                                                                                                                                            if ((sbox[x4] ^ sbox[x4 ^ m4]) == p4) {
                                                                                                                                                for (int x5 = 0; x5 < 16; x5++)     //x5
                                                                                                                                                    if ((sbox[x5] ^ sbox[x5 ^ m5]) == p5) {
                                                                                                                                                        for (int x6 = 0; x6 < 16; x6++)     //x6
                                                                                                                                                            if ((sbox[x6] ^ sbox[x6 ^ s6]) == p6) {
                                                                                                                                                                for (int x7 = 0; x7 < 16; x7++)     //x7
                                                                                                                                                                    if ((sbox[x7] ^ sbox[x7 ^ s6]) == p7) {
                                                                                                                                                                        for (int x8 = 0; x8 < 16; x8++)     //x8
                                                                                                                                                                            if ((sbox[x8] ^ sbox[x8 ^ m8]) == p8) {
                                                                                                                                                                                for (int x9 = 0; x9 < 16; x9++)     //x9
                                                                                                                                                                                    if ((sbox[x9] ^ sbox[x9 ^ s5]) == p9) {
                                                                                                                                                                                        for (int x10 = 0; x10 < 16; x10++)     //x10
                                                                                                                                                                                            if ((sbox[x10] ^ sbox[x10 ^ s5]) == p10) {
                                                                                                                                                                                                for (int x11 = 0; x11 < 16; x11++)     //x11
                                                                                                                                                                                                    if ((sbox[x11] ^ sbox[x11 ^ m11]) == p11) {
                                                                                                                                                                                                        for (int x12 = 0; x12 < 16; x12++)     //x12
                                                                                                                                                                                                            if ((sbox[x12] ^ sbox[x12 ^ s8]) == p12) {
                                                                                                                                                                                                                for (int x13 = 0; x13 < 16; x13++)     //x13
                                                                                                                                                                                                                    if ((sbox[x13] ^ sbox[x13 ^ s8]) == p13) {
                                                                                                                                                                                                                        for (int x14 = 0; x14 < 16; x14++)     //x14
                                                                                                                                                                                                                            if ((sbox[x14] ^ sbox[x14 ^ m14]) == p14) {
                                                                                                                                                                                                                                for (int x15 = 0; x15 < 16; x15++)     //x15
                                                                                                                                                                                                                                    if ((sbox[x15] ^ sbox[x15 ^ m15]) == p15) {

                                                                                                                                                                                                                                        counter[w4 - 1]++;

                                                                                                                                                                                                                                    }
                                                                                                                                                                                                                            }
                                                                                                                                                                                                                    }
                                                                                                                                                                                                            }
                                                                                                                                                                                                    }
                                                                                                                                                                                            }
                                                                                                                                                                                    }
                                                                                                                                                                            }
                                                                                                                                                                    }
                                                                                                                                                            }
                                                                                                                                                    }
                                                                                                                                            }
                                                                                                                                    }
                                                                                                                            }


                                                                                                                    }
                                                                                                            }
                                                                                                    }
                                    }//x2_3
                                }
                            }
                        }
                    }//Δz0
                }//z4_3
            }
        }
    }
}*/




