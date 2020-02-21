//
// Created by jin on 8/23/19.
//

#include "SPI_DRV8711.h"

SPI_DRV8711::SPI_DRV8711(int SCSPIN, int SLEEPPIN, float motorCurrent, int microstepping) {
    _SCS = SCSPIN;
    _SLEEP = SLEEPPIN;
    _MTCURRENT = motorCurrent;

    // Set Default Register Settings

    // CTRL Register
    G_CTRL_REG.Address 	= _CTRL_ADDR;
    G_CTRL_REG.DTIME 	= _CTRL_DTIME;  //850ns
    G_CTRL_REG.ISGAIN 	= _CTRL_ISGAIN_10;  //Gain of 10
    G_CTRL_REG.EXSTALL 	= _CTRL_EXSTALL_INTERNAL;  //Internal Stall Detect
    G_CTRL_REG.MODE 	= microstepping;  // Full Step
    G_CTRL_REG.RSTEP 	= 0x00;  //No Action
    G_CTRL_REG.RDIR 	= 0x00;  //Direction set by DIR Pin
    G_CTRL_REG.ENBL 	= _CTRL_ENABLE;  //enable motor
    //1000 11 01 0 0011 0 0 1

    //Torque setup based on ISGAIN and Motor Current

    // TORQUE Register
    G_TORQUE_REG.Address = _TORQ_ADDR;
    G_TORQUE_REG.SMPLTH = _TORQ_SMPLTH;  //50uS Back EMF Sample Threshold
    G_TORQUE_REG.TORQUE  = _TORQ_TORQ;  //with ISGAIN set to 10 this gives about 1.5 AMP Current Chopper
    //1000 0 000  01000110

    // OFF Register
    G_OFF_REG.Address 	= _OFF_ADDR;
    G_OFF_REG.PWMMODE 	= _OFF_PWMMODE;  //Internal Indexer
    G_OFF_REG.TOFF 		= _OFF_TOFF;  //Default
    //1000 000 0 00110000

    // BLANK Register
    G_BLANK_REG.Address = _BLANK_ADDR;
    G_BLANK_REG.ABT 	= _BLANK_ABT;  //enable adaptive blanking time
    G_BLANK_REG.TBLANK 	= _BLANK_TBLANK;  //no idea what this should be but the
    //1000 000 1 00001000            //user guide shows it set to this

    // DECAY Register.
    G_DECAY_REG.Address = _DECAY_ADDR;
    G_DECAY_REG.DECMOD  = _DECAY_DECMOD_MIX;  //mixed decay
    G_DECAY_REG.TDECAY 	= _DECAY_TDECAY;  //default
    //1000001100010000

    // STALL Register
    G_STALL_REG.Address = _STALL_ADDR;
    G_STALL_REG.VDIV 	= _STALL_VDIV;  //Back EMF is divided by 4
    G_STALL_REG.SDCNT 	= _STALL_SDCNT;  //stalln asserted after 8 steps
    G_STALL_REG.SDTHR 	= _STALL_SDTHR;  //default
    //1000111101000000

    // DRIVE Register
    G_DRIVE_REG.Address = _DRIVE_ADDR;
    G_DRIVE_REG.IDRIVEP = _DRIVE_IDRIVEP;  //High Side 150mA peak (source)
    G_DRIVE_REG.IDRIVEN = _DRIVE_IDRIVEN;  //Low Side 300mA peak (sink)
    G_DRIVE_REG.TDRIVEP = _DRIVE_TDRIVEP;  //High Side gate drive 500nS
    G_DRIVE_REG.TDRIVEN = _DRIVE_TDRIVEN;  //Low Side Gate Drive 500nS
    G_DRIVE_REG.OCPDEG = _DRIVE_OCPDEG;  //OCP Deglitch Time 4uS
    G_DRIVE_REG.OCPTH = _DRIVE_OCPTH;  //OCP Threshold 500mV
    //1000000001010101

    // STATUS Register
    G_STATUS_REG.Address = _STATUS_ADDR;
    G_STATUS_REG.STDLAT  = _STATUS_STDLAT;
    G_STATUS_REG.STD     = _STATUS_STD;
    G_STATUS_REG.UVLO    = _STATUS_UVLO;
    G_STATUS_REG.BPDF    = _STATUS_BPDF;
    G_STATUS_REG.APDF    = _STATUS_APDF;
    G_STATUS_REG.BOCP    = _STATUS_BOCP;
    G_STATUS_REG.AOCP    = _STATUS_AOCP;
    G_STATUS_REG.OTS     = _STATUS_OTS;

}

void SPI_DRV8711::BootUpSPI(){
    pinMode(_SLEEP, OUTPUT);
    pinMode(_SCS, OUTPUT); //SPI Slave Select
    digitalWrite(_SLEEP, LOW);
    digitalWrite(_SCS, LOW);
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    digitalWrite(_SLEEP, HIGH);
}

void SPI_DRV8711::WriteAllRegisters(){
    unsigned char dataHi = 0x00;
    unsigned char dataLo = 0x00;

    // Write CTRL Register
    dataHi = REGWRITE | (G_CTRL_REG.Address << 4) | (G_CTRL_REG.DTIME << 2) | (G_CTRL_REG.ISGAIN);
    dataLo = (G_CTRL_REG.EXSTALL << 7) | (G_CTRL_REG.MODE << 3) | (G_CTRL_REG.RSTEP << 2) | (G_CTRL_REG.RDIR << 1) | (G_CTRL_REG.ENBL);
    ReadWrite(dataHi, dataLo);

    // Write TORQUE Register
    dataHi = REGWRITE | (G_TORQUE_REG.Address << 4) | (G_TORQUE_REG.SMPLTH);
    dataLo = G_TORQUE_REG.TORQUE;
    ReadWrite(dataHi, dataLo);

    // Write OFF Register
    dataHi = REGWRITE | (G_OFF_REG.Address << 4) | (G_OFF_REG.PWMMODE);
    dataLo = G_OFF_REG.TOFF;
    ReadWrite(dataHi, dataLo);

    // Write BLANK Register
    dataHi = REGWRITE | (G_BLANK_REG.Address << 4) | (G_BLANK_REG.ABT);
    dataLo = G_BLANK_REG.TBLANK;
    ReadWrite(dataHi, dataLo);

    // Write DECAY Register
    dataHi = REGWRITE | (G_DECAY_REG.Address << 4) | (G_DECAY_REG.DECMOD);
    dataLo = G_DECAY_REG.TDECAY;
    ReadWrite(dataHi, dataLo);

    // Write STALL Register
    dataHi = REGWRITE | (G_STALL_REG.Address << 4) | (G_STALL_REG.VDIV << 2) | (G_STALL_REG.SDCNT);
    dataLo = G_STALL_REG.SDTHR;
    ReadWrite(dataHi, dataLo);

    // Write DRIVE Register
    dataHi = REGWRITE | (G_DRIVE_REG.Address << 4) | (G_DRIVE_REG.IDRIVEP << 2) | (G_DRIVE_REG.IDRIVEN);
    dataLo = (G_DRIVE_REG.TDRIVEP << 6) | (G_DRIVE_REG.TDRIVEN << 4) | (G_DRIVE_REG.OCPDEG << 2) | (G_DRIVE_REG.OCPTH);
    ReadWrite(dataHi, dataLo);

    // Write STATUS Register
    dataHi = REGWRITE | (G_STATUS_REG.Address << 4);
    dataLo = (G_STATUS_REG.STDLAT << 7) | (G_STATUS_REG.STD << 6) | (G_STATUS_REG.UVLO << 5) | (G_STATUS_REG.BPDF << 4) | (G_STATUS_REG.APDF << 3) | (G_STATUS_REG.BOCP << 2) | (G_STATUS_REG.AOCP << 1) | (G_STATUS_REG.OTS);
    ReadWrite(dataHi, dataLo);
}


unsigned int SPI_DRV8711::ReadWrite(unsigned int dataHigh, unsigned int dataLow){
    unsigned int readData = 0;
    digitalWrite(_SCS, HIGH);
    SPI.beginTransaction(_settings);

    readData |= (SPI.transfer(dataHigh) << 8);
    readData |= SPI.transfer(dataLow);

    SPI.endTransaction();
    digitalWrite(_SCS, LOW);

    return readData;
}


void SPI_DRV8711::ReadAllRegisters(){
    unsigned char dataHi = 0x00;
    const unsigned char dataLo = 0x00;
    unsigned int readData = 0x00;

    // Read CTRL Register
    dataHi = REGREAD | (G_CTRL_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_CTRL_REG.DTIME        = ((readData >> 10) & 0x0003);
    G_CTRL_REG.ISGAIN       = ((readData >> 8) & 0x0003);
    G_CTRL_REG.EXSTALL      = ((readData >> 7) & 0x0001);
    G_CTRL_REG.MODE         = ((readData >> 3) & 0x000F);
    G_CTRL_REG.RSTEP        = ((readData >> 2) & 0x0001);
    G_CTRL_REG.RDIR         = ((readData >> 1) & 0x0001);
    G_CTRL_REG.ENBL         = ((readData >> 0) & 0x0001);

    Serial.println("Control Register");
    Serial.print("DTIME = ");
    Serial.println(G_CTRL_REG.DTIME, BIN);
    Serial.print("ISGAIN = ");
    Serial.println(G_CTRL_REG.ISGAIN, BIN);
    Serial.print("EXSTALL = ");
    Serial.println(G_CTRL_REG.EXSTALL, BIN);
    Serial.print("MODE = ");
    Serial.println(G_CTRL_REG.MODE, BIN);
    Serial.print("RSTEP = ");
    Serial.println(G_CTRL_REG.RSTEP, BIN);
    Serial.print("RDIR = ");
    Serial.println(G_CTRL_REG.RDIR, BIN);
    Serial.print("ENBL = ");
    Serial.println(G_CTRL_REG.ENBL, BIN);

    // Read TORQUE Register
    dataHi = REGREAD | (G_TORQUE_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_TORQUE_REG.SMPLTH    = ((readData >> 8) & 0x0007);
    G_TORQUE_REG.TORQUE     = ((readData >> 0) & 0x00FF);
    Serial.print("TORQUE = ");
    Serial.println(G_TORQUE_REG.TORQUE, HEX);

    // Read OFF Register
    dataHi = REGREAD | (G_OFF_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_OFF_REG.PWMMODE       = ((readData >> 8) & 0x0001);
    G_OFF_REG.TOFF          = ((readData >> 0) & 0x00FF);

    // Read BLANK Register
    dataHi = REGREAD | (G_BLANK_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_BLANK_REG.ABT         = ((readData >> 8) & 0x0001);
    G_BLANK_REG.TBLANK      = ((readData >> 0) & 0x00FF);

    // Read DECAY Register
    dataHi = REGREAD | (G_DECAY_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_DECAY_REG.DECMOD      = ((readData >> 8) & 0x0007);
    G_DECAY_REG.TDECAY      = ((readData >> 0) & 0x00FF);

    // Read STALL Register
    dataHi = REGREAD | (G_STALL_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_STALL_REG.VDIV        = ((readData >> 10) & 0x0003);
    G_STALL_REG.SDCNT       = ((readData >> 8) & 0x0003);
    G_STALL_REG.SDTHR       = ((readData >> 0) & 0x00FF);

    // Read DRIVE Register
    dataHi = REGREAD | (G_DRIVE_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_DRIVE_REG.IDRIVEP     = ((readData >> 10) & 0x0003);
    G_DRIVE_REG.IDRIVEN     = ((readData >> 8) & 0x0003);
    G_DRIVE_REG.TDRIVEP     = ((readData >> 6) & 0x0003);
    G_DRIVE_REG.TDRIVEN     = ((readData >> 4) & 0x0003);
    G_DRIVE_REG.OCPDEG      = ((readData >> 2) & 0x0003);
    G_DRIVE_REG.OCPTH       = ((readData >> 0) & 0x0003);

    // Read STATUS Register
    dataHi = REGREAD | (G_STATUS_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_STATUS_REG.STDLAT     = ((readData >> 7) & 0x0001);
    G_STATUS_REG.STD        = ((readData >> 6) & 0x0001);
    G_STATUS_REG.UVLO       = ((readData >> 5) & 0x0001);
    G_STATUS_REG.BPDF       = ((readData >> 4) & 0x0001);
    G_STATUS_REG.APDF       = ((readData >> 3) & 0x0001);
    G_STATUS_REG.BOCP       = ((readData >> 2) & 0x0001);
    G_STATUS_REG.AOCP       = ((readData >> 1) & 0x0001);
    G_STATUS_REG.OTS        = ((readData >> 0) & 0x0001);
}

unsigned int SPI_DRV8711::TorqueSetUp(float motorCurrent) {
    unsigned int torqueValue = 0;
    int _ISGain = 10;
    if (G_CTRL_REG.ISGAIN == _CTRL_ISGAIN_5) {
        _ISGain = 5;
    }
    else if (G_CTRL_REG.ISGAIN == _CTRL_ISGAIN_10) {
        _ISGain = 10;
    }
    else if  (G_CTRL_REG.ISGAIN == _CTRL_ISGAIN_20) {
        _ISGain = 20;
    }
    else if  (G_CTRL_REG.ISGAIN == _CTRL_ISGAIN_40) {
        _ISGain = 40;
    }
    torqueValue = (256 * motorCurrent * _ISGain * _SENSERESISTOR) / 2.75;
    return torqueValue;
}

void SPI_DRV8711::Write_CTRL_Disable(int microstepping){
    // CTRL Register
    G_CTRL_REG.Address 	= _CTRL_ADDR; // 0
    delay(SPI_delay);
    G_CTRL_REG.DTIME 	= _CTRL_DTIME;  //850ns, 11
    delay(SPI_delay);
    G_CTRL_REG.ISGAIN 	= _CTRL_ISGAIN_10;  //Gain of 10, 1
    delay(SPI_delay);
    G_CTRL_REG.EXSTALL 	= _CTRL_EXSTALL_INTERNAL;  //Internal Stall Detect, 0
    delay(SPI_delay);
    G_CTRL_REG.MODE 	= microstepping;  // Full Step, 11
    delay(SPI_delay);
    G_CTRL_REG.RSTEP 	= 0x00;  //No Action
    delay(SPI_delay);
    G_CTRL_REG.RDIR 	= 0x00;  //Direction set by DIR Pin
    delay(SPI_delay);
    G_CTRL_REG.ENBL 	= _CTRL_DISABLE;  //enable motor, 1
    //1000 11 01 0 0011 0 0 1

    unsigned char dataHi = 0x00;
    unsigned char dataLo = 0x00;
    Serial.print("ISGAIN : "); Serial.println(G_CTRL_REG.ISGAIN);

    // Write CTRL Register
    dataHi = REGWRITE | (G_CTRL_REG.Address << 4) | (G_CTRL_REG.DTIME << 2) | (G_CTRL_REG.ISGAIN);
    delay(SPI_delay);
    dataLo = (G_CTRL_REG.EXSTALL << 7) | (G_CTRL_REG.MODE << 3) | (G_CTRL_REG.RSTEP << 2) | (G_CTRL_REG.RDIR << 1) | (G_CTRL_REG.ENBL);
    delay(SPI_delay);
    ReadWrite(dataHi, dataLo);
}

void SPI_DRV8711::Write_CTRL_Enable(int microstepping){
    // CTRL Register
    G_CTRL_REG.Address 	= _CTRL_ADDR; // 0
    delay(SPI_delay);
    G_CTRL_REG.DTIME 	= _CTRL_DTIME;  //850ns, 11
    delay(SPI_delay);
    G_CTRL_REG.ISGAIN 	= _CTRL_ISGAIN_10;  //Gain of 10, 1
    delay(SPI_delay);
    G_CTRL_REG.EXSTALL 	= _CTRL_EXSTALL_INTERNAL;  //Internal Stall Detect, 0
    delay(SPI_delay);
    G_CTRL_REG.MODE 	= microstepping;  // Full Step, 11
    delay(SPI_delay);
    G_CTRL_REG.RSTEP 	= 0x00;  //No Action
    delay(SPI_delay);
    G_CTRL_REG.RDIR 	= 0x00;  //Direction set by DIR Pin
    delay(SPI_delay);
    G_CTRL_REG.ENBL 	= _CTRL_ENABLE;  //enable motor, 1
    //1000 11 01 0 0011 0 0 1

    unsigned char dataHi = 0x00;
    unsigned char dataLo = 0x00;

    // Write CTRL Register
    dataHi = REGWRITE | (G_CTRL_REG.Address << 4) | (G_CTRL_REG.DTIME << 2) | (G_CTRL_REG.ISGAIN);
    delay(SPI_delay);
    dataLo = (G_CTRL_REG.EXSTALL << 7) | (G_CTRL_REG.MODE << 3) | (G_CTRL_REG.RSTEP << 2) | (G_CTRL_REG.RDIR << 1) | (G_CTRL_REG.ENBL);
    delay(SPI_delay);
    ReadWrite(dataHi, dataLo);
}

void SPI_DRV8711::Write_TORQUE(float motor_current){
    // TORQUE Register
    Serial.print("Current : "); Serial.println(motor_current);
    _MTCURRENT = motor_current;
    _TORQ_TORQ = TorqueSetUp(_MTCURRENT);
    Serial.print("Torque : "); Serial.println(_TORQ_TORQ);
    delay(SPI_delay);
    G_TORQUE_REG.Address = _TORQ_ADDR;
    delay(SPI_delay);
    G_TORQUE_REG.SMPLTH = _TORQ_SMPLTH;  //50uS Back EMF Sample Threshold
    delay(SPI_delay);
    G_TORQUE_REG.TORQUE  = _TORQ_TORQ;  //with ISGAIN set to 10 this gives about 1.5 AMP Current Chopper
    //1000 0 000  01000110
    delay(SPI_delay);
    unsigned char dataHi = 0x00;
    unsigned char dataLo = 0x00;

    // Write TORQUE Register
    dataHi = REGWRITE | (G_TORQUE_REG.Address << 4) | (G_TORQUE_REG.SMPLTH);
    delay(SPI_delay);
    dataLo = G_TORQUE_REG.TORQUE;
    delay(SPI_delay);
    ReadWrite(dataHi, dataLo);
}

void SPI_DRV8711::Write_OFF(){
    // OFF Register
    G_OFF_REG.Address 	= _OFF_ADDR;
    G_OFF_REG.PWMMODE 	= _OFF_PWMMODE;  //Internal Indexer
    G_OFF_REG.TOFF 		= _OFF_TOFF;  //Default
    //1000 000 0 00110000
    unsigned char dataHi = 0x00;
    unsigned char dataLo = 0x00;

    // Write OFF Register
    dataHi = REGWRITE | (G_OFF_REG.Address << 4) | (G_OFF_REG.PWMMODE);
    dataLo = G_OFF_REG.TOFF;
    ReadWrite(dataHi, dataLo);

}

void SPI_DRV8711::Write_BLANK(){
    // BLANK Register
    G_BLANK_REG.Address = _BLANK_ADDR;
    G_BLANK_REG.ABT 	= _BLANK_ABT;  //enable adaptive blanking time
    G_BLANK_REG.TBLANK 	= _BLANK_TBLANK;  //no idea what this should be but the
    //1000 000 1 00001000            //user guide shows it set to this
    unsigned char dataHi = 0x00;
    unsigned char dataLo = 0x00;

    // Write BLANK Register
    dataHi = REGWRITE | (G_BLANK_REG.Address << 4) | (G_BLANK_REG.ABT);
    dataLo = G_BLANK_REG.TBLANK;
    ReadWrite(dataHi, dataLo);

}

void SPI_DRV8711::Write_DECAY(){
    // DECAY Register.
    G_DECAY_REG.Address = _DECAY_ADDR;
    G_DECAY_REG.DECMOD  = _DECAY_DECMOD_MIX;  //mixed decay
    G_DECAY_REG.TDECAY 	= _DECAY_TDECAY;  //default
    //1000001100010000
    unsigned char dataHi = 0x00;
    unsigned char dataLo = 0x00;

    // Write DECAY Register
    dataHi = REGWRITE | (G_DECAY_REG.Address << 4) | (G_DECAY_REG.DECMOD);
    dataLo = G_DECAY_REG.TDECAY;
    ReadWrite(dataHi, dataLo);

}

void SPI_DRV8711::Write_STALL(){
    // STALL Register
    G_STALL_REG.Address = _STALL_ADDR;
    G_STALL_REG.VDIV 	= _STALL_VDIV;  //Back EMF is divided by 4
    G_STALL_REG.SDCNT 	= _STALL_SDCNT;  //stalln asserted after 8 steps
    G_STALL_REG.SDTHR 	= _STALL_SDTHR;  //default
    //1000111101000000
    unsigned char dataHi = 0x00;
    unsigned char dataLo = 0x00;

    // Write STALL Register
    dataHi = REGWRITE | (G_STALL_REG.Address << 4) | (G_STALL_REG.VDIV << 2) | (G_STALL_REG.SDCNT);
    dataLo = G_STALL_REG.SDTHR;
    ReadWrite(dataHi, dataLo);

}

void SPI_DRV8711::Write_DRIVE(){
    // DRIVE Register
    G_DRIVE_REG.Address = _DRIVE_ADDR;
    G_DRIVE_REG.IDRIVEP = _DRIVE_IDRIVEP;  //High Side 150mA peak (source)
    G_DRIVE_REG.IDRIVEN = _DRIVE_IDRIVEN;  //Low Side 300mA peak (sink)
    G_DRIVE_REG.TDRIVEP = _DRIVE_TDRIVEP;  //High Side gate drive 500nS
    G_DRIVE_REG.TDRIVEN = _DRIVE_TDRIVEN;  //Low Side Gate Drive 500nS
    G_DRIVE_REG.OCPDEG = _DRIVE_OCPDEG;  //OCP Deglitch Time 4uS
    G_DRIVE_REG.OCPTH = _DRIVE_OCPTH;  //OCP Threshold 500mV
    //1000000001010101
    unsigned char dataHi = 0x00;
    unsigned char dataLo = 0x00;

    // Write DRIVE Register
    dataHi = REGWRITE | (G_DRIVE_REG.Address << 4) | (G_DRIVE_REG.IDRIVEP << 2) | (G_DRIVE_REG.IDRIVEN);
    dataLo = (G_DRIVE_REG.TDRIVEP << 6) | (G_DRIVE_REG.TDRIVEN << 4) | (G_DRIVE_REG.OCPDEG << 2) | (G_DRIVE_REG.OCPTH);
    ReadWrite(dataHi, dataLo);

}

void SPI_DRV8711::Write_STATUS(){
    // STATUS Register
    G_STATUS_REG.Address = _STATUS_ADDR;
    G_STATUS_REG.STDLAT  = _STATUS_STDLAT;
    G_STATUS_REG.STD     = _STATUS_STD;
    G_STATUS_REG.UVLO    = _STATUS_UVLO;
    G_STATUS_REG.BPDF    = _STATUS_BPDF;
    G_STATUS_REG.APDF    = _STATUS_APDF;
    G_STATUS_REG.BOCP    = _STATUS_BOCP;
    G_STATUS_REG.AOCP    = _STATUS_AOCP;
    G_STATUS_REG.OTS     = _STATUS_OTS;

    unsigned char dataHi = 0x00;
    unsigned char dataLo = 0x00;

    // Write STATUS Register
    dataHi = REGWRITE | (G_STATUS_REG.Address << 4);
    dataLo = (G_STATUS_REG.STDLAT << 7) | (G_STATUS_REG.STD << 6) | (G_STATUS_REG.UVLO << 5) | (G_STATUS_REG.BPDF << 4) | (G_STATUS_REG.APDF << 3) | (G_STATUS_REG.BOCP << 2) | (G_STATUS_REG.AOCP << 1) | (G_STATUS_REG.OTS);
    ReadWrite(dataHi, dataLo);
}

void SPI_DRV8711::Read_CTRL() {
    unsigned char dataHi = 0x00;
    const unsigned char dataLo = 0x00;
    unsigned int readData = 0x00;

    // Read CTRL Register
    dataHi = REGREAD | (G_CTRL_REG.Address << 4);
    delay(SPI_delay);
    readData = ReadWrite(dataHi, dataLo);
    delay(SPI_delay);
    Serial.println(readData, BIN);
    delay(SPI_delay);
    G_CTRL_REG.DTIME        = ((readData >> 10) & 0x0003);
    delay(SPI_delay);
    G_CTRL_REG.ISGAIN       = ((readData >> 8) & 0x0003);
    delay(SPI_delay);
    G_CTRL_REG.EXSTALL      = ((readData >> 7) & 0x0001);
    delay(SPI_delay);
    G_CTRL_REG.MODE         = ((readData >> 3) & 0x000F);
    delay(SPI_delay);
    G_CTRL_REG.RSTEP        = ((readData >> 2) & 0x0001);
    delay(SPI_delay);
    G_CTRL_REG.RDIR         = ((readData >> 1) & 0x0001);
    delay(SPI_delay);
    G_CTRL_REG.ENBL         = ((readData >> 0) & 0x0001);
    delay(SPI_delay);
    Serial.println("Control Register");
    Serial.print("DTIME = ");
    Serial.println(G_CTRL_REG.DTIME, BIN);
    delay(SPI_delay);
    Serial.print("ISGAIN = ");
    Serial.println(G_CTRL_REG.ISGAIN, BIN);
    delay(SPI_delay);
    Serial.print("EXSTALL = ");
    Serial.println(G_CTRL_REG.EXSTALL, BIN);
    delay(SPI_delay);
    Serial.print("MODE = ");
    Serial.println(G_CTRL_REG.MODE, BIN);
    delay(SPI_delay);
    Serial.print("RSTEP = ");
    Serial.println(G_CTRL_REG.RSTEP, BIN);
    delay(SPI_delay);
    Serial.print("RDIR = ");
    Serial.println(G_CTRL_REG.RDIR, BIN);
    delay(SPI_delay);
    Serial.print("ENBL = ");
    Serial.println(G_CTRL_REG.ENBL, BIN);
    delay(SPI_delay);
}

void SPI_DRV8711::Read_TORQUE() {
    unsigned char dataHi = 0x00;
    const unsigned char dataLo = 0x00;
    unsigned int readData = 0x00;

    // Read TORQUE Register
    dataHi = REGREAD | (G_TORQUE_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_TORQUE_REG.SMPLTH    = ((readData >> 8) & 0x0007);
    G_TORQUE_REG.TORQUE     = ((readData >> 0) & 0x00FF);
    Serial.print("TORQUE = ");
    Serial.println(G_TORQUE_REG.TORQUE, HEX);
}

void SPI_DRV8711::Read_BLANK() {
    unsigned char dataHi = 0x00;
    const unsigned char dataLo = 0x00;
    unsigned int readData = 0x00;

    // Read BLANK Register
    dataHi = REGREAD | (G_BLANK_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_BLANK_REG.ABT         = ((readData >> 8) & 0x0001);
    G_BLANK_REG.TBLANK      = ((readData >> 0) & 0x00FF);
}


void SPI_DRV8711::Read_DECAY() {
    unsigned char dataHi = 0x00;
    const unsigned char dataLo = 0x00;
    unsigned int readData = 0x00;

    // Read DECAY Register
    dataHi = REGREAD | (G_DECAY_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_DECAY_REG.DECMOD      = ((readData >> 8) & 0x0007);
    G_DECAY_REG.TDECAY      = ((readData >> 0) & 0x00FF);
}



void SPI_DRV8711::Read_STALL() {
    unsigned char dataHi = 0x00;
    const unsigned char dataLo = 0x00;
    unsigned int readData = 0x00;

    // Read STALL Register
    dataHi = REGREAD | (G_STALL_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_STALL_REG.VDIV        = ((readData >> 10) & 0x0003);
    G_STALL_REG.SDCNT       = ((readData >> 8) & 0x0003);
    G_STALL_REG.SDTHR       = ((readData >> 0) & 0x00FF);

}

void SPI_DRV8711::Read_DRIVE() {
    unsigned char dataHi = 0x00;
    const unsigned char dataLo = 0x00;
    unsigned int readData = 0x00;


    // Read DRIVE Register
    dataHi = REGREAD | (G_DRIVE_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_DRIVE_REG.IDRIVEP     = ((readData >> 10) & 0x0003);
    G_DRIVE_REG.IDRIVEN     = ((readData >> 8) & 0x0003);
    G_DRIVE_REG.TDRIVEP     = ((readData >> 6) & 0x0003);
    G_DRIVE_REG.TDRIVEN     = ((readData >> 4) & 0x0003);
    G_DRIVE_REG.OCPDEG      = ((readData >> 2) & 0x0003);
    G_DRIVE_REG.OCPTH       = ((readData >> 0) & 0x0003);

    // Read STATUS Register
    dataHi = REGREAD | (G_STATUS_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_STATUS_REG.STDLAT     = ((readData >> 7) & 0x0001);
    G_STATUS_REG.STD        = ((readData >> 6) & 0x0001);
    G_STATUS_REG.UVLO       = ((readData >> 5) & 0x0001);
    G_STATUS_REG.BPDF       = ((readData >> 4) & 0x0001);
    G_STATUS_REG.APDF       = ((readData >> 3) & 0x0001);
    G_STATUS_REG.BOCP       = ((readData >> 2) & 0x0001);
    G_STATUS_REG.AOCP       = ((readData >> 1) & 0x0001);
    G_STATUS_REG.OTS        = ((readData >> 0) & 0x0001);
}

void SPI_DRV8711::Read_STATUS() {
    unsigned char dataHi = 0x00;
    const unsigned char dataLo = 0x00;
    unsigned int readData = 0x00;

    // Read STATUS Register
    dataHi = REGREAD | (G_STATUS_REG.Address << 4);
    readData = ReadWrite(dataHi, dataLo);
    Serial.println(readData, BIN);
    G_STATUS_REG.STDLAT     = ((readData >> 7) & 0x0001);
    G_STATUS_REG.STD        = ((readData >> 6) & 0x0001);
    G_STATUS_REG.UVLO       = ((readData >> 5) & 0x0001);
    G_STATUS_REG.BPDF       = ((readData >> 4) & 0x0001);
    G_STATUS_REG.APDF       = ((readData >> 3) & 0x0001);
    G_STATUS_REG.BOCP       = ((readData >> 2) & 0x0001);
    G_STATUS_REG.AOCP       = ((readData >> 1) & 0x0001);
    G_STATUS_REG.OTS        = ((readData >> 0) & 0x0001);
}
