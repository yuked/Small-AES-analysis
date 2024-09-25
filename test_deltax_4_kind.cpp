#include <stdio.h>
#include<iostream>
#include <fstream>
#include <string.h>
#include <stdint.h>
#include<thread>
#include<mutex>

#include "head.h"

using namespace std;
typedef unsigned __int64 uint64_t;


uint64_t counter[15] = { 0 };      //计数碰撞的差分对数


int main(void) {
    
    clock_t begin, end;
    begin = clock();

    thread t[15];
    for (int i = 0; i < 15; i = i++)   //15线程
        t[i] = thread(test, i + 1);

    for (int i = 0; i < 15; i++)
        t[i].join();

    uint64_t x = 0;
    for (int i = 0; i < 15; i++)
        x += counter[i];
    cout << "\n" << "counter= " << x << endl;

    end = clock();
    cout << (double)(end - begin) / CLOCKS_PER_SEC;
}



/*头0，尾0*/

void test(uint8_t w4) {                 //为了实现多线程，将ΔW4[0]作为参数传入，记为变量W4
    uint8_t x2_0, x2_1, x2_2, x2_3;     //存储 x2[0,1,2,3] 的值
    uint8_t z1_0;                       //存储 ΔZ1[0] 的值
    uint8_t z4_0, z4_1, z4_2, z4_3;     //存储 z4[0,1,2,3] 的值

    //从尾部开始向中间
    uint8_t t1, t2, t3, t4;        //临时变量，因为实际只有逆列混淆矩阵的某一列与ΔW4[0]进行一次计算，得到ΔZ4[0,1,2,3]的取值，其他均为0
    //ΔW4的非零nibble所处行数不同，则关于t1-t4的计算不同，共四种
    t1 = Multiply(w4, 0xb);        //t1为ΔZ4[0]
    t2 = Multiply(w4, 0xe);        //t2为ΔZ4[1]
    t3 = Multiply(w4, 0x9);        //t3为ΔZ4[2]
    t4 = Multiply(w4, 0xd);        //t4为ΔZ4[3]
    for (z4_0 = 0; z4_0 < 16; z4_0++) {                         
        uint8_t s1 = rsbox[t1 ^ z4_0] ^ rsbox[z4_0];            //s1=ΔX4[0] 
        for (z4_1 = 0; z4_1 < 16; z4_1++) {
            uint8_t s2 = rsbox[t2 ^ z4_1] ^ rsbox[z4_1];        //s2=ΔX4[5]
            for (z4_2 = 0; z4_2 < 16; z4_2++) {
                uint8_t s3 = rsbox[t3 ^ z4_2] ^ rsbox[z4_2];    //s3=ΔX4[10]
                for (z4_3 = 0; z4_3 < 16; z4_3++) {
                    uint8_t s4 = rsbox[t4 ^ z4_3] ^ rsbox[z4_3];//s4=ΔX4[15]

                    uint8_t p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15;    //ΔY3的状态矩阵

                    //ΔX4—逆MC、逆SR—>ΔY3
                    //ΔW4的非零nibble所处列数不同，则关于p0-p15的计算不同，共四种
                    p0 = Multiply(s4, 0x09); p4 = Multiply(s1, 0x0e); p8 = Multiply(s2, 0x0b); p12 = Multiply(s3, 0x0d); //1
                    p5 = Multiply(s4, 0x0d); p9 = Multiply(s1, 0x09); p13 = Multiply(s2, 0x0e); p1 = Multiply(s3, 0x0b);
                    p10 = Multiply(s4, 0x0b); p14 = Multiply(s1, 0x0d); p2 = Multiply(s2, 0x09); p6 = Multiply(s3, 0x0e);
                    p15 = Multiply(s4, 0x0e); p3 = Multiply(s1, 0x0b); p7 = Multiply(s2, 0x0d); p11 = Multiply(s3, 0x09);

                    //从头部开始向中间
                    uint8_t c1, c2;             //临时变量
                    for (z1_0 = 1; z1_0 < 16; z1_0++) {   
                        c1 = xtime(z1_0);         //c1为2*Z1[0]
                        c2 = c1 ^ z1_0;           //c2为3*Z1[0]
                        for (x2_0 = 0; x2_0 < 16; x2_0++) {                         //ΔZ1的非零nibble所处行数不同，则关于s5-s8的计算不同，共四种
                            uint8_t s5 = sbox[c2 ^ x2_0] ^ sbox[x2_0];              //s5为Δy2[0]
                            for (x2_1 = 0; x2_1 < 16; x2_1++) {
                                uint8_t s6 = sbox[c1 ^ x2_1] ^ sbox[x2_1];        //s6为Δy2[1]
                                for (x2_2 = 0; x2_2 < 16; x2_2++) {
                                    uint8_t s7 = sbox[z1_0 ^ x2_2] ^ sbox[x2_2];    //s7为Δy2[2]
                                    for (x2_3 = 0; x2_3 < 16; x2_3++) {
                                        uint8_t s8 = sbox[z1_0 ^ x2_3] ^ sbox[x2_3];  //s8为Δy2[3]
                                        uint8_t m0, m1, m4, m7, m10, m11, m13, m14;   //11111111111111

                                                                                    //m2 = m3 = s6;     //ΔY2 —SR —MC—> ΔX3
                                        m1 = xtime(s6);   //只需要考虑Z2处在MC的每一列即可
                                        m0 = m1 ^ s6;

                                        //m5 = m6 = s5;
                                        m4 = xtime(s5);
                                        m7 = m4 ^ s5;

                                        //m8 = m9 = s8;
                                        m11 = xtime(s8);
                                        m10 = m11 ^ s8;

                                        //m12 = m15 = s7;
                                        m14 = xtime(s7);
                                        m13 = m14 ^ s7;


                                        if (s_xor[m0][p0] != 0)   //找ΔX3-ΔY3
                                            if (s_xor[m1][p1] != 0)
                                                if (s_xor[s6][p2] != 0)
                                                    if (s_xor[s6][p3] != 0)
                                                        if (s_xor[m4][p4] != 0)
                                                            if (s_xor[s5][p5] != 0)
                                                                if (s_xor[s5][p6] != 0)
                                                                    if (s_xor[m7][p7] != 0)
                                                                        if (s_xor[s8][p8] != 0)
                                                                            if (s_xor[s8][p9] != 0)
                                                                                if (s_xor[m10][p10] != 0)
                                                                                    if (s_xor[m11][p11] != 0)
                                                                                        if (s_xor[s7][p12] != 0)
                                                                                            if (s_xor[m13][p13] != 0)
                                                                                                if (s_xor[m14][p14] != 0)
                                                                                                    if (s_xor[s7][p15] != 0) {
                                                                                                        uint64_t num;
                                                                                                        //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        num = (uint64_t)((uint64_t)s_xor[m0][p0] + (uint64_t)s_xor[m1][p1] + (uint64_t)s_xor[s6][p2] + (uint64_t)s_xor[s6][p3] + \
                                                                                                            (uint64_t)s_xor[m4][p4] + (uint64_t)s_xor[s5][p5] + (uint64_t)s_xor[s5][p6] + (uint64_t)s_xor[m7][p7] + \
                                                                                                            (uint64_t)s_xor[s8][p8] + (uint64_t)s_xor[s8][p9] + (uint64_t)s_xor[m10][p10] + (uint64_t)s_xor[m11][p11] + \
                                                                                                            (uint64_t)s_xor[s7][p12] + (uint64_t)s_xor[m13][p13] + (uint64_t)s_xor[m14][p14] + (uint64_t)s_xor[s7][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少
                                                                                                    }
                                    }//x2_3
                                }
                            }
                        }
                    }//Δz0
                }//z4_3
            }//z4_2
        }//z4_1
    }//z4_0
}//Δw4_0





/*头尾均是第2行
void test(uint8_t w4) {
    uint8_t x2_0, x2_1, x2_2, x2_3;     //x2[0,1,2,3]
    uint8_t z1_0;                         //Δw4_0  
    uint8_t z4_0, z4_1, z4_2, z4_3;     //z4[0,1,2,3]

    uint8_t t1, t2, t3, t4;        //第二行
    t1 = Multiply(w4, 0x0d);
    t2 = Multiply(w4, 0x0b);
    t3 = Multiply(w4, 0x0e);
    t4 = Multiply(w4, 0x09);
    for (z4_0 = 0; z4_0 < 16; z4_0++) {
        uint8_t s1 = rsbox[t1 ^ z4_0] ^ rsbox[z4_0];      //s1,s2,s3,s4表示ΔX4的[0,5,10,15]的值
        for (z4_1 = 0; z4_1 < 16; z4_1++) {
            uint8_t s2 = rsbox[t2 ^ z4_1] ^ rsbox[z4_1];
            for (z4_2 = 0; z4_2 < 16; z4_2++) {
                uint8_t s3 = rsbox[t3 ^ z4_2] ^ rsbox[z4_2];
                for (z4_3 = 0; z4_3 < 16; z4_3++) {
                    uint8_t s4 = rsbox[t4 ^ z4_3] ^ rsbox[z4_3];

                    uint8_t p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15;    //状态矩阵

                    //ΔX4—逆MC—逆SR—>ΔY3  此处逆SR位置已固定，按逆SR前的位置改即可
                    p0 = Multiply(s1, 0x0e); p4 = Multiply(s2, 0x0b); p8 = Multiply(s3, 0x0d); p12 = Multiply(s4, 0x09); //0
                    p5 = Multiply(s1, 0x09); p9 = Multiply(s2, 0x0e); p13 = Multiply(s3, 0x0b); p1 = Multiply(s4, 0x0d);
                    p10 = Multiply(s1, 0x0d); p14 = Multiply(s2, 0x09); p2 = Multiply(s3, 0x0e); p6 = Multiply(s4, 0x0b);
                    p15 = Multiply(s1, 0x0b); p3 = Multiply(s2, 0x0d); p7 = Multiply(s3, 0x09); p11 = Multiply(s4, 0x0e);

                    uint8_t c1, c2;
                    for (z1_0 = 1; z1_0 < 16; z1_0++) {   //第二行
                        c1 = xtime(z1_0);
                        c2 = c1 ^ z1_0;
                        for (x2_0 = 0; x2_0 < 16; x2_0++) {
                            uint8_t s5 = sbox[z1_0 ^ x2_0] ^ sbox[x2_0];   //s5,s6,s7,s8为Δy2[0,1,2,3]
                            for (x2_1 = 0; x2_1 < 16; x2_1++) {
                                uint8_t s6 = sbox[ c2^ x2_1] ^ sbox[x2_1];
                                for (x2_2 = 0; x2_2 < 16; x2_2++) {
                                    uint8_t s7 = sbox[c1 ^ x2_2] ^ sbox[x2_2];
                                    for (x2_3 = 0; x2_3 < 16; x2_3++) {
                                        uint8_t s8 = sbox[z1_0 ^ x2_3] ^ sbox[x2_3];

                                        uint8_t m0, m3, m6, m7, m9, m10, m12, m13;      //0000000000000000000
                                        
                                            //m1 = m2 = s5;      //ΔY2 —SR—> ΔZ2 —MC—> ΔX3
                                            m0 = xtime(s5);
                                            m3 = m0 ^ s5;

                                            //m4 = m5 = s8;
                                            m7 = xtime(s8);
                                            m6 = m7 ^ s8;

                                            //m8 = m11 = s7;
                                            m10 = xtime(s7);
                                            m9 = m10 ^ s7;

                                            //m14 = m15 = s6;
                                            m13 = xtime(s6);
                                            m12 = m13 ^ s6;
                                        

                                        if (s_xor[m0][p0] != 0)
                                            if (s_xor[s5][p1] != 0)
                                                if (s_xor[s5][p2] != 0)
                                                    if (s_xor[m3][p3] != 0)
                                                        if (s_xor[s8][p4] != 0)
                                                            if (s_xor[s8][p5] != 0)
                                                                if (s_xor[m6][p6] != 0)
                                                                    if (s_xor[m7][p7] != 0)
                                                                        if (s_xor[s7][p8] != 0)
                                                                            if (s_xor[m9][p9] != 0)
                                                                                if (s_xor[m10][p10] != 0)
                                                                                    if (s_xor[s7][p11] != 0)
                                                                                        if (s_xor[m12][p12] != 0)
                                                                                            if (s_xor[m13][p13] != 0)
                                                                                                if (s_xor[s6][p14] != 0)
                                                                                                    if (s_xor[s6][p15] != 0) {
                                                                                                          //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        uint64_t num;
                                                                                                        num = (uint64_t)((uint64_t)s_xor[m0][p0] + (uint64_t)s_xor[s5][p1] + (uint64_t)s_xor[s5][p2] + (uint64_t)s_xor[m3][p3] + \
                                                                                                            (uint64_t)s_xor[s8][p4] + (uint64_t)s_xor[s8][p5] + (uint64_t)s_xor[m6][p6] + (uint64_t)s_xor[m7][p7] + \
                                                                                                            (uint64_t)s_xor[s7][p8] + (uint64_t)s_xor[m9][p9] + (uint64_t)s_xor[m10][p10] + (uint64_t)s_xor[s7][p11] + \
                                                                                                            (uint64_t)s_xor[m12][p12] + (uint64_t)s_xor[m13][p13] + (uint64_t)s_xor[s6][p14] + (uint64_t)s_xor[s6][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少

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




//第二行——头
/*第一种 0——

                                        uint8_t m0, m3, m6, m7, m9, m10, m12, m13;      //0000000000000000000

                                            //m1 = m2 = s5;      //ΔY2 —SR—> ΔZ2 —MC—> ΔX3
                                            m0 = xtime(s5);
                                            m3 = m0 ^ s5;

                                            //m4 = m5 = s8;
                                            m7 = xtime(s8);
                                            m6 = m7 ^ s8;

                                            //m8 = m11 = s7;
                                            m10 = xtime(s7);
                                            m9 = m10 ^ s7;

                                            //m14 = m15 = s6;
                                            m13 = xtime(s6);
                                            m12 = m13 ^ s6;


                                        if (s_xor[m0][p0] != 0)
                                            if (s_xor[s5][p1] != 0)
                                                if (s_xor[s5][p2] != 0)
                                                    if (s_xor[m3][p3] != 0)
                                                        if (s_xor[s8][p4] != 0)
                                                            if (s_xor[s8][p5] != 0)
                                                                if (s_xor[m6][p6] != 0)
                                                                    if (s_xor[m7][p7] != 0)
                                                                        if (s_xor[s7][p8] != 0)
                                                                            if (s_xor[m9][p9] != 0)
                                                                                if (s_xor[m10][p10] != 0)
                                                                                    if (s_xor[s7][p11] != 0)
                                                                                        if (s_xor[m12][p12] != 0)
                                                                                            if (s_xor[m13][p13] != 0)
                                                                                                if (s_xor[s6][p14] != 0)
                                                                                                    if (s_xor[s6][p15] != 0) {
                                                                                                          //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        uint64_t num;
                                                                                                        num = (uint64_t)((uint64_t)s_xor[m0][p0] + (uint64_t)s_xor[s5][p1] + (uint64_t)s_xor[s5][p2] + (uint64_t)s_xor[m3][p3] + \
                                                                                                            (uint64_t)s_xor[s8][p4] + (uint64_t)s_xor[s8][p5] + (uint64_t)s_xor[m6][p6] + (uint64_t)s_xor[m7][p7] + \
                                                                                                            (uint64_t)s_xor[s7][p8] + (uint64_t)s_xor[m9][p9] + (uint64_t)s_xor[m10][p10] + (uint64_t)s_xor[s7][p11] + \
                                                                                                            (uint64_t)s_xor[m12][p12] + (uint64_t)s_xor[m13][p13] + (uint64_t)s_xor[s6][p14] + (uint64_t)s_xor[s6][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少

                                                                                                    }
                                    }//x2_3
                                }
                            }
                        }
                    }//Δz0
                }//z4_3


//第二种 1——

                                        uint8_t m0, m1, m4, m7, m10, m11, m13, m14;   //11111111111111

                                            //m2 = m3 = s6;     //ΔY2 —SR —MC—> ΔX3
                                            m1 = xtime(s6);   //只需要考虑Z2处在MC的每一列即可
                                            m0 = m1 ^ s6;

                                            //m5 = m6 = s5;
                                            m4 = xtime(s5);
                                            m7 = m4 ^ s5;

                                            //m8 = m9 = s8;
                                            m11 = xtime(s8);
                                            m10 = m11 ^ s8;

                                            //m12 = m15 = s7;
                                            m14 = xtime(s7);
                                            m13 = m14 ^ s7;


                                        if (s_xor[m0][p0] != 0)   //找ΔX3-ΔY3
                                            if (s_xor[m1][p1] != 0)
                                                if (s_xor[s6][p2] != 0)
                                                    if (s_xor[s6][p3] != 0)
                                                        if (s_xor[m4][p4] != 0)
                                                            if (s_xor[s5][p5] != 0)
                                                                if (s_xor[s5][p6] != 0)
                                                                    if (s_xor[m7][p7] != 0)
                                                                        if (s_xor[s8][p8] != 0)
                                                                            if (s_xor[s8][p9] != 0)
                                                                                if (s_xor[m10][p10] != 0)
                                                                                    if (s_xor[m11][p11] != 0)
                                                                                        if (s_xor[s7][p12] != 0)
                                                                                            if (s_xor[m13][p13] != 0)
                                                                                                if (s_xor[m14][p14] != 0)
                                                                                                    if (s_xor[s7][p15] != 0) {
                                                                                                        uint64_t num;
                                                                                                        //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        num = (uint64_t)((uint64_t)s_xor[m0][p0] + (uint64_t)s_xor[m1][p1] + (uint64_t)s_xor[s6][p2] + (uint64_t)s_xor[s6][p3] + \
                                                                                                            (uint64_t)s_xor[m4][p4] + (uint64_t)s_xor[s5][p5] + (uint64_t)s_xor[s5][p6] + (uint64_t)s_xor[m7][p7] + \
                                                                                                            (uint64_t)s_xor[s8][p8] + (uint64_t)s_xor[s8][p9] + (uint64_t)s_xor[m10][p10] + (uint64_t)s_xor[m11][p11] + \
                                                                                                            (uint64_t)s_xor[s7][p12] + (uint64_t)s_xor[m13][p13] + (uint64_t)s_xor[m14][p14] + (uint64_t)s_xor[s7][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少
                                                                                                    }
                                    }//x2_3
                                }
                            }
                        }
                    }//Δz0
                }//z4_3


//第三种 2——

                                        uint8_t  m1, m2, m4, m5, m8, m11, m14, m15;     //2222222222222222

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
                                                                                                           uint64_t num;
                                                                                                        //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        num = (uint64_t)((uint64_t)s_xor[s7][p0] + (uint64_t)s_xor[m1][p1] + (uint64_t)s_xor[m2][p2] + (uint64_t)s_xor[s7][p3] + \
                                                                                                            (uint64_t)s_xor[m4][p4] + (uint64_t)s_xor[m5][p5] + (uint64_t)s_xor[s6][p6] + (uint64_t)s_xor[s6][p7] + \
                                                                                                            (uint64_t)s_xor[m8][p8] + (uint64_t)s_xor[s5][p9] + (uint64_t)s_xor[s5][p10] + (uint64_t)s_xor[m11][p11] + \
                                                                                                            (uint64_t)s_xor[s8][p12] + (uint64_t)s_xor[s8][p13] + (uint64_t)s_xor[m14][p14] + (uint64_t)s_xor[m15][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少

                                                                                                    }
                                    }//x2_3
                                }
                            }
                        }
                    }//Δz0
                }//z4_3


//第四种 3 ——

                                        uint8_t  m2, m3, m5, m6, m8, m9, m12, m15;      //3333333333333333333

                                        //m0 = m1 = s8;     //ΔY2 —SR —MC—> ΔX3
                                        m3 = xtime(s8);   //只需要考虑Z2处在MC的每一列即可
                                        m2 = m3 ^ s8;

                                        //m4 = m7 = s7;
                                        m6 = xtime(s7);
                                        m5 = m6 ^ s7;

                                        //m10 = m11 = s6;
                                        m9 = xtime(s6);
                                        m8 = m9 ^ s6;

                                        //m13 = m14 = s5;
                                        m12 = xtime(s5);
                                        m15 = m12 ^ s5;


                                        if (s_xor[s8][p0] != 0)   //找ΔX3-ΔY3
                                            if (s_xor[s8][p1] != 0)
                                                if (s_xor[m2][p2] != 0)
                                                    if (s_xor[m3][p3] != 0)
                                                        if (s_xor[s7][p4] != 0)
                                                            if (s_xor[m5][p5] != 0)
                                                                if (s_xor[m6][p6] != 0)
                                                                    if (s_xor[s7][p7] != 0)
                                                                        if (s_xor[m8][p8] != 0)
                                                                            if (s_xor[m9][p9] != 0)
                                                                                if (s_xor[s6][p10] != 0)
                                                                                    if (s_xor[s6][p11] != 0)
                                                                                        if (s_xor[m12][p12] != 0)
                                                                                            if (s_xor[s5][p13] != 0)
                                                                                                if (s_xor[s5][p14] != 0)
                                                                                                    if (s_xor[m15][p15] != 0) {
                                                                                                        uint64_t num;
                                                                                                        //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        num = (uint64_t)((uint64_t)s_xor[s8][p0] + (uint64_t)s_xor[s8][p1] + (uint64_t)s_xor[m2][p2] + (uint64_t)s_xor[m3][p3] + \
                                                                                                            (uint64_t)s_xor[s7][p4] + (uint64_t)s_xor[m5][p5] + (uint64_t)s_xor[m6][p6] + (uint64_t)s_xor[s7][p7] + \
                                                                                                            (uint64_t)s_xor[m8][p8] + (uint64_t)s_xor[m9][p9] + (uint64_t)s_xor[s6][p10] + (uint64_t)s_xor[s6][p11] + \
                                                                                                            (uint64_t)s_xor[m12][p12] + (uint64_t)s_xor[s5][p13] + (uint64_t)s_xor[s5][p14] + (uint64_t)s_xor[m15][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少
                                                                                                    }
                                    }//x2_3
                                }
                            }
                        }
                    }//Δz0
                }//z4_3
*/













//第一行
/*1 - 1测x3
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

                    p0 = Multiply(s4, 0x09); p4 = Multiply(s1, 0x0e); p8 = Multiply(s2, 0x0b); p12 = Multiply(s3, 0x0d); //1
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

                                        uint8_t m0, m1, m4, m7, m10, m11, m13, m14;   //1

                                            //m2 = m3 = s6;     //ΔY2 —SR —MC—> ΔX3
                                        m1 = xtime(s6);   //只需要考虑Z2处在MC的每一列即可
                                        m0 = m1 ^ s6;

                                        //m5 = m6 = s5;
                                        m4 = xtime(s5);
                                        m7 = m4 ^ s5;

                                        //m8 = m9 = s8;
                                        m11 = xtime(s8);
                                        m10 = m11 ^ s8;

                                        //m12 = m15 = s7;
                                        m14 = xtime(s7);
                                        m13 = m14 ^ s7;


                                        if (s_xor[m0][p0] != 0)   //找ΔX3-ΔY3
                                            if (s_xor[m1][p1] != 0)
                                                if (s_xor[s6][p2] != 0)
                                                    if (s_xor[s6][p3] != 0)
                                                        if (s_xor[m4][p4] != 0)
                                                            if (s_xor[s5][p5] != 0)
                                                                if (s_xor[s5][p6] != 0)
                                                                    if (s_xor[m7][p7] != 0)
                                                                        if (s_xor[s8][p8] != 0)
                                                                            if (s_xor[s8][p9] != 0)
                                                                                if (s_xor[m10][p10] != 0)
                                                                                    if (s_xor[m11][p11] != 0)
                                                                                        if (s_xor[s7][p12] != 0)
                                                                                            if (s_xor[m13][p13] != 0)
                                                                                                if (s_xor[m14][p14] != 0)
                                                                                                    if (s_xor[s7][p15] != 0) {
                                                                                                        uint64_t num;
                                                                                                        //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        num = (uint64_t)((uint64_t)s_xor[m0][p0] + (uint64_t)s_xor[m1][p1] + (uint64_t)s_xor[s6][p2] + (uint64_t)s_xor[s6][p3] + \
                                                                                                            (uint64_t)s_xor[m4][p4] + (uint64_t)s_xor[s5][p5] + (uint64_t)s_xor[s5][p6] + (uint64_t)s_xor[m7][p7] + \
                                                                                                            (uint64_t)s_xor[s8][p8] + (uint64_t)s_xor[s8][p9] + (uint64_t)s_xor[m10][p10] + (uint64_t)s_xor[m11][p11] + \
                                                                                                            (uint64_t)s_xor[s7][p12] + (uint64_t)s_xor[m13][p13] + (uint64_t)s_xor[m14][p14] + (uint64_t)s_xor[s7][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少
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
}


//3-0 
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
                      p0 = Multiply(s1, 0x0e); p4 = Multiply(s2, 0x0b); p8 = Multiply(s3, 0x0d); p12 = Multiply(s4, 0x09); //0
                        p5 = Multiply(s1, 0x09); p9 = Multiply(s2, 0x0e); p13 = Multiply(s3, 0x0b); p1 = Multiply(s4, 0x0d);
                        p10 = Multiply(s1, 0x0d); p14 = Multiply(s2, 0x09); p2 = Multiply(s3, 0x0e); p6 = Multiply(s4, 0x0b);
                        p15 = Multiply(s1, 0x0b); p3 = Multiply(s2, 0x0d); p7 = Multiply(s3, 0x09); p11 = Multiply(s4, 0x0e);


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

                                        uint8_t  m2, m3, m5, m6, m8, m9, m12, m15;      //3

                                       //m0 = m1 = s8;     //ΔY2 —SR —MC—> ΔX3
                                        m3 = xtime(s8);   //只需要考虑Z2处在MC的每一列即可
                                        m2 = m3 ^ s8;

                                        //m4 = m7 = s7;
                                        m6 = xtime(s7);
                                        m5 = m6 ^ s7;

                                        //m10 = m11 = s6;
                                        m9 = xtime(s6);
                                        m8 = m9 ^ s6;

                                        //m13 = m14 = s5;
                                        m12 = xtime(s5);
                                        m15 = m12 ^ s5;


                                        if (s_xor[s8][p0] != 0)   //找ΔX3-ΔY3
                                            if (s_xor[s8][p1] != 0)
                                                if (s_xor[m2][p2] != 0)
                                                    if (s_xor[m3][p3] != 0)
                                                        if (s_xor[s7][p4] != 0)
                                                            if (s_xor[m5][p5] != 0)
                                                                if (s_xor[m6][p6] != 0)
                                                                    if (s_xor[s7][p7] != 0)
                                                                        if (s_xor[m8][p8] != 0)
                                                                            if (s_xor[m9][p9] != 0)
                                                                                if (s_xor[s6][p10] != 0)
                                                                                    if (s_xor[s6][p11] != 0)
                                                                                        if (s_xor[m12][p12] != 0)
                                                                                            if (s_xor[s5][p13] != 0)
                                                                                                if (s_xor[s5][p14] != 0)
                                                                                                    if (s_xor[m15][p15] != 0) {
                                                                                                        uint64_t num;
                                                                                                        //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        num = (uint64_t)((uint64_t)s_xor[s8][p0] + (uint64_t)s_xor[s8][p1] + (uint64_t)s_xor[m2][p2] + (uint64_t)s_xor[m3][p3] + \
                                                                                                            (uint64_t)s_xor[s7][p4] + (uint64_t)s_xor[m5][p5] + (uint64_t)s_xor[m6][p6] + (uint64_t)s_xor[s7][p7] + \
                                                                                                            (uint64_t)s_xor[m8][p8] + (uint64_t)s_xor[m9][p9] + (uint64_t)s_xor[s6][p10] + (uint64_t)s_xor[s6][p11] + \
                                                                                                            (uint64_t)s_xor[m12][p12] + (uint64_t)s_xor[s5][p13] + (uint64_t)s_xor[s5][p14] + (uint64_t)s_xor[m15][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少
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
}

//2-2
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
                    p0 = Multiply(s3, 0x0d); p4 = Multiply(s4, 0x09); p8 = Multiply(s1, 0x0e); p12 = Multiply(s2, 0x0b); //2
                    p5 = Multiply(s3, 0x0b); p9 = Multiply(s4, 0x0d); p13 = Multiply(s1, 0x09); p1 = Multiply(s2, 0x0e);
                    p10 = Multiply(s3, 0x0e); p14 = Multiply(s4, 0x0b); p2 = Multiply(s1, 0x0d); p6 = Multiply(s2, 0x09);
                    p15 = Multiply(s3, 0x09); p3 = Multiply(s4, 0x0e); p7 = Multiply(s1, 0x0b); p11 = Multiply(s2, 0x0d);


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
                                                                                                        uint64_t num;
                                                                                                        //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        num = (uint64_t)((uint64_t)s_xor[s7][p0] + (uint64_t)s_xor[m1][p1] + (uint64_t)s_xor[m2][p2] + (uint64_t)s_xor[s7][p3] + \
                                                                                                            (uint64_t)s_xor[m4][p4] + (uint64_t)s_xor[m5][p5] + (uint64_t)s_xor[s6][p6] + (uint64_t)s_xor[s6][p7] + \
                                                                                                            (uint64_t)s_xor[m8][p8] + (uint64_t)s_xor[s5][p9] + (uint64_t)s_xor[s5][p10] + (uint64_t)s_xor[m11][p11] + \
                                                                                                            (uint64_t)s_xor[s8][p12] + (uint64_t)s_xor[s8][p13] + (uint64_t)s_xor[m14][p14] + (uint64_t)s_xor[m15][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少

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


//第一行——头
/*第一种 0——
                                            uint8_t m0, m3, m6, m7, m9, m10, m12, m13;      //0000000000000000000

                                            //m1 = m2 = s5;      //ΔY2 —SR—> ΔZ2 —MC—> ΔX3
                                            m0 = xtime(s5);
                                            m3 = m0 ^ s5;

                                            //m4 = m5 = s8;
                                            m7 = xtime(s8);
                                            m6 = m7 ^ s8;

                                            //m8 = m11 = s7;
                                            m10 = xtime(s7);
                                            m9 = m10 ^ s7;

                                            //m14 = m15 = s6;
                                            m13 = xtime(s6);
                                            m12 = m13 ^ s6;


                                        if (s_xor[m0][p0] != 0)
                                            if (s_xor[s5][p1] != 0)
                                                if (s_xor[s5][p2] != 0)
                                                    if (s_xor[m3][p3] != 0)
                                                        if (s_xor[s8][p4] != 0)
                                                            if (s_xor[s8][p5] != 0)
                                                                if (s_xor[m6][p6] != 0)
                                                                    if (s_xor[m7][p7] != 0)
                                                                        if (s_xor[s7][p8] != 0)
                                                                            if (s_xor[m9][p9] != 0)
                                                                                if (s_xor[m10][p10] != 0)
                                                                                    if (s_xor[s7][p11] != 0)
                                                                                        if (s_xor[m12][p12] != 0)
                                                                                            if (s_xor[m13][p13] != 0)
                                                                                                if (s_xor[s6][p14] != 0)
                                                                                                    if (s_xor[s6][p15] != 0) {
                                                                                                          //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        uint64_t num;
                                                                                                        num = (uint64_t)((uint64_t)s_xor[m0][p0] + (uint64_t)s_xor[s5][p1] + (uint64_t)s_xor[s5][p2] + (uint64_t)s_xor[m3][p3] + \
                                                                                                            (uint64_t)s_xor[s8][p4] + (uint64_t)s_xor[s8][p5] + (uint64_t)s_xor[m6][p6] + (uint64_t)s_xor[m7][p7] + \
                                                                                                            (uint64_t)s_xor[s7][p8] + (uint64_t)s_xor[m9][p9] + (uint64_t)s_xor[m10][p10] + (uint64_t)s_xor[s7][p11] + \
                                                                                                            (uint64_t)s_xor[m12][p12] + (uint64_t)s_xor[m13][p13] + (uint64_t)s_xor[s6][p14] + (uint64_t)s_xor[s6][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少

                                                                                                    }
                                    }//x2_3
                                }
                            }
                        }
                    }//Δz0
                }//z4_3


//第二种 1——

                                        uint8_t m0, m1, m4, m7, m10, m11, m13, m14;   //11111111111111
                                        
                                            //m2 = m3 = s6;     //ΔY2 —SR —MC—> ΔX3
                                            m1 = xtime(s6);   //只需要考虑Z2处在MC的每一列即可
                                            m0 = m1 ^ s6;

                                            //m5 = m6 = s5;
                                            m4 = xtime(s5);
                                            m7 = m4 ^ s5;

                                            //m8 = m9 = s8;
                                            m11 = xtime(s8);
                                            m10 = m11 ^ s8;

                                            //m12 = m15 = s7;
                                            m14 = xtime(s7);
                                            m13 = m14 ^ s7;
                                        

                                        if (s_xor[m0][p0] != 0)   //找ΔX3-ΔY3
                                            if (s_xor[m1][p1] != 0)
                                                if (s_xor[s6][p2] != 0)
                                                    if (s_xor[s6][p3] != 0)
                                                        if (s_xor[m4][p4] != 0)
                                                            if (s_xor[s5][p5] != 0)
                                                                if (s_xor[s5][p6] != 0)
                                                                    if (s_xor[m7][p7] != 0)
                                                                        if (s_xor[s8][p8] != 0)
                                                                            if (s_xor[s8][p9] != 0)
                                                                                if (s_xor[m10][p10] != 0)
                                                                                    if (s_xor[m11][p11] != 0)
                                                                                        if (s_xor[s7][p12] != 0)
                                                                                            if (s_xor[m13][p13] != 0)
                                                                                                if (s_xor[m14][p14] != 0)
                                                                                                    if (s_xor[s7][p15] != 0) {
                                                                                                        uint64_t num;
                                                                                                        //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        num = (uint64_t)((uint64_t)s_xor[m0][p0] + (uint64_t)s_xor[m1][p1] + (uint64_t)s_xor[s6][p2] + (uint64_t)s_xor[s6][p3] + \
                                                                                                            (uint64_t)s_xor[m4][p4] + (uint64_t)s_xor[s5][p5] + (uint64_t)s_xor[s5][p6] + (uint64_t)s_xor[m7][p7] + \
                                                                                                            (uint64_t)s_xor[s8][p8] + (uint64_t)s_xor[s8][p9] + (uint64_t)s_xor[m10][p10] + (uint64_t)s_xor[m11][p11] + \
                                                                                                            (uint64_t)s_xor[s7][p12] + (uint64_t)s_xor[m13][p13] + (uint64_t)s_xor[m14][p14] + (uint64_t)s_xor[s7][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少
                                                                                                    }
                                    }//x2_3
                                }
                            }
                        }
                    }//Δz0
                }//z4_3


//第三种 2——

                                        uint8_t  m1, m2, m4, m5, m8, m11, m14, m15;     //2222222222222222
                                        
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
                                                                                                           uint64_t num;
                                                                                                        //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        num = (uint64_t)((uint64_t)s_xor[s7][p0] + (uint64_t)s_xor[m1][p1] + (uint64_t)s_xor[m2][p2] + (uint64_t)s_xor[s7][p3] + \
                                                                                                            (uint64_t)s_xor[m4][p4] + (uint64_t)s_xor[m5][p5] + (uint64_t)s_xor[s6][p6] + (uint64_t)s_xor[s6][p7] + \
                                                                                                            (uint64_t)s_xor[m8][p8] + (uint64_t)s_xor[s5][p9] + (uint64_t)s_xor[s5][p10] + (uint64_t)s_xor[m11][p11] + \
                                                                                                            (uint64_t)s_xor[s8][p12] + (uint64_t)s_xor[s8][p13] + (uint64_t)s_xor[m14][p14] + (uint64_t)s_xor[m15][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少

                                                                                                    }
                                    }//x2_3
                                }
                            }
                        }
                    }//Δz0
                }//z4_3


//第四种 3 ——

                                        uint8_t  m2, m3, m5, m6, m8, m9, m12, m15;      //3333333333333333333
                                        
                                        //m0 = m1 = s8;     //ΔY2 —SR —MC—> ΔX3
                                        m3 = xtime(s8);   //只需要考虑Z2处在MC的每一列即可
                                        m2 = m3 ^ s8;

                                        //m4 = m7 = s7;
                                        m6 = xtime(s7);
                                        m5 = m6 ^ s7;

                                        //m10 = m11 = s6;
                                        m9 = xtime(s6);
                                        m8 = m9 ^ s6;

                                        //m13 = m14 = s5;
                                        m12 = xtime(s5);
                                        m15 = m12 ^ s5;
                                        

                                        if (s_xor[s8][p0] != 0)   //找ΔX3-ΔY3
                                            if (s_xor[s8][p1] != 0)
                                                if (s_xor[m2][p2] != 0)
                                                    if (s_xor[m3][p3] != 0)
                                                        if (s_xor[s7][p4] != 0)
                                                            if (s_xor[m5][p5] != 0)
                                                                if (s_xor[m6][p6] != 0)
                                                                    if (s_xor[s7][p7] != 0)
                                                                        if (s_xor[m8][p8] != 0)
                                                                            if (s_xor[m9][p9] != 0)
                                                                                if (s_xor[s6][p10] != 0)
                                                                                    if (s_xor[s6][p11] != 0)
                                                                                        if (s_xor[m12][p12] != 0)
                                                                                            if (s_xor[s5][p13] != 0)
                                                                                                if (s_xor[s5][p14] != 0)
                                                                                                    if (s_xor[m15][p15] != 0) {
                                                                                                        uint64_t num;
                                                                                                        //防止下面子表达式溢出 最外的(uint64_t)和第一个计算数的(uint64_t)不能少
                                                                                                        num = (uint64_t)((uint64_t)s_xor[s8][p0] + (uint64_t)s_xor[s8][p1] + (uint64_t)s_xor[m2][p2] + (uint64_t)s_xor[m3][p3] + \
                                                                                                            (uint64_t)s_xor[s7][p4] + (uint64_t)s_xor[m5][p5] + (uint64_t)s_xor[m6][p6] + (uint64_t)s_xor[s7][p7] + \
                                                                                                            (uint64_t)s_xor[m8][p8] + (uint64_t)s_xor[m9][p9] + (uint64_t)s_xor[s6][p10] + (uint64_t)s_xor[s6][p11] + \
                                                                                                            (uint64_t)s_xor[m12][p12] + (uint64_t)s_xor[s5][p13] + (uint64_t)s_xor[s5][p14] + (uint64_t)s_xor[m15][p15]) / 2;

                                                                                                        counter[w4 - 1] += (uint64_t)((uint64_t)1 << num);  //这俩(uint64_t)不能少
                                                                                                    }
                                    }//x2_3
                                }
                            }
                        }
                    }//Δz0
                }//z4_3
*/


//第一行——尾
/*尾部第一种——0

                    //ΔX4—逆MC—逆SR—>ΔY3  此处逆SR位置已固定，按逆SR前的位置改即可
                        p0 = Multiply(s1, 0x0e); p4 = Multiply(s2, 0x0b); p8 = Multiply(s3, 0x0d); p12 = Multiply(s4, 0x09); //0
                        p5 = Multiply(s1, 0x09); p9 = Multiply(s2, 0x0e); p13 = Multiply(s3, 0x0b); p1 = Multiply(s4, 0x0d);
                        p10 = Multiply(s1, 0x0d); p14 = Multiply(s2, 0x09); p2 = Multiply(s3, 0x0e); p6 = Multiply(s4, 0x0b);
                        p15 = Multiply(s1, 0x0b); p3 = Multiply(s2, 0x0d); p7 = Multiply(s3, 0x09); p11 = Multiply(s4, 0x0e);

尾部第一种 ——1
                    p0 = Multiply(s4, 0x09); p4 = Multiply(s1, 0x0e); p8 = Multiply(s2, 0x0b); p12 = Multiply(s3, 0x0d); //1
                    p5 = Multiply(s4, 0x0d); p9 = Multiply(s1, 0x09); p13 = Multiply(s2, 0x0e); p1 = Multiply(s3, 0x0b);
                    p10 = Multiply(s4, 0x0b); p14 = Multiply(s1, 0x0d); p2 = Multiply(s2, 0x09); p6 = Multiply(s3, 0x0e);
                    p15 = Multiply(s4, 0x0e); p3 = Multiply(s1, 0x0b); p7 = Multiply(s2, 0x0d); p11 = Multiply(s3, 0x09);

尾部第二种 ——2
                    //ΔX4—逆MC—逆SR—>ΔY3  此处逆SR位置已固定，按逆SR前的位置改即可
                    p0 = Multiply(s3, 0x0d); p4 = Multiply(s4, 0x09); p8 = Multiply(s1, 0x0e); p12 = Multiply(s2, 0x0b); //2
                    p5 = Multiply(s3, 0x0b); p9 = Multiply(s4, 0x0d); p13 = Multiply(s1, 0x09); p1 = Multiply(s2, 0x0e);
                    p10 = Multiply(s3, 0x0e); p14 = Multiply(s4, 0x0b); p2 = Multiply(s1, 0x0d); p6 = Multiply(s2, 0x09);
                    p15 = Multiply(s3, 0x09); p3 = Multiply(s4, 0x0e); p7 = Multiply(s1, 0x0b); p11 = Multiply(s2, 0x0d);

尾部第三种——3

                    //ΔX4—逆MC—逆SR—>ΔY3  此处逆SR位置已固定，按逆SR前的位置改即可
                    p0 = Multiply(s2, 0x0b); p4 = Multiply(s3, 0x0d); p8 = Multiply(s4, 0x09); p12 = Multiply(s1, 0x0e); //3
                    p5 = Multiply(s2, 0x0e); p9 = Multiply(s3, 0x0b); p13 = Multiply(s4, 0x0d); p1 = Multiply(s1, 0x09);
                    p10 = Multiply(s2, 0x09); p14 = Multiply(s3, 0x0e); p2 = Multiply(s4, 0x0b); p6 = Multiply(s1, 0x0d);
                    p15 = Multiply(s2, 0x0d); p3 = Multiply(s3, 0x09); p7 = Multiply(s4, 0x0e); p11 = Multiply(s1, 0x0b);


                    //按行的顺序对四个字节写列混淆，对应的列数——找到相应的S（0，1，2，3），根据S对应行数——MC矩阵的对应列数——四个常数写入即可
                    //写好列混淆后，将变量名“左移”即可（相当于循环右移）
*/
 