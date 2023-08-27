#include <stdio.h>
#include<iostream>
#include <string.h>
#include <stdint.h>

#include "small-aes.h"

using namespace std;

int main(void){

    uint8_t in[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf }; //输入明文

    state_t m={0};
    for (int i = 0; i < 16; i++)
        m[i / 4][i % 4] = in[i];

    uint8_t key[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };  //密钥
    uint8_t RoundKey[AES_keyExpSize];
    KeyExpansion(RoundKey, key);

    Cipher(m, RoundKey);          //加密
    cout << "加密的密文为：";
    print(m);

    InvCipher(m, RoundKey);       //解密
    cout << "解密的明文为：";
    print(m);

    for (int i = 0; i < 16; i++)  //判断密文解密后是否仍然等于明文
        if (in[i] != m[i/4][i%4]) {
            cout << "failure!\n";
            return 0;
        }
    cout << "success!\n";

    return 0;
}


void test_encrypt_ecb(state_t& in){
    uint8_t key[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };

    uint8_t RoundKey[AES_keyExpSize];

    KeyExpansion(RoundKey, key);
    Cipher(in, RoundKey);

    printf("ECB encrypt: ");
    
    /*输出密文
    for (int i = 0; i < 4; i++) {
        cout << endl;
        for (int j = 0; j < 4; j++)
            cout<<hex<<int(in[j*4+i])<<" ";
    }*/

}

void test_decrypt_ecb(state_t& in){

    uint8_t key[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };

    uint8_t RoundKey[AES_keyExpSize];

    KeyExpansion(RoundKey, key);
    InvCipher(in, RoundKey);
    
}

void print(state_t m) {
    for (int i = 0; i < 4; i++) {
        cout << endl;
        for (int j = 0; j < 4; j++)
            cout << hex << int(m[j][i]) << " ";
    }
    cout << "\n\n";
}