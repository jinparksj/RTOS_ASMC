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

#define MAX_MOTOR_NUMBER 6

class RTOS_ASMC : public RTOS_RS485 {
public:
    RTOS_ASMC(long BPS, int Mega_ID, int TE_PIN, int total_motor_number);

public:
    void InitializeSystem(int motor_number[], float motor_current[], unsigned int max_speed[], unsigned int min_speed[], \
                                unsigned int home_speed[], int microstepping[], float lead[], \
                                int sample_time_BEMF[], int stall_detection_count[], int stall_detection_threshold[]);
    void InitializeMotors(int motor_number, float motor_current, unsigned int max_speed, unsigned int min_speed, \
                                unsigned int home_speed, int microstepping, float lead, \
                                int sample_time_BEMF, int stall_detection_count, int stall_detection_threshold);
    void Begin(unsigned int max_speed, unsigned int min_speed, unsigned int home_speed);
    void InitializeSemaphore(SemaphoreHandle_t motor1_key, SemaphoreHandle_t motor2_key, SemaphoreHandle_t motor3_key, \
                                    SemaphoreHandle_t motor4_key, SemaphoreHandle_t motor5_key, SemaphoreHandle_t motor6_key);

    void CreateTasks();
    //void vTaskSerial();
    void Run(float target_position_mm);

//Motor member variables
public:
    int _TOTAL_MOTOR_NUMBER = 0;
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
    int POSITIVE = 1;
    int NEGATIVE = 0;
    unsigned long _OCR_max_speed = 0;
    unsigned long _OCR_min_speed = 0;
    unsigned long _OCR_mid_speed_1 = 0;
    unsigned long _OCR_mid_speed_2 = 0;
    unsigned long _OCR_mid_speed_3 = 0;
    unsigned long _OCR_mid_speed_4 = 0;
    unsigned long _OCR_Update = 0;
    unsigned long _OCR_home = 0;
    int _is_acceleration[MAX_MOTOR_NUMBER] = {1, }; //acceleration: 1, no acceleration: 0
    float _acceleration_section_step_1[MAX_MOTOR_NUMBER] = {50, };
    float _acceleration_section_step_2[MAX_MOTOR_NUMBER] = {100, };
    float _acceleration_section_step_3[MAX_MOTOR_NUMBER] = {200, };
    float _acceleration_section_step_4[MAX_MOTOR_NUMBER] = {300, };
    float _acceleration_section_step_5[MAX_MOTOR_NUMBER] = {400, };
    static volatile float _deceleration_position_1[MAX_MOTOR_NUMBER] = {0, };
    float _deceleration_position_2[MAX_MOTOR_NUMBER] = {0, };
    float _deceleration_position_3[MAX_MOTOR_NUMBER] = {0, };
    float _deceleration_position_4[MAX_MOTOR_NUMBER] = {0, };
    float _deceleration_position_5[MAX_MOTOR_NUMBER] = {0, };
    float _current_position[MAX_MOTOR_NUMBER] = {0, };
    float _target_position[MAX_MOTOR_NUMBER] = {0, };

    int _count_moving_step = 0;

    float _distance = 0; //_target_position - _current_position
    float _abs_distance = 0;
    float _lead_one_step = 0;
    bool isHoming = false;
    bool isPumpHoming = false;
    int _positive_direction = 1; // positive: 1, negative: 0

//Communication member variables
public:
    long _BPS = 250000;
    int _Mega_ID = 0;
    int _TE_PIN = 22;
//    RTOS_RS485* RS485COM;

//RTOS member variables
private:
    SemaphoreHandle_t _motor1_key;
    SemaphoreHandle_t _motor2_key;
    SemaphoreHandle_t _motor3_key;
    SemaphoreHandle_t _motor4_key;
    SemaphoreHandle_t _motor5_key;
    SemaphoreHandle_t _motor6_key;


};



#endif //RTOS_ASMC_RTOS_ASMC_H
