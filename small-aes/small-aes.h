#pragma once
#include <string.h> 
#include <iostream>
#include <stdio.h>

#define Nb 4        //列数
#define Nk 4        // key中32bit字的数量
#define Nr 4       // 轮数

#define AES_KEYLEN 16   // 密钥长度(Byte)
#define AES_keyExpSize 80

// 状态值
typedef uint8_t state_t[4][4];

// const防止被修改
static const uint8_t sbox[256] = {
    //0   1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x6, 0xB, 0x5, 0x4, 0x2, 0xE, 0x7, 0xA, 0x9, 0xD, 0xF, 0xC, 0x3, 0x1, 0x0, 0x8 };

static const uint8_t rsbox[256] = {
    0xE, 0xD, 0x4, 0xC, 0x3, 0x2, 0x0, 0x6, 0xF, 0x8, 0x7, 0x1, 0xB, 0x9, 0x5, 0xA };

static const uint8_t Rcon[8] = {
  0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x06, 0xC};


#define getSBoxValue(num) (sbox[(num)])

void test_encrypt_ecb(state_t& in);
void test_decrypt_ecb(state_t& in);
void print(state_t m);

// 密钥扩展 
static void KeyExpansion(uint8_t* RoundKey, const uint8_t* Key){
    unsigned i, j, k;
    uint8_t tempa[4]; 

    // 第一轮是原始密钥
    for (i = 0; i < Nk; ++i){
        RoundKey[(i * 4) + 0] = Key[(i * 4) + 0];
        RoundKey[(i * 4) + 1] = Key[(i * 4) + 1];
        RoundKey[(i * 4) + 2] = Key[(i * 4) + 2];
        RoundKey[(i * 4) + 3] = Key[(i * 4) + 3];
    }

    // 其他轮密钥
    for (i = Nk; i < Nb * (Nr + 1); ++i){
        k = (i - 1) * 4;
        tempa[0] = RoundKey[k + 0];
        tempa[1] = RoundKey[k + 1];
        tempa[2] = RoundKey[k + 2];
        tempa[3] = RoundKey[k + 3];

        if (i % Nk == 0){
            const uint8_t u8tmp = tempa[0];
            tempa[0] = tempa[1];
            tempa[1] = tempa[2];
            tempa[2] = tempa[3];
            tempa[3] = u8tmp;

            tempa[0] = getSBoxValue(tempa[0]);
            tempa[1] = getSBoxValue(tempa[1]);
            tempa[2] = getSBoxValue(tempa[2]);
            tempa[3] = getSBoxValue(tempa[3]);

            tempa[0] = tempa[0] ^ Rcon[i / Nk];
        }

        j = i * 4; k = (i - Nk) * 4;
        RoundKey[j + 0] = RoundKey[k + 0] ^ tempa[0];
        RoundKey[j + 1] = RoundKey[k + 1] ^ tempa[1];
        RoundKey[j + 2] = RoundKey[k + 2] ^ tempa[2];
        RoundKey[j + 3] = RoundKey[k + 3] ^ tempa[3];
    }
}

static void AddRoundKey(uint8_t round, state_t& state, const uint8_t* RoundKey){
    uint8_t i, j;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            state[i][j] ^= RoundKey[(round * Nb * 4) + (i * Nb) + j];
}

static void SubBytes(state_t& state){
    uint8_t i, j;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            state[j][i] = getSBoxValue(state[j][i]);
}

static void ShiftRows(state_t& state){
    uint8_t temp;
    temp = state[0][1];
    state[0][1] = state[1][1];
    state[1][1] = state[2][1];
    state[2][1] = state[3][1];
    state[3][1] = temp;

    temp = state[0][2];
    state[0][2] = state[2][2];
    state[2][2] = temp;
    temp = state[1][2];
    state[1][2] = state[3][2];
    state[3][2] = temp;

    temp = state[0][3];
    state[0][3] = state[3][3];
    state[3][3] = state[2][3];
    state[2][3] = state[1][3];
    state[1][3] = temp;
}

static uint8_t xtime(uint8_t x){
    return ((x << 1) ^ (((x >> 3) & 1) * 0x13));
}

static void MixColumns(state_t& state){
    uint8_t i;
    uint8_t Tmp, Tm, t;
    for (i = 0; i < 4; ++i){
        t = state[i][0];
        Tmp = state[i][0] ^ state[i][1] ^ state[i][2] ^ state[i][3];
        Tm = state[i][0] ^ state[i][1]; Tm = xtime(Tm);  state[i][0] ^= Tm ^ Tmp;
        Tm = state[i][1] ^ state[i][2]; Tm = xtime(Tm);  state[i][1] ^= Tm ^ Tmp;
        Tm = state[i][2] ^ state[i][3]; Tm = xtime(Tm);  state[i][2] ^= Tm ^ Tmp;
        Tm = state[i][3] ^ t;              Tm = xtime(Tm);  state[i][3] ^= Tm ^ Tmp;
    }
}

// GF(2^8)上的乘法
#define Multiply(x, y)                                \
      (  ((y & 1) * x) ^                              \
      ((y>>1 & 1) * xtime(x)) ^                       \
      ((y>>2 & 1) * xtime(xtime(x))) ^                \
      ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^         \
      ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))   \


#define getSBoxInvert(num) (rsbox[(num)])

static void InvMixColumns(state_t& state){
    int i;
    uint8_t a, b, c, d;
    for (i = 0; i < 4; ++i){
        a = state[i][0];
        b = state[i][1];
        c = state[i][2];
        d = state[i][3];

        state[i][0] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
        state[i][1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
        state[i][2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
        state[i][3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
    }
}
static void InvSubBytes(state_t& state){
    uint8_t i, j;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            state[j][i] = getSBoxInvert(state[j][i]);
}
static void InvShiftRows(state_t& state){
    uint8_t temp;

    // Rotate first row 1 columns to right  
    temp = state[3][1];
    state[3][1] = state[2][1];
    state[2][1] = state[1][1];
    state[1][1] = state[0][1];
    state[0][1] = temp;

    // Rotate second row 2 columns to right 
    temp = state[0][2];
    state[0][2] = state[2][2];
    state[2][2] = temp;

    temp = state[1][2];
    state[1][2] = state[3][2];
    state[3][2] = temp;

    // Rotate third row 3 columns to right
    temp = state[0][3];
    state[0][3] = state[1][3];
    state[1][3] = state[2][3];
    state[2][3] = state[3][3];
    state[3][3] = temp;
}


static void Cipher(state_t& state, const uint8_t* RoundKey){
    uint8_t round = 0;

    AddRoundKey(0, state, RoundKey);

    for (round = 1; ; ++round){
        SubBytes(state);
        ShiftRows(state);
        if (round == Nr)
            break;
        MixColumns(state);
        AddRoundKey(round, state, RoundKey);
    }

    AddRoundKey(Nr, state, RoundKey);
}

static void InvCipher(state_t& state, const uint8_t* RoundKey){
    uint8_t round = 0;

    AddRoundKey(Nr, state, RoundKey);

    for (round = (Nr - 1); ; --round){
        InvShiftRows(state);
        InvSubBytes(state);
        AddRoundKey(round, state, RoundKey);
        if (round == 0)
            break;
        InvMixColumns(state);
    }
}

