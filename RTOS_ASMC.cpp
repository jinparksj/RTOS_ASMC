//
// Created by jin on 2/19/20.
//
#include "RTOS_ASMC.h"

static void vTaskSerial(void *pvParameters);

static String user_command = "";
static String uart_command = "";
static String rs485_command = "";
static RTOS_RS485* RS485COM;

RTOS_ASMC::RTOS_ASMC(long BPS, int Mega_ID, int TE_PIN, int total_motor_number) : RTOS_RS485(BPS, Mega_ID, TE_PIN) {

    RS485COM = new RTOS_RS485(BPS, Mega_ID, TE_PIN);
    _TOTAL_MOTOR_NUMBER = total_motor_number;
}

void RTOS_ASMC::InitializeSystem(int motor_number[], float motor_current[], unsigned int max_speed[], unsigned int min_speed[], \
                                unsigned int home_speed[], int microstepping[], float lead[], \
                                int sample_time_BEMF[], int stall_detection_count[], int stall_detection_threshold[]) {
    //Initialize Motor, RS485, and Servo
    RS485COM->InitializeCommunication();

    for (int i = 0; i < _TOTAL_MOTOR_NUMBER; i++) {
        InitializeMotors(motor_number[i], motor_current[i], max_speed[i], min_speed[i], home_speed[i], microstepping[i], lead[i], \
                         sample_time_BEMF[i], stall_detection_count[i], stall_detection_threshold[i]);
    }


}

void RTOS_ASMC::InitializeMotors(int motor_number, float motor_current, unsigned int max_speed, unsigned int min_speed, \
                                unsigned int home_speed, int microstepping, float lead, \
                                int sample_time_BEMF = 7, int stall_detection_count = 3, int stall_detection_threshold = 0) {

    //  1) Sampling Time for BEMF - 0: 50us, 1: 100us, 2: 200us, 3: 300us, 4: 400us, 5: 600us, 6: 800us, 7: 1000us
    //  2) Stall Detection Count - 0~3 -> 2^0 ~ 2^3
    //  3) Stall Detection Threshold - 0~255 -> Emperically, decided
    
    NEGATIVE = 0;
    POSITIVE = 1;

    int _motor_index = (motor_number % 10) - 1;
    
    if (_motor_index > 5) {
        Serial.println("%Wrong assigned motor number");
        _motor_index = 5;
    }  
    else if (_motor_index < 0 ) {
        Serial.println("%Wrong assigned motor number");
        _motor_index = 0;
    }
    
    switch (motor_number){
        case 1:
            _STEP[_motor_index] = A0;
            _DIR[_motor_index] = A1;
            _SS = 31;
            _SLEEP = 39;
            _STLFLT[_motor_index] = 22;
            _HOME[_motor_index] = A8;
            _RESET = 48;
            break;
        case 2:

            _STEP[_motor_index] = A6;
            _DIR[_motor_index] = A7;
            _SS = 30;
            _SLEEP = A2;
            _STLFLT[_motor_index] = 24;
            _HOME[_motor_index] = A9;
            _RESET = 48;
            break;
        case 3:

            _STEP[_motor_index] = 47;
            _DIR[_motor_index] = 49;
            _SS = 33;
            _SLEEP = A3;
            _STLFLT[_motor_index] = 26;
            _HOME[_motor_index] = A10;
            _RESET = 48;
            break;
        case 4:

            _STEP[_motor_index] = 27;
            _DIR[_motor_index] = 29;
            _SS = 32;
            _SLEEP = 25;
            _STLFLT[_motor_index] = 37;
            _HOME[_motor_index] = A11;
            _RESET = 48;
            break;
        case 5:

            _STEP[_motor_index] = 45;
            _DIR[_motor_index] = 44;
            _SS = 35;
            _SLEEP = 28;
            _STLFLT[_motor_index] = 36;
            _HOME[_motor_index] = A12;
            _RESET = 48;
            break;
        case 6:

            _STEP[_motor_index] = 41;
            _DIR[_motor_index] = 40;
            _SS = 34;
            _SLEEP = 43;
            _STLFLT[_motor_index] = 38;
            _HOME[_motor_index] = A13;
            _RESET = 48;
            break;
        case 11:

            _STEP[_motor_index] = 45;
            _DIR[_motor_index] = 44;
            _SS = 35;
            _SLEEP = 28;
            _STLFLT[_motor_index] = 36;
            _HOME[_motor_index] = A12;
            _RESET = 48;
            break;
        case 14:

            _STEP[_motor_index] = 27;
            _DIR[_motor_index] = 29;
            _SS = 32;
            _SLEEP = 25;
            _STLFLT[_motor_index] = 37;
            _HOME[_motor_index] = A11;
            _RESET = 48;
            break;
        case 22:

            _STEP[_motor_index] = 47;
            _DIR[_motor_index] = 49;
            _SS = 33;
            _SLEEP = A3;
            _STLFLT[_motor_index] = 26;
            _HOME[_motor_index] = A10;
            _RESET = 48;
            break;
        case 101:

            _STEP[_motor_index] = A0;
            _DIR[_motor_index] = A1;
            _SS = 30;
            _SLEEP = 38;
            _STLFLT[_motor_index] = 23;
            _HOME[_motor_index] = A8;
            _RESET = 49;
            break;
        case 102:

            _STEP[_motor_index] = A6;
            _DIR[_motor_index] = A7;
            _SS = 31;
            _SLEEP = A2;
            _STLFLT[_motor_index] = 25;
            _HOME[_motor_index] = A9;
            _RESET = 49;
            break;
        case 103:

            _STEP[_motor_index] = 46;
            _DIR[_motor_index] = 48;
            _SS = 32;
            _SLEEP = A3;
            _STLFLT[_motor_index] = 27;
            _HOME[_motor_index] = A10;
            _RESET = 49;
            break;
        case 104:

            _STEP[_motor_index] = 26;
            _DIR[_motor_index] = 28;
            _SS = 33;
            _SLEEP = 24;
            _STLFLT[_motor_index] = 36;
            _HOME[_motor_index] = A11;
            _RESET = 49;
            break;
        case 105:

            _STEP[_motor_index] = 44;
            _DIR[_motor_index] = 45;
            _SS = 34;
            _SLEEP = 29;
            _STLFLT[_motor_index] = 37;
            _HOME[_motor_index] = A12;
            _RESET = 49;
            break;
        case 106:

            _STEP[_motor_index] = 40;
            _DIR[_motor_index] = 41;
            _SS = 35;
            _SLEEP = 42;
            _STLFLT[_motor_index] = 39;
            _HOME[_motor_index] = A13;
            _RESET = 49;
            break;
        case 111:

            _STEP[_motor_index] = 44;
            _DIR[_motor_index] = 45;
            _SS = 34;
            _SLEEP = 29;
            _STLFLT[_motor_index] = 37;
            _HOME[_motor_index] = A12;
            _RESET = 49;
            break;
        case 114:

            _STEP[_motor_index] = 26;
            _DIR[_motor_index] = 28;
            _SS = 33;
            _SLEEP = 24;
            _STLFLT[_motor_index] = 36;
            _HOME[_motor_index] = A11;
            _RESET = 49;
            break;
    }

    SPI_DRV8711 SPIMOTOR(_SS, _SLEEP, motor_current, microstepping);
    SPIMOTOR.BootUpSPI();

    pinMode(_STEP[_motor_index], OUTPUT);
    pinMode(_DIR[_motor_index], OUTPUT);
    pinMode(_STLFLT[_motor_index], INPUT);
    pinMode(_RESET, OUTPUT);
    pinMode(_HOME[_motor_index], INPUT);

    digitalWrite(_STEP[_motor_index], LOW);
    digitalWrite(_DIR[_motor_index], LOW);
    
    SPIMOTOR._TORQ_SMPLTH = sample_time_BEMF; //0: 50us, 1: 100us, 2: 200us, 3: 300us, 4: 400us, 5: 600us, 6: 800us, 7: 1000us
    SPIMOTOR._STALL_SDCNT = stall_detection_count;  //0~4, 1, 2, 4, 8
    SPIMOTOR._STALL_SDTHR = stall_detection_threshold; //0~255

    SPIMOTOR.Write_TORQUE(motor_current);
    SPIMOTOR.Write_CTRL_Disable(microstepping);
    SPIMOTOR.Write_OFF();
    SPIMOTOR.Write_BLANK();
    SPIMOTOR.Write_DECAY();
    SPIMOTOR.Write_STALL();
    SPIMOTOR.Write_DRIVE();
    SPIMOTOR.Write_STATUS();
    SPIMOTOR.Write_CTRL_Enable(microstepping);
    SPIMOTOR.ReadAllRegisters();
    SPI.end();
    
    Serial.print(motor_number); Serial.print(" / ");
    Serial.print(_STEP[_motor_index]); Serial.print(" / ");
    Serial.print(_DIR[_motor_index]); Serial.print(" / ");
    Serial.print(_SS); Serial.print(" / ");
    Serial.print(_SLEEP); Serial.print(" / ");
    Serial.print(_STLFLT[_motor_index]); Serial.print(" / ");
    Serial.print(_HOME[_motor_index]); Serial.print(" / ");
    Serial.print(_RESET); Serial.println(" / ");

    _motor_status[_motor_index] = 0; // status of motor, 0 = Idle, 1 = Accel, 2 = const speed, 3 = decel
    _current_position[ _motor_index ] = 0;
    _target_position[ _motor_index ] = 0;
    _deceleration_position_1[ _motor_index ] = 0;
    _deceleration_position_2[ _motor_index ] = 0;
    _deceleration_position_3[ _motor_index ] = 0;
    _deceleration_position_4[ _motor_index ] = 0;
    _deceleration_position_5[ _motor_index ] = 0;
    //lead: one revolution with microstepping (200 steps * microstepping) is converted to linear transition (mm)
    _lead_one_step[_motor_index] = lead / (200 * (2 << (microstepping - 1))); //one step -> how long it is moves linearly in mm (unit: mm / step);
    _acceleration_section_step_1[_motor_index] = 1 * 200 * (2 << (microstepping - 1)); // half revolution (step)
    _acceleration_section_step_2[_motor_index] = 1.5 * 200 * (2 << (microstepping - 1)); //
    _acceleration_section_step_3[_motor_index] = 2 * 200 * (2 << (microstepping - 1));
    _acceleration_section_step_4[_motor_index] = 2.5 * 200 * (2 << (microstepping - 1));
    _acceleration_section_step_5[_motor_index] = 3 * 200 * (2 << (microstepping - 1));

    Begin(_motor_index, max_speed, min_speed, home_speed);
}

void RTOS_ASMC::Begin(const int motor_index, unsigned int max_speed, unsigned int min_speed, unsigned int home_speed) {
    /*
     * Begin: Initiate motor object by providing default maximum speed, initial speed and acceleration
     * SPEED: 35 ~ 7000
     */
    if (min_speed < 35) {
        min_speed = 35;
    }
    else if (min_speed > 7000) {
        min_speed = 7000;
    }

    if (max_speed < 35) {
        max_speed = 35;
    }
    else if (max_speed > 7000) {
        max_speed = 7000;
    }

    //_CLOCK_FREQUENCY_IO : 2000000
    _OCR_max_speed[motor_index] = 2000000 / max_speed;
    _OCR_min_speed[motor_index] = 2000000 / min_speed;
    _OCR_mid_speed_1[motor_index] = 2000000 / (max_speed - (max_speed - min_speed) / 1.2);
    _OCR_mid_speed_2[motor_index] = 2000000 / (max_speed - (max_speed - min_speed) / 1.5);
    _OCR_mid_speed_3[motor_index] = 2000000 / (max_speed - (max_speed - min_speed) / 2);
    _OCR_mid_speed_4[motor_index] = 2000000 / (max_speed - (max_speed - min_speed) / 4);
    _OCR_home[motor_index] = 2000000 / home_speed;
    _OCR_Update[motor_index] = _OCR_min_speed[motor_index];

    Serial.print("%_OCR_max_speed : "); Serial.println(_OCR_max_speed[motor_index]);
    Serial.print("%_OCR_mid_speed_4 : "); Serial.println(_OCR_mid_speed_4[motor_index]);
    Serial.print("%_OCR_mid_speed_3 : "); Serial.println(_OCR_mid_speed_3[motor_index]);
    Serial.print("%_OCR_mid_speed_2 : "); Serial.println(_OCR_mid_speed_2[motor_index]);
    Serial.print("%_OCR_mid_speed_1 : "); Serial.println(_OCR_mid_speed_1[motor_index]);
    Serial.print("%_OCR_min_speed : "); Serial.println(_OCR_min_speed[motor_index]);
}

void RTOS_ASMC::InitializeSemaphore(SemaphoreHandle_t motor1_key, SemaphoreHandle_t motor2_key, SemaphoreHandle_t motor3_key, \
                                    SemaphoreHandle_t motor4_key, SemaphoreHandle_t motor5_key, SemaphoreHandle_t motor6_key) {

    _motor1_key = motor1_key;
    _motor2_key = motor2_key;
    _motor3_key = motor3_key;
    _motor4_key = motor4_key;
    _motor5_key = motor5_key;
    _motor6_key = motor6_key;

    vSemaphoreCreateBinary(_motor1_key);
    vSemaphoreCreateBinary(_motor2_key);
    vSemaphoreCreateBinary(_motor3_key);
    vSemaphoreCreateBinary(_motor4_key);
    vSemaphoreCreateBinary(_motor5_key);
    vSemaphoreCreateBinary(_motor6_key);

    xSemaphoreTake(_motor1_key, 0);
    xSemaphoreTake(_motor2_key, 0);
    xSemaphoreTake(_motor3_key, 0);
    xSemaphoreTake(_motor4_key, 0);
    xSemaphoreTake(_motor5_key, 0);
    xSemaphoreTake(_motor6_key, 0);


}

void RTOS_ASMC::CreateTasks() {
    xTaskCreate(vTaskSerial, NULL, configMINIMAL_STACK_SIZE, NULL, 2, NULL);

}



void RTOS_ASMC::Run(const int motor_index, float target_position_mm) {
    /*
     * one half revolution takes one section of scurve: 100 steps
     */

    _count_moving_step[motor_index] = 0;
    _target_position[motor_index] = target_position_mm / _lead_one_step[motor_index]; //_target_position unit - step, target_position - mm
    _distance = _target_position[motor_index] - _current_position[motor_index];
    _abs_distance = abs(_distance);

    if (_distance > 0) {
        digitalWrite(_DIR[motor_index], POSITIVE);
        _positive_direction[motor_index] = 1;
    }
    else {
        digitalWrite(_DIR[motor_index], NEGATIVE);
        _positive_direction[motor_index] = 0;
    }

    _deceleration_position_1[motor_index] = _abs_distance - _acceleration_section_step_1[motor_index]; // 3
    _deceleration_position_2[motor_index] = _abs_distance - _acceleration_section_step_2[motor_index]; // 5
    _deceleration_position_3[motor_index] = _abs_distance - _acceleration_section_step_3[motor_index]; // 6
    _deceleration_position_4[motor_index] = _abs_distance - _acceleration_section_step_4[motor_index]; // 6
    _deceleration_position_5[motor_index] = _abs_distance - _acceleration_section_step_5[motor_index]; // 6

    if (_deceleration_position_1[motor_index] < 0) {
        _is_acceleration[motor_index] = 0;
    }
    else if ((_deceleration_position_1[motor_index] > 0) && (_deceleration_position_2[motor_index] < 0)){
        _is_acceleration[motor_index] = 1;
    }
    else if ((_deceleration_position_2[motor_index] > 0) && (_deceleration_position_3[motor_index] < 0)) {
        _is_acceleration[motor_index] = 2;
    }
    else if ((_deceleration_position_3[motor_index] > 0) && (_deceleration_position_4[motor_index] < 0)) {
        _is_acceleration[motor_index] = 3;
    }
    else if ((_deceleration_position_4[motor_index] > 0) && (_deceleration_position_5[motor_index] < 0)) {
        _is_acceleration[motor_index] = 4;
    }
    else if ((_deceleration_position_5[motor_index] > 0)){
        _is_acceleration[motor_index] = 5;
    }
    _motor_status[motor_index] = 1;
}



static void vTaskSerial(void *pvParameters) {
    Serial.println("%Ready to Get UART Serial Packets");
    while (true) {
        if (Serial.available() > 0) {
            uart_command = Serial.readStringUntil('/');
        }
        vTaskDelay( 100 / portTICK_PERIOD_MS);
    }
}

static void vTaskMotor1(int *pvMotorNumber) {
//    if (_status == 0)
//        return IDLE_RESPONSE;
//    switch(_positive_direction) {
//        case 1:
//            if (_current_position >= _target_position) {
//                _status = 0;
//                _current_position = _target_position;
//                _count_moving_step = 0;
//                if (isPumpHoming) {
//                    _current_position = 0;
//                    isPumpHoming = false;
//                }
//                return 10;
//            }
//            break;
//        case 0:
//            if (_current_position <= _target_position) {
//                _status = 0;
//                _count_moving_step = 0;
//                _current_position = _target_position;
//                if (isPumpHoming) {
//                    _current_position = 0;
//                    isPumpHoming = false;
//                }
//                return 10;
//            }
//            break;
//        default:
//            break;
//    }
//    if ((digitalRead(_HOME) == false) && isHoming) {
//        _current_position = 0;
//        _target_position = 0;
//        _count_moving_step = 0;
//        isHoming = false;
//        return 1;
//    }
//
//    _count_moving_step++;
//
//    switch(_is_acceleration) {
//        case 0:
//            _OCR_Update = _OCR_min_speed;
//            break;
//        case 1:
//            if (_count_moving_step > _acceleration_section_step_1 && _count_moving_step < _deceleration_position_1) {
//                _OCR_Update = _OCR_mid_speed_1;
//            }
//            else {
//                _OCR_Update = _OCR_min_speed;
//            }
//            break;
//        case 2:
//            if (_count_moving_step > _acceleration_section_step_1 && _count_moving_step < _deceleration_position_1) {
//                if (_count_moving_step > _acceleration_section_step_2 && _count_moving_step < _deceleration_position_2) {
//                    _OCR_Update = _OCR_mid_speed_2;
//                }
//                else {
//                    _OCR_Update = _OCR_mid_speed_1;
//                }
//            }
//            else {
//                _OCR_Update = _OCR_min_speed;
//            }
//            break;
//        case 3:
//            if (_count_moving_step > _acceleration_section_step_1 && _count_moving_step < _deceleration_position_1) {
//                if (_count_moving_step > _acceleration_section_step_2 && _count_moving_step < _deceleration_position_2) {
//                    if (_count_moving_step > _acceleration_section_step_3 && _count_moving_step < _deceleration_position_3) {
//                        _OCR_Update = _OCR_max_speed;
//                    }
//                    else {
//                        _OCR_Update = _OCR_mid_speed_2;
//                    }
//                }
//                else {
//                    _OCR_Update = _OCR_mid_speed_1;
//                }
//            }
//            else {
//                _OCR_Update = _OCR_min_speed;
//            }
//            break;
//        case 4:
//            if (_count_moving_step > _acceleration_section_step_1 && _count_moving_step < _deceleration_position_1) {
//                if (_count_moving_step > _acceleration_section_step_2 && _count_moving_step < _deceleration_position_2) {
//                    if (_count_moving_step > _acceleration_section_step_3 && _count_moving_step < _deceleration_position_3) {
//                        if (_count_moving_step > _acceleration_section_step_4 && _count_moving_step < _deceleration_position_4) {
//                            _OCR_Update = _OCR_max_speed;
//                        }
//                        else{
//                            _OCR_Update = _OCR_mid_speed_3;
//                        }
//                    }
//                    else {
//                        _OCR_Update = _OCR_mid_speed_2;
//                    }
//                }
//                else {
//                    _OCR_Update = _OCR_mid_speed_1;
//                }
//            }
//            else {
//                _OCR_Update = _OCR_min_speed;
//            }
//            break;
//        case 5:
//            if (_count_moving_step > _acceleration_section_step_1 && _count_moving_step < _deceleration_position_1) {
//                if (_count_moving_step > _acceleration_section_step_2 && _count_moving_step < _deceleration_position_2) {
//                    if (_count_moving_step > _acceleration_section_step_3 && _count_moving_step < _deceleration_position_3) {
//                        if (_count_moving_step > _acceleration_section_step_4 && _count_moving_step < _deceleration_position_4) {
//                            if (_count_moving_step > _acceleration_section_step_5 && _count_moving_step < _deceleration_position_5) {
//                                _OCR_Update = _OCR_max_speed;
//                            }
//                            else {
//                                _OCR_Update = _OCR_mid_speed_4;
//                            }
//                        }
//                        else{
//                            _OCR_Update = _OCR_mid_speed_3;
//                        }
//                    }
//                    else {
//                        _OCR_Update = _OCR_mid_speed_2;
//                    }
//                }
//                else {
//                    _OCR_Update = _OCR_mid_speed_1;
//                }
//            }
//            else {
//                _OCR_Update = _OCR_min_speed;
//            }
//            break;
//        case 6:
//            _OCR_Update = _OCR_home;
//            break;
//        default:
//            break;
//    }
//
//    OCR1A = _OCR_Update; // compare match register
//    if (TCNT1 > OCR1A) TCNT1 = 0;
//
//    digitalWrite(_STEP, HIGH);
//    switch (_positive_direction) {
//        case 1:
//            _current_position++;
//            break;
//        case 0:
//            _current_position--;
//            break;
//        default:
//            break;
//    }
//
//    __asm__ __volatile__ (
//    "nop" "\n\t"
//    "nop" "\n\t"
//    "nop" "\n\t"); // wasting one cycle ( 6.2ns on a 16MHz Arduino)
//    // Wait for step pulse
//    digitalWrite(_STEP, LOW);
//
//    return NORMAL_RESPONSE;

}

static void vTaskMotor2(void *pvParameters) {

}

static void vTaskMotor3(void *pvParameters) {

}

static void vTaskMotor4(void *pvParameters) {

}

static void vTaskMotor5(void *pvParameters) {

}

static void vTaskMotor6(void *pvParameters) {

}