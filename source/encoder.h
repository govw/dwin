#ifndef ENCODER_H
#define ENCODER_H

enum {
    ENC_L0,
    ENC_L1,
    ENC_R0,
    ENC_R1,
    ENC_STOP,
};


extern void Encoder_Init(void);
void Encoder_process_code(u8);

#endif //ENCODER_H