//
// Created by jin on 2/19/20.
//

#ifndef RTOS_ASMC_RTOS_ASMC_H
#define RTOS_ASMC_RTOS_ASMC_H

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "RTOS_RS485.h"
#include "Motor/SPI_DRV8711.h"

class RTOS_ASMC : public RTOS_RS485 {
public:
    RTOS_ASMC(long BPS, int Mega_ID, int TE_PIN, \
                    int motor_number, float motor_current, unsigned int max_speed, unsigned int min_speed, \
                    unsigned int home_speed, int microstepping, float lead, \
                    int sample_time_BEMF = 7, int stall_detection_count = 3, int stall_detection_threshold = 0);
public:
    void InitializeSystem();
    void InitializeMotors();
    void Begin(unsigned int max_speed, unsigned int min_speed, unsigned int home_speed);
    void Run(float target_position_mm);


public:
    int _STEP = 0;
    int _DIR = 0;
    int _SS = 0;
    int _SLEEP = 0;
    int _STLFLT = 0;
    int _HOME = 0;
    int _RESET = 48;
    float _motor_current = 0;
    unsigned int _microstepping = 0; //full-step
    int _motor_number = 0;
    int _max_speed = 0; //max speed, unit = step/s
    int _min_speed = 0;
    int _home_speed = 0;
    int _sampling_time_BEMF = 7; //0: 50us, 1: 100us, 2: 200us, 3: 300us, 4: 400us, 5: 600us, 6: 800us, 7: 1000us
    int _stall_detection_count = 3; //0~3
    int _stall_detection_threshold = 0; //0~255
    int _status = 0; // status of motor

public:
    long _BPS = 250000;
    int _Mega_ID = 0;
    int _TE_PIN = 22;
    RTOS_RS485* RS485COM;


public:
    int POSITIVE = 1;
    int NEGATIVE = 0;

private:
    unsigned long _OCR_max_speed = 0;
    unsigned long _OCR_min_speed = 0;
    unsigned long _OCR_mid_speed_1 = 0;
    unsigned long _OCR_mid_speed_2 = 0;
    unsigned long _OCR_mid_speed_3 = 0;
    unsigned long _OCR_mid_speed_4 = 0;
    unsigned long _OCR_Update = 0;
    unsigned long _OCR_home = 0;
    int _is_acceleration = 1; //acceleration: 1, no acceleration: 0
    float _acceleration_section_step_1 = 50;
    float _acceleration_section_step_2 = 100;
    float _acceleration_section_step_3 = 200;
    float _acceleration_section_step_4 = 300;
    float _acceleration_section_step_5 = 400;
    float _deceleration_position_1 = 0;
    float _deceleration_position_2 = 0;
    float _deceleration_position_3 = 0;
    float _deceleration_position_4 = 0;
    float _deceleration_position_5 = 0;
    int _count_moving_step = 0;
    float _current_position = 0;
    float _target_position = 0;
    float _distance = 0; //_target_position - _current_position
    float _abs_distance = 0;
    float _lead_one_step = 0;
    bool isHoming = false;
    bool isPumpHoming = false;
    int _positive_direction = 1; // positive: 1, negative: 0

};



#endif //RTOS_ASMC_RTOS_ASMC_H