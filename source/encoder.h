#ifndef ENCODER_H
#define ENCODER_H

enum {
    ENC_L0,
    ENC_L1,
    ENC_R0,
    ENC_R1,
};


extern void Encoder_Init(void);
void Process_Encoder(u8);

#endif //ENCODER_H