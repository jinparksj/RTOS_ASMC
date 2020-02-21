//
// Created by jin on 2/19/20.
//
#include "RTOS_ASMC.h"

RTOS_ASMC::RTOS_ASMC(long BPS, int Mega_ID, int TE_PIN, \
                    int motor_number, float motor_current, unsigned int max_speed, unsigned int min_speed, \
                    unsigned int home_speed, int microstepping, float lead, \
                    int sample_time_BEMF = 7, int stall_detection_count = 3, int stall_detection_threshold = 0) \
                    : RTOS_RS485(BPS, Mega_ID, TE_PIN){

    RS485COM = new RTOS_RS485(BPS, Mega_ID, TE_PIN);

    _motor_current = motor_current;
    _microstepping = microstepping;
    _max_speed = max_speed;
    _min_speed = min_speed;
    _home_speed = home_speed;
    _sampling_time_BEMF = sample_time_BEMF;
    _stall_detection_count = stall_detection_count;
    _stall_detection_threshold = stall_detection_threshold;

    switch (motor_number){
        case 1:
            _motor_number = motor_number;
            _STEP = A0;
            _DIR = A1;
            _SS = 31;
            _SLEEP = 39;
            _STLFLT = 22;
            _HOME = A8;
            _RESET = 48;
            break;
        case 2:
            _motor_number = motor_number;
            _STEP = A6;
            _DIR = A7;
            _SS = 30;
            _SLEEP = A2;
            _STLFLT = 24;
            _HOME = A9;
            _RESET = 48;
            break;
        case 3:
            _motor_number = motor_number;
            _STEP = 47;
            _DIR = 49;
            _SS = 33;
            _SLEEP = A3;
            _STLFLT = 26;
            _HOME = A10;
            _RESET = 48;
            break;
        case 4:
            _motor_number = motor_number;
            _STEP = 27;
            _DIR = 29;
            _SS = 32;
            _SLEEP = 25;
            _STLFLT = 37;
            _HOME = A11;
            _RESET = 48;
            break;
        case 5:
            _motor_number = motor_number;
            _STEP = 45;
            _DIR = 44;
            _SS = 35;
            _SLEEP = 28;
            _STLFLT = 36;
            _HOME = A12;
            _RESET = 48;
            break;
        case 6:
            _motor_number = motor_number;
            _STEP = 41;
            _DIR = 40;
            _SS = 34;
            _SLEEP = 43;
            _STLFLT = 38;
            _HOME = A13;
            _RESET = 48;
            break;
        case 11:
            _motor_number = motor_number;
            _STEP = 45;
            _DIR = 44;
            _SS = 35;
            _SLEEP = 28;
            _STLFLT = 36;
            _HOME = A12;
            _RESET = 48;
            break;
        case 14:
            _motor_number = motor_number;
            _STEP = 27;
            _DIR = 29;
            _SS = 32;
            _SLEEP = 25;
            _STLFLT = 37;
            _HOME = A11;
            _RESET = 48;
            break;
        case 22:
            _motor_number = motor_number;
            _STEP = 47;
            _DIR = 49;
            _SS = 33;
            _SLEEP = A3;
            _STLFLT = 26;
            _HOME = A10;
            _RESET = 48;
            break;
        case 101:
            _motor_number = motor_number;
            _STEP = A0;
            _DIR = A1;
            _SS = 30;
            _SLEEP = 38;
            _STLFLT = 23;
            _HOME = A8;
            _RESET = 49;
            break;
        case 102:
            _motor_number = motor_number;
            _STEP = A6;
            _DIR = A7;
            _SS = 31;
            _SLEEP = A2;
            _STLFLT = 25;
            _HOME = A9;
            _RESET = 49;
            break;
        case 103:
            _motor_number = motor_number;
            _STEP = 46;
            _DIR = 48;
            _SS = 32;
            _SLEEP = A3;
            _STLFLT = 27;
            _HOME = A10;
            _RESET = 49;
            break;
        case 104:
            _motor_number = motor_number;
            _STEP = 26;
            _DIR = 28;
            _SS = 33;
            _SLEEP = 24;
            _STLFLT = 36;
            _HOME = A11;
            _RESET = 49;
            break;
        case 105:
            _motor_number = motor_number;
            _STEP = 44;
            _DIR = 45;
            _SS = 34;
            _SLEEP = 29;
            _STLFLT = 37;
            _HOME = A12;
            _RESET = 49;
            break;
        case 106:
            _motor_number = motor_number;
            _STEP = 40;
            _DIR = 41;
            _SS = 35;
            _SLEEP = 42;
            _STLFLT = 39;
            _HOME = A13;
            _RESET = 49;
            break;
        case 111:
            _motor_number = motor_number;
            _STEP = 44;
            _DIR = 45;
            _SS = 34;
            _SLEEP = 29;
            _STLFLT = 37;
            _HOME = A12;
            _RESET = 49;
            break;
        case 114:
            _motor_number = motor_number;
            _STEP = 26;
            _DIR = 28;
            _SS = 33;
            _SLEEP = 24;
            _STLFLT = 36;
            _HOME = A11;
            _RESET = 49;
            break;
    }

    SPI_DRV8711 SPIMOTOR(_SS, _SLEEP, _motor_current, _microstepping);
    SPIMOTOR.BootUpSPI();

    pinMode(_STEP, OUTPUT);
    pinMode(_DIR, OUTPUT);
    pinMode(_STLFLT, INPUT);
    pinMode(_RESET, OUTPUT);
    pinMode(_HOME, INPUT);

    digitalWrite(_STEP, LOW);
    digitalWrite(_DIR, LOW);
    SPI.end();

    _status = 0; // status of motor, 0 = Idle, 1 = Accel, 2 = const speed, 3 = decel
    _current_position = 0;
    _target_position = 0;
    _deceleration_position_1 = 0;
    _deceleration_position_2 = 0;
    //lead: one revolution with microstepping (200 steps * microstepping) is converted to linear transition (mm)
    _lead_one_step = lead / (200 * (2 << (_microstepping - 1))); //one step -> how long it is moves linearly in mm (unit: mm / step);
    _acceleration_section_step_1 = 1 * 200 * (2 << (_microstepping - 1)); // half revolution (step)
    _acceleration_section_step_2 = 1.5 * 200 * (2 << (_microstepping - 1)); //
    _acceleration_section_step_3 = 2 * 200 * (2 << (_microstepping - 1));
    _acceleration_section_step_4 = 2.5 * 200 * (2 << (_microstepping - 1));
    _acceleration_section_step_5 = 3 * 200 * (2 << (_microstepping - 1));
}

void RTOS_ASMC::InitializeSystem() {
    //Initialize Motor, RS485, and Servo
    RS485COM->InitializeCommunication();
    InitialzeMotors();
}

void RTOS_ASMC::InitializeMotors() {
    NEGATIVE = 0;
    POSITIVE = 1;
    Serial.print(_motor_number); Serial.print(" / ");
    Serial.print(_STEP); Serial.print(" / ");
    Serial.print(_DIR); Serial.print(" / ");
    Serial.print(_SS); Serial.print(" / ");
    Serial.print(_SLEEP); Serial.print(" / ");
    Serial.print(_STLFLT); Serial.print(" / ");
    Serial.print(_HOME); Serial.print(" / ");
    Serial.print(_RESET); Serial.println(" / ");

    SPI_DRV8711 SPIMOTOR(_SS, _SLEEP, _motor_current, _microstepping);

    SPIMOTOR._TORQ_SMPLTH = _sampling_time_BEMF; //0: 50us, 1: 100us, 2: 200us, 3: 300us, 4: 400us, 5: 600us, 6: 800us, 7: 1000us
    SPIMOTOR._STALL_SDCNT = _stall_detection_count;  //0~4, 1, 2, 4, 8
    SPIMOTOR._STALL_SDTHR = _stall_detection_threshold; //0~255

    SPIMOTOR.Write_TORQUE(_motor_current);
    SPIMOTOR.Write_CTRL_Disable(_microstepping);
    SPIMOTOR.Write_OFF();
    SPIMOTOR.Write_BLANK();
    SPIMOTOR.Write_DECAY();
    SPIMOTOR.Write_STALL();
    SPIMOTOR.Write_DRIVE();
    SPIMOTOR.Write_STATUS();
    SPIMOTOR.Write_CTRL_Enable(_microstepping);
    SPIMOTOR.ReadAllRegisters();
    SPI.end();

    Begin(_max_speed, _min_speed, _home_speed);
}

void RTOS_ASMC::Begin(unsigned int max_speed, unsigned int min_speed, unsigned int home_speed) {
    /*
     * Begin: Initiate motor object by providing default maximum speed, initial speed and acceleration
     * SPEED: 35 ~ 7000
     */
    if (min_speed < 35) {
        _min_speed = 35;
    }
    else if (min_speed > 7000) {
        _min_speed = 7000;
    }

    if (max_speed < 35) {
        _max_speed = 35;
    }
    else if (max_speed > 7000) {
        _max_speed = 7000;
    }

    _home_speed = home_speed;

    //_CLOCK_FREQUENCY_IO : 2000000
    _OCR_max_speed = 2000000 / _max_speed;
    _OCR_min_speed = 2000000 / _min_speed;
    _OCR_mid_speed_1 = 2000000 / (_max_speed - (_max_speed - _min_speed) / 1.2);
    _OCR_mid_speed_2 = 2000000 / (_max_speed - (_max_speed - _min_speed) / 1.5);
    _OCR_mid_speed_3 = 2000000 / (_max_speed - (_max_speed - _min_speed) / 2);
    _OCR_mid_speed_4 = 2000000 / (_max_speed - (_max_speed - _min_speed) / 4);
    _OCR_home = 2000000 / _home_speed;
    _OCR_Update = _OCR_min_speed;

    Serial.print("%_OCR_max_speed : "); Serial.println(_OCR_max_speed);
    Serial.print("%_OCR_mid_speed_4 : "); Serial.println(_OCR_mid_speed_4);
    Serial.print("%_OCR_mid_speed_3 : "); Serial.println(_OCR_mid_speed_3);
    Serial.print("%_OCR_mid_speed_2 : "); Serial.println(_OCR_mid_speed_2);
    Serial.print("%_OCR_mid_speed_1 : "); Serial.println(_OCR_mid_speed_1);
    Serial.print("%_OCR_min_speed : "); Serial.println(_OCR_min_speed);
}

void RTOS_ASMC::Run(float target_position_mm) {
    /*
     * one half revolution takes one section of scurve: 100 steps
     */
    _count_moving_step = 0;
    _target_position = target_position_mm / _lead_one_step; //_target_position unit - step, target_position - mm
    _distance = _target_position - _current_position;
    _abs_distance = abs(_distance);

    if (_distance > 0) {
        digitalWrite(_DIR, POSITIVE);
        _positive_direction = 1;
    }
    else {
        digitalWrite(_DIR, NEGATIVE);
        _positive_direction = 0;
    }

    _deceleration_position_1 = _abs_distance - _acceleration_section_step_1; // 3
    _deceleration_position_2 = _abs_distance - _acceleration_section_step_2; // 5
    _deceleration_position_3 = _abs_distance - _acceleration_section_step_3; // 6
    _deceleration_position_4 = _abs_distance - _acceleration_section_step_4; // 6
    _deceleration_position_5 = _abs_distance - _acceleration_section_step_5; // 6

    if (_deceleration_position_1 < 0) {
        _is_acceleration = 0;
    }
    else if ((_deceleration_position_1 > 0) && (_deceleration_position_2 < 0)){
        _is_acceleration = 1;
    }
    else if ((_deceleration_position_2 > 0) && (_deceleration_position_3 < 0)) {
        _is_acceleration = 2;
    }
    else if ((_deceleration_position_3 > 0) && (_deceleration_position_4 < 0)) {
        _is_acceleration = 3;
    }
    else if ((_deceleration_position_4 > 0) && (_deceleration_position_5 < 0)) {
        _is_acceleration = 4;
    }
    else if ((_deceleration_position_5 > 0)){
        _is_acceleration = 5;
    }
    _status = 1;
}

