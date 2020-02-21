//
// Created by jin on 8/23/19.
//

#ifndef GFMOTORV2_SPI_DRV8711_H
#define GFMOTORV2_SPI_DRV8711_H

#include <SPI.h>
#include "Register_DRV8711.h"
#include <Arduino.h>
#define REGWRITE    0x00
#define REGREAD     0x80

class SPI_DRV8711 {
public:
    SPI_DRV8711(int SCSPIN, int SLEEPPIN, float motorCurrent, int microstepping);

public:
    void BootUpSPI();
    void WriteAllRegisters();
    unsigned int ReadWrite(unsigned int dataHigh, unsigned int dataLow);
    void ReadAllRegisters();
    unsigned int TorqueSetUp(float motorCurrent);
    void Write_CTRL_Disable(int microstepping);
    void Write_TORQUE(float motor_current);
    void Write_OFF();
    void Write_BLANK();
    void Write_DECAY();
    void Write_STALL();
    void Write_DRIVE();
    void Write_STATUS();
    void Write_CTRL_Enable(int microstepping);

    void Read_CTRL();
    void Read_TORQUE();
    void Read_BLANK();
    void Read_DECAY();
    void Read_STALL();
    void Read_DRIVE();
    void Read_STATUS();

public:
    struct CTRL_Register 	G_CTRL_REG;
    struct TORQUE_Register 	G_TORQUE_REG;
    struct OFF_Register 	G_OFF_REG;
    struct BLANK_Register	G_BLANK_REG;
    struct DECAY_Register 	G_DECAY_REG;
    struct STALL_Register 	G_STALL_REG;
    struct DRIVE_Register 	G_DRIVE_REG;
    struct STATUS_Register 	G_STATUS_REG;


public:
    unsigned int _TORQ_SMPLTH = _TORQ_SMPLTH_100US; //Default
    unsigned int _STALL_SDCNT = _STALL_SDCNT_2STEP;
    unsigned int _STALL_SDTHR = 0x30; //Default: Sets stall detect threshold, correct setting determined experimentally

    //0x00 CTRL Register
private:
    unsigned int _CTRL_ADDR = 0x00;
    unsigned int _CTRL_ENABLE = 0x01;
    unsigned int _CTRL_DISABLE = 0x00;
    unsigned int _CTRL_MODE_FULL = 0x00;
    unsigned int _CTRL_MODE_HALF = 0x01;
    unsigned int _CTRL_MODE_4 = 0x02;
    unsigned int _CTRL_MODE_8 = 0x03;
    unsigned int _CTRL_MODE_16 = 0x04;
    unsigned int _CTRL_MODE_32 = 0x05;
    unsigned int _CTRL_MODE_64 = 0x06;
    unsigned int _CTRL_MODE_128 = 0x07;
    unsigned int _CTRL_MODE_256 = 0x08;
    unsigned int _CTRL_EXSTALL_INTERNAL = 0x00;
    unsigned int _CTRL_EXSTALL_EXTERNAL = 0x01;
    unsigned int _CTRL_ISGAIN_5 = 0x00;
    unsigned int _CTRL_ISGAIN_10 = 0x01;
    unsigned int _CTRL_ISGAIN_20 = 0x02;
    unsigned int _CTRL_ISGAIN_40 = 0x03;
    unsigned int _CTRL_DTIME_400NS = 0x00;
    unsigned int _CTRL_DTIME_450NS = 0x01;
    unsigned int _CTRL_DTIME_650NS = 0x02;
    unsigned int _CTRL_DTIME_850NS = 0x03;
    unsigned int _CTRL_DTIME = _CTRL_DTIME_850NS;


    //0x01 TORQUE Register
private:
    unsigned int _TORQ_ADDR = 0x01;
    unsigned int _TORQ_TORQ = 0x5A;
    unsigned int _TORQ_SMPLTH_50US = 0x00;
    unsigned int _TORQ_SMPLTH_100US = 0x00;
    unsigned int _TORQ_SMPLTH_200US = 0x00;
    unsigned int _TORQ_SMPLTH_300US = 0x00;
    unsigned int _TORQ_SMPLTH_400US = 0x00;
    unsigned int _TORQ_SMPLTH_600US = 0x00;
    unsigned int _TORQ_SMPLTH_800US = 0x00;
    unsigned int _TORQ_SMPLTH_1000US = 0x00;


    //0x02 OFF Register
private:
    unsigned int _OFF_ADDR = 0x02;
    unsigned int _OFF_TOFF = 0x30; //Default for setting fixed off time
    unsigned int _OFF_PWMMODE_INTERNAL = 0x00;
    unsigned int _OFF_PWMMODE_BYPASS = 0x01;
    unsigned int _OFF_PWMMODE = _OFF_PWMMODE_INTERNAL;

    //0x03 BLANK Register
private:
    unsigned int _BLANK_ADDR = 0x03;
    unsigned int _BLANK_TBLANK = 0x80;//Default: 1000ns + 20ns * 128 = 3.56us, Current trip blanking time
    unsigned int _BLANK_ABT_ENABLE = 0x01;
    unsigned int _BLANK_ABT_DISABLE = 0x00;
    unsigned int _BLANK_ABT = _BLANK_ABT_ENABLE;

    //0x04 DECAY Register
private:
    unsigned int _DECAY_ADDR = 0x04;
    unsigned int _DECAY_TDECAY = 0x10; // Default: Mixed decay transition time
    unsigned int _DECAY_DECMOD_SLOW = 0x00;
    unsigned int _DECAY_DECMOD_SLOW_MIX = 0x01;
    unsigned int _DECAY_DECMOD_FAST = 0x02;
    unsigned int _DECAY_DECMOD_MIX = 0x03;
    unsigned int _DECAY_DECMOD_SLOW_AUTOMIX = 0x04;
    unsigned int _DECAY_DECMOD_AUTOMIX = 0x05;
    unsigned int _DECAY_DECMOD = _DECAY_DECMOD_MIX;

    //0x05 STALL Register
private:
    unsigned int _STALL_ADDR = 0x05;
    unsigned int _STALL_SDCNT_1STEP = 0x00;
    unsigned int _STALL_SDCNT_2STEP = 0x01;
    unsigned int _STALL_SDCNT_4STEP = 0x02;
    unsigned int _STALL_SDCNT_8STEP = 0x03;
    unsigned int _STALL_VDIV_DIV4 = 0x03;
    unsigned int _STALL_VDIV_DIV8 = 0x02;
    unsigned int _STALL_VDIV_DIV16 = 0x01;
    unsigned int _STALL_VDIV_DIV32 = 0x00;
    unsigned int _STALL_VDIV = _STALL_VDIV_DIV4;

    //0x06 DRIVE Register
private:
    unsigned int _DRIVE_ADDR = 0x06;
    unsigned int _DRIVE_OCPTH_250MV = 0x00;
    unsigned int _DRIVE_OCPTH_500MV = 0x01;
    unsigned int _DRIVE_OCPTH_750MV = 0x02;
    unsigned int _DRIVE_OCPTH_1000MV = 0x03;
    unsigned int _DRIVE_OCPTH = _DRIVE_OCPTH_500MV; //Default
    unsigned int _DRIVE_OCPDEG_1US = 0x00;
    unsigned int _DRIVE_OCPDEG_2US = 0x01;
    unsigned int _DRIVE_OCPDEG_4US = 0x02;
    unsigned int _DRIVE_OCPDEG_8US = 0x03;
    unsigned int _DRIVE_OCPDEG = _DRIVE_OCPDEG_4US; //Default
    unsigned int _DRIVE_TDRIVEN_250NS = 0x00;
    unsigned int _DRIVE_TDRIVEN_500NS = 0x01;
    unsigned int _DRIVE_TDRIVEN_1US = 0x02;
    unsigned int _DRIVE_TDRIVEN_2US = 0x03;
    unsigned int _DRIVE_TDRIVEN = _DRIVE_TDRIVEN_500NS;
    unsigned int _DRIVE_TDRIVEP_250NS = 0x00;
    unsigned int _DRIVE_TDRIVEP_500NS = 0x01;
    unsigned int _DRIVE_TDRIVEP_1US = 0x02;
    unsigned int _DRIVE_TDRIVEP_2US = 0x03;
    unsigned int _DRIVE_TDRIVEP = _DRIVE_TDRIVEP_500NS;
    unsigned int _DRIVE_IDRIVEN_100MA = 0x00;
    unsigned int _DRIVE_IDRIVEN_200MA = 0x01;
    unsigned int _DRIVE_IDRIVEN_300MA = 0x02;
    unsigned int _DRIVE_IDRIVEN_400MA = 0x03;
    unsigned int _DRIVE_IDRIVEN = _DRIVE_IDRIVEN_300MA; //Default
    unsigned int _DRIVE_IDRIVEP_50MA = 0x00;
    unsigned int _DRIVE_IDRIVEP_100MA = 0x01;
    unsigned int _DRIVE_IDRIVEP_150MA = 0x02;
    unsigned int _DRIVE_IDRIVEP_200MA = 0x03;
    unsigned int _DRIVE_IDRIVEP = _DRIVE_IDRIVEP_150MA; //Default

    //0x07 STATUS Register
private:
    unsigned int _STATUS_ADDR = 0x07;
    unsigned int _STATUS_OTS = 0x00;
    unsigned int _STATUS_AOCP = 0x00;
    unsigned int _STATUS_BOCP = 0x00;
    unsigned int _STATUS_APDF = 0x00;
    unsigned int _STATUS_BPDF = 0x00;
    unsigned int _STATUS_UVLO = 0x00;
    unsigned int _STATUS_STD = 0x00;
    unsigned int _STATUS_STDLAT = 0x00;


private:
    int _SCS = 0;
    int _SLEEP = 0;
    float _MTCURRENT = 0;
    float _SENSERESISTOR = 0.03;
    int SPI_delay = 50;
    SPISettings _settings = SPISettings(500000, MSBFIRST, SPI_MODE0);
};

#endif //GFMOTORV2_SPI_DRV8711_H
