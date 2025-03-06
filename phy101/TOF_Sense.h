#ifndef _TOF_SENSE_H_
#define _TOF_SENSE_H_

#include "Arduino.h"
#include <HardwareSerial.h>

#define TOF_RX_PIN  PA3
#define TOF_TX_PIN  PA2
#define TOF_UART Serial2

#define TOF_FRAME_HEADER 0x57
#define TOF_FUNCTION_MARK 0x00

typedef struct {
    uint8_t id;
    uint32_t system_time;
    uint32_t dis;
    uint8_t dis_status;
    uint16_t signal_strength;
    uint8_t range_precision;
} TOF_Parameter;

extern TOF_Parameter TOF_0;

void TOF_Active_Decoding();
void TOF_Inquire_Decoding(uint8_t id);

#endif
