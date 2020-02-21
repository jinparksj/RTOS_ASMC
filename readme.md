# FreeRTOS Asynchronous Motor Control with RS-485
This library enables to control 4 actuators in an asynchronous manner through FreeRTOS

**1. RTOS Framework**

    #inclde <Arduino_FreeRTOS.h>
    #include <semphr.h>
    
    //1) Pre-define task main function
    void vTaskMain(void *pvParameters);
    
    void setup() {
        //2) Create Real-time Task 
        xTaskCreate(vTaskMain, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        
        //3) Start RTOS
        vTaskStartScheduler();
        
    }
    
    //4) Define task main function
    void vTaskMain(void *pvParameters) {
        //5) Initialization
        ...
        while (true) {
            //6) Task Process
            ...
        }
    }
    
    //Arduino ino should have this loop(), even though this script doesn't use this part.
    void loop() {} 

**2. Hardware**

    Main Hardware
    1) 1 MCU
    2) Motor Driver
    3) RS-485
    4) Servo
    
- **Arduino Mega**
- RS-485, Half-duplex RS-485 driver IC, MAX485, 250000 baudrate
- Four Stepper Motor, Syringe pump
- Stepper position is set based on optical homing sensor and PWM step during moving
- __DRV8711__ Texas Instruments Motor Driver Module (**Polulu**)
- Support microstepping
- Servo library

