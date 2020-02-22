//
// Created by jin on 9/24/19.
//

#ifndef RTOS_ASMC_RTOS_RS485_H
#define RTOS_ASMC_RTOS_RS485_H

#include <Arduino.h>
#include "RS485/RS485HwSerial.h"
#include <Vector.h>
#include <Streaming.h>

#define PACKET_COUNT_MAX 8

class RTOS_RS485 {
public:
    RTOS_RS485(long BPS, int Mega_ID, int TE_PIN);
    void InitializeCommunication();
    void SendSerialPacket(String Packet, bool isConfirmParity);
    String PacketGenerator_485(char ReceiverID, char MotorID, String string_command);
    String PacketGenerator_485(char ReceiverID, char MotorID, char char_command);
    String PacketGenerator_485(char ReceiverID, char MotorID, char char_command, char char_value);
    String PacketGenerator_485(char ReceiverID, char MotorID, char set_command, float float_value);
    String PacketGenerator_485(char ReceiverID, char MotorID, char set_command, String string_value);
    String PacketGenerator_485(char ReceiverID, char MotorID, char set_command1, char set_command2, float float_value);
    String PacketGenerator_485(char ReceiverID, char MotorID, char set_command1, char set_command2, int int_value);
    String PacketGenerator_485(char ReceiverID, char set_command);
    String CheckParityPacket(String expected_echo_packet);
    bool CheckSerialPacket(String strExpectedEcho);
    String EchoGenerator(char sender_id, char motor_id, String string_command);
    String EchoGenerator(char sender_id, char motor_id, char char_command);
    String EchoGenerator(char sender_id, char motor_id, char char_command, char char_value);
    String EchoGenerator(char sender_id, char motor_id, char char_command, float float_value);
    String EchoGenerator(char sender_id, char char_command);
    String EchoGenerator(char sender_id, char motor_id, char set_command1, char set_command2, float float_value);
    String EchoGenerator(char sender_id, char motor_id, char set_command1, char set_command2, int int_value);
    bool ReadPacketsBUS(String desired_packet, bool is_desired_packet_mode, bool ConfirmParityOrCheckSerial);
    String ReadBusSerialPacket();
    String ReadUSBSerialPacket();
    void SerialFlush();
    String ParityGenerator(String Packet);
    bool ParityChecker(String Packet);
    bool ConfirmParity(char receiver_id);
    bool ParityProcessing(String packet_with_parity);
    bool ConfirmParityProcessing(String packet_with_parity);

public:
    String storage_array_0[PACKET_COUNT_MAX];
    String storage_array_1[PACKET_COUNT_MAX];
    String storage_array_2[PACKET_COUNT_MAX];
    String storage_array_3[PACKET_COUNT_MAX];

    String storage_array_parity_0[PACKET_COUNT_MAX];
    String storage_array_parity_1[PACKET_COUNT_MAX];
    String storage_array_parity_2[PACKET_COUNT_MAX];
    String storage_array_parity_3[PACKET_COUNT_MAX];

    Vector<String> packet_storage_mega0;
    Vector<String> packet_storage_mega1;
    Vector<String> packet_storage_mega2;
    Vector<String> packet_storage_mega3;

    Vector<String> parity_storage_mega0;
    Vector<String> parity_storage_mega1;
    Vector<String> parity_storage_mega2;
    Vector<String> parity_storage_mega3;

    String storage_array_process[PACKET_COUNT_MAX];
    Vector<String> process_storage;

    String confirm_parity_echo = "";
    String confirm_parity_echo_wo_parity = "";
    String serial_packet_echo = "";
    String latest_string_result = "";
    String check_serial_echo_packet = "";


protected:
    int _MEGA_TE_PIN = 23; //22
    const int _MEGA_RX_PIN = 19;
    long _BPS = 250000;
    int _mainDelay = 100;
    int _serialDelay = 50;
    int _slaveDelay = 400;
    String _start_delimiter = String('*'); //ASCII 42
    String _end_delimiter = String('/');
    int _Mega_ID = 0;
    char _character_Mega_ID = '\0';
    int _parity_echo_delay = 1000;
    String _latest_sending_packet = "";
    unsigned long resend_interval = 20000;
};


#endif //RTOS_ASMC_RTOS_RS485_H
