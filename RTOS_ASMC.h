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
#define TIMER_TICK_DELAY_uSec 0.5 // 1 / 2MHz = 0.5uSec * OCR value -> High and Low delay for PWM

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
    void Begin(const int motor_index, unsigned int max_speed, unsigned int min_speed, unsigned int home_speed);
    void InitializeSemaphore(SemaphoreHandle_t motor1_key, SemaphoreHandle_t motor2_key, SemaphoreHandle_t motor3_key, \
                                    SemaphoreHandle_t motor4_key, SemaphoreHandle_t motor5_key, SemaphoreHandle_t motor6_key);

    void CreateTasks();
    //void vTaskSerial();
    void Run(const int motor_index, float target_position_mm);

//Motor member variables
public:
    //Used for Real-time Running / It should be array style
    static volatile int _motor_status[MAX_MOTOR_NUMBER] = {0, }; //status of motors
    static volatile bool _positive_direction[MAX_MOTOR_NUMBER] = {1, }; // positive: 1, negative: 0
    static volatile float _current_position[MAX_MOTOR_NUMBER] = {0, }; //motor current position
    static volatile float _target_position[MAX_MOTOR_NUMBER] = {0, };
    static volatile int _count_moving_step[MAX_MOTOR_NUMBER] = {0, }; //if target position == current position, 0
    static bool isHoming[MAX_MOTOR_NUMBER] = {false, };
    static bool isPumpHoming[MAX_MOTOR_NUMBER]= {false, };

    static volatile int _is_acceleration[MAX_MOTOR_NUMBER] = {1, }; //acceleration: 1, no acceleration: 0
    static volatile float _acceleration_section_step_1[MAX_MOTOR_NUMBER] = {50, };
    static volatile float _acceleration_section_step_2[MAX_MOTOR_NUMBER] = {100, };
    static volatile float _acceleration_section_step_3[MAX_MOTOR_NUMBER] = {200, };
    static volatile float _acceleration_section_step_4[MAX_MOTOR_NUMBER] = {300, };
    static volatile float _acceleration_section_step_5[MAX_MOTOR_NUMBER] = {400, };
    static volatile float _deceleration_position_1[MAX_MOTOR_NUMBER] = {0, };
    static volatile float _deceleration_position_2[MAX_MOTOR_NUMBER] = {0, };
    static volatile float _deceleration_position_3[MAX_MOTOR_NUMBER] = {0, };
    static volatile float _deceleration_position_4[MAX_MOTOR_NUMBER] = {0, };
    static volatile float _deceleration_position_5[MAX_MOTOR_NUMBER] = {0, };

    static int _STEP[MAX_MOTOR_NUMBER] = {0, };
    static int _DIR[MAX_MOTOR_NUMBER] = {0, };
    static int _HOME[MAX_MOTOR_NUMBER] = {0, };
    static int _STLFLT[MAX_MOTOR_NUMBER] = {0, };

    float _lead_one_step[MAX_MOTOR_NUMBER] = {0, };

    static unsigned long _OCR_max_speed[MAX_MOTOR_NUMBER] = {0, };
    static unsigned long _OCR_min_speed[MAX_MOTOR_NUMBER] = {0, };
    static unsigned long _OCR_mid_speed_1[MAX_MOTOR_NUMBER] = {0, };
    static unsigned long _OCR_mid_speed_2[MAX_MOTOR_NUMBER] = {0, };
    static unsigned long _OCR_mid_speed_3[MAX_MOTOR_NUMBER] = {0, };
    static unsigned long _OCR_mid_speed_4[MAX_MOTOR_NUMBER] = {0, };
    static unsigned long _OCR_Update[MAX_MOTOR_NUMBER] = {0, };
    static unsigned long _OCR_home[MAX_MOTOR_NUMBER] = {0, };

public:
    int _TOTAL_MOTOR_NUMBER = 0;
    int POSITIVE = 1;
    int NEGATIVE = 0;
    int _SS = 0;
    int _SLEEP = 0;
    int _RESET = 48;

    float _distance = 0; //_target_position - _current_position
    float _abs_distance = 0;




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
