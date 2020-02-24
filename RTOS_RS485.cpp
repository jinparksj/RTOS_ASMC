//
// Created by jin on 9/24/19.
//

#include "RTOS_RS485.h"

RTOS_RS485::RTOS_RS485(long BPS, int Mega_ID, int TE_PIN) {
    _MEGA_TE_PIN = TE_PIN;
    _BPS = BPS;
    _Mega_ID = Mega_ID;
    _character_Mega_ID = char(_Mega_ID + 48); //Ascii value : 48 == '0'

    packet_storage_mega0.setStorage(storage_array_0);
    packet_storage_mega1.setStorage(storage_array_1);
    packet_storage_mega2.setStorage(storage_array_2);
    packet_storage_mega3.setStorage(storage_array_3);

    parity_storage_mega0.setStorage(storage_array_parity_0);
    parity_storage_mega1.setStorage(storage_array_parity_1);
    parity_storage_mega2.setStorage(storage_array_parity_2);
    parity_storage_mega3.setStorage(storage_array_parity_3);

    process_storage.setStorage(storage_array_process);
}

void RTOS_RS485::InitializeCommunication() {
    Serial.begin(115200);
    while(!Serial && (millis() < 10000));

    RS485HwSerial1.transmitterEnable(_MEGA_TE_PIN);
    RS485HwSerial1.begin(_BPS);
    while(!RS485HwSerial1);
    pinMode(_MEGA_RX_PIN, INPUT_PULLUP);
    Serial.println("%Arduino Mega RTOS RS-485 Boot Up completed");
}

void RTOS_RS485::SendSerialPacket(String Packet, bool isConfirmParity = true) {
    String parity_packet = ParityGenerator(Packet);
    Packet = Packet + parity_packet + _end_delimiter;
    _latest_sending_packet = Packet;
    Serial.print("SendSerialPacket : "); Serial.println(_latest_sending_packet);
    RS485HwSerial1.print(Packet);
    delay(_mainDelay);
    if (isConfirmParity) {
        while(!ConfirmParity(Packet[1])) {
            Serial.print("%Confirm Parity Echo... : "); Serial.println(Packet);
            delay(1000);
        }
    }
}

//RS-485
String RTOS_RS485::PacketGenerator_485(char ReceiverID, char MotorID, String string_command) {
    String stringPacket = _start_delimiter + String(ReceiverID) + _start_delimiter + String(_Mega_ID) + String(MotorID) + string_command;
    return stringPacket;
}

String RTOS_RS485::PacketGenerator_485(char ReceiverID, char MotorID, char char_command) {
    String stringPacket = _start_delimiter + String(ReceiverID) + _start_delimiter + String(_Mega_ID) + String(MotorID) + String(char_command);
    return stringPacket;
}

String RTOS_RS485::PacketGenerator_485(char ReceiverID, char MotorID, char char_command, char char_value) {
    String stringPacket = _start_delimiter + String(ReceiverID) + _start_delimiter + String(_Mega_ID) + String(MotorID) + String(char_command) + String(char_value);
    return stringPacket;
}

String RTOS_RS485::PacketGenerator_485(char ReceiverID, char MotorID, char set_command, float float_value) {
    String stringPacket = _start_delimiter + String(ReceiverID) + _start_delimiter + String(_Mega_ID) + String(MotorID) + String(set_command) + String(float_value);
    return stringPacket;
}

String RTOS_RS485::PacketGenerator_485(char ReceiverID, char MotorID, char set_command, String string_value) {
    String stringPacket = _start_delimiter + String(ReceiverID) + _start_delimiter + String(_Mega_ID) + String(MotorID) + String(set_command) + string_value;
    return stringPacket;
}

String RTOS_RS485::PacketGenerator_485(char ReceiverID, char MotorID, char set_command1, char set_command2, float float_value) {
    String stringPacket = _start_delimiter + String(ReceiverID) + _start_delimiter + String(_Mega_ID) + String(MotorID) + String(set_command1) + String(set_command2) + String(float_value);
    return stringPacket;
}

String RTOS_RS485::PacketGenerator_485(char ReceiverID, char MotorID, char set_command1, char set_command2, int int_value) {
    String stringPacket = _start_delimiter + String(ReceiverID) + _start_delimiter + String(_Mega_ID) + String(MotorID) + String(set_command1) + String(set_command2) + String(int_value);
    return stringPacket;
}
String RTOS_RS485::PacketGenerator_485(char ReceiverID, char set_command) {
    String stringPacket = _start_delimiter + String(ReceiverID) + _start_delimiter + String(_Mega_ID) + String(set_command);
    return stringPacket;
}

bool RTOS_RS485::ConfirmParityProcessing(String packet_with_parity) {
    if (ParityChecker(packet_with_parity)) {
        Serial.print("ConfirmParityProcessing - Parity is Good : "); Serial.println(packet_with_parity);
        delay(100);
        return true;
    }
    else {
        Serial.print("ConfirmParityProcessing - Parity is Bad : "); Serial.println(packet_with_parity);
        delay(100);
        return false;
    }
}

bool RTOS_RS485::ReadPacketsBUS(String desired_packet = "", bool is_desired_packet_mode = false, bool ConfirmParityOrCheckSerial = true) {
    // ConfirmParityOrCheckSerial: true - confirm parity / false - check serial packet
    int temp_int_read_return = 0;
    String bus_packet = "";
    char receiver_id  = '\0';
    int count_empty = 0;

    unsigned long start_time = millis();
    unsigned long interval_echo = 5000;
    bool is_return_true = false;
    //////////MAKE IT GLOBAL???? _latest_coming_packet
    String _latest_coming_packet = ""; //Protect duplicated conditions.
    char expected_sender_id = desired_packet[3];

    while(RS485HwSerial1.available() == 0) {
        if (ConfirmParityOrCheckSerial) {
            if ((millis() - start_time) > interval_echo) {
                Serial.print("ReadPacketsBUS/Confirm Parity - Resend Again : ");
                Serial.println(_latest_sending_packet);
                RS485HwSerial1.print(_latest_sending_packet);
                delay(_mainDelay);
                start_time = millis();
            }
        }
        else {
            if ((millis() - start_time) > (interval_echo)) {
                if(expected_sender_id == '0') {
                    if (!packet_storage_mega0.empty()) {
                        Serial.print("Not Wait - Break for Next : "); Serial.println(expected_sender_id);
                        break;
                    }
                }
                else if(expected_sender_id == '1') {
                    if (!packet_storage_mega1.empty()) {
                        Serial.print("Not Wait - Break for Next : "); Serial.println(expected_sender_id);
                        break;
                    }
                }
                else if(expected_sender_id == '2') {
                    if (!packet_storage_mega2.empty()) {
                        Serial.print("Not Wait - Break for Next : "); Serial.println(expected_sender_id);
                        break;
                    }
                }
                else if(expected_sender_id == '3') {
                    if (!packet_storage_mega3.empty()) {
                        Serial.print("Not Wait - Break for Next : "); Serial.println(expected_sender_id);
                        break;
                    }
                }
            }
        }
        if (Serial.available() > 0) {
            Serial.println("!!! USB Mode");
            return true;
        }
    }

    while (RS485HwSerial1.available() > 0){
        delay(200);
        bus_packet = "";
        Serial.println("ReadPacketsBUS/Startpoint");        
        while (1) {
            temp_int_read_return = RS485HwSerial1.read(); //return integer - ascii number
            delay(10);
            if (temp_int_read_return == 47) {
                if (bus_packet.length() < 4) {
                    continue;
                }
                else {
                    break;
                }           
            }
            else if (temp_int_read_return == -1) {
                count_empty++;
                if (count_empty > 15) {
                    bus_packet = "";
                    break;
                }
                else {
                    continue; 
                }
                       
            }
            else if (temp_int_read_return <= 32) {
                if (bus_packet.length() < 4) {
                    continue;
                }
                else {
                    break;
                }   
            }
            else {
                bus_packet += String(char(temp_int_read_return));
                count_empty = 0;
            }
        }
        Serial.print("ReadPacketsBUS - "); Serial.println(bus_packet);

        delay(20);
        char first_packet_integer = bus_packet[0];
        Serial.print("ReadPacketsBUS - first_packet_integer : "); Serial.println(int(first_packet_integer));
        int id_packet = bus_packet.substring(1, 2).toInt();
        char sender_packet = bus_packet[3];
        char parity_confirm_packet = bus_packet[4];

        delay(20);
        
        //0. bus_packet == "", break
        if (bus_packet == "") {
            Serial.print("ReadPacketsBUS - no bus_packet : "); Serial.println(bus_packet);
            break;
        }

        //1. bus_packet length is less than 5, it is
        if (bus_packet[0] != '*' || bus_packet[2] != '*' || bus_packet.length() < 8) {
            Serial.print("ReadPacketsBUS - Corrupted Data : "); Serial.println(bus_packet);
        }

        //2. bus_packet corruption case 1
        if (first_packet_integer == -1) {
            Serial.print("ReadPacketsBUS - Corrupted Data (-1) : "); Serial.println(bus_packet);
            bus_packet = "";
            break;
        }

        //3. bus_packet corruption case 2
        if (first_packet_integer <= 32) {
            Serial.print("ReadPacketsBUS - Null or Line Space, Carriage Return : "); Serial.println(bus_packet);
            bus_packet = "";
            break;
        }

        //4. / case
        if (first_packet_integer == 47) {
            Serial.print("ReadPacketsBUS - Delimiter '/' : "); Serial.println(bus_packet);
            bus_packet = "";
            break;
        }

        //5. not my packet
        if (id_packet != _Mega_ID) {
            Serial.print("ReadPacketsBUS - This Serial Packet is not belong to this device : "); Serial.println(bus_packet);
            bus_packet = "";            
            continue;
        }

        //6. Protect Duplicated Serial Packet Case
        if (_latest_coming_packet == bus_packet) {
            Serial.print("ReadPacketsBUS - Duplicated Case : "); Serial.print(_latest_coming_packet); Serial.print(" : "); Serial.println(bus_packet);
            bus_packet = "";
            continue;
        }

        //7. Any '-s-', '-e-', '-ID-' come in, do parity processing!
        if ((bus_packet[4] == 's' || bus_packet[4] == _character_Mega_ID || bus_packet[4] == 'e' || bus_packet[4] == 't' || bus_packet[4] == 'd')) {
            if(!ParityProcessing(bus_packet)){
                Serial.print("ReadPacketsBUS - Parity Packet Error : "); Serial.println(bus_packet);
                bus_packet = "";
                continue;
            }
            if (bus_packet[4] != 'e') {
                Serial.print("ReadPacketsBUS - Save in process_storage : "); Serial.println(bus_packet);
                process_storage.push_back(bus_packet);
            }
            Serial.print("ReadPacketsBUS - Process Stroage : ");
            Serial << process_storage << endl;
        }

        //8. Protect falling into infinite loop due to communication corrupted error
        if (ConfirmParityOrCheckSerial && is_desired_packet_mode){
            if(!ConfirmParityProcessing(bus_packet)) {
                bus_packet = desired_packet;
                bus_packet = bus_packet + ParityGenerator(bus_packet);
                first_packet_integer = bus_packet[0];
                id_packet = bus_packet[1];
                sender_packet = bus_packet[3];
                parity_confirm_packet = bus_packet[4];
            }
        }
        delay(20);

        String bus_packet_wo_parity = bus_packet.substring(0, bus_packet.length() - 3);
        //8. Distribution to Parity and Packet Storages
        //  1) sender ID
        switch (sender_packet) {
            case '0':
                //  2)  packet_storage_mega or parity_storage_mega
                if (parity_confirm_packet == 'O') {
                    parity_storage_mega0.push_back(bus_packet);
                }
                else if (parity_confirm_packet == 'N') {
                    //  3) N case: resend latest sending packet and wait 0.5S, Go To the beginning of while loop
                    Serial.print("ReadPacketsBUS/Confirm Parity - N case - Resend Again : "); Serial.println(bus_packet);
                    RS485HwSerial1.print(_latest_sending_packet);
                    delay(500);
                    continue;//////TOMORROW CHECK IT WORKS??!!
                }
                else {
                    //  4) Check Serial Packet case
                    if (bus_packet_wo_parity != desired_packet) {
                        packet_storage_mega0.push_back(bus_packet);
                    }
                }
                break;
            case '1':
                //  2)  packet_storage_mega or parity_storage_mega
                if (parity_confirm_packet == 'O') {
                    parity_storage_mega1.push_back(bus_packet);
                }
                else if (parity_confirm_packet == 'N') {
                    //  3) N case: resend latest sending packet and wait 0.5S, Go To the beginning of while loop
                    Serial.print("ReadPacketsBUS/Confirm Parity - N case - Resend Again : "); Serial.println(bus_packet);
                    RS485HwSerial1.print(_latest_sending_packet);
                    delay(500);
                    continue;//////TOMORROW CHECK IT WORKS??!!
                }
                    //  4) Check Serial Packet case
                else {
                    if (bus_packet_wo_parity != desired_packet) {
                        packet_storage_mega1.push_back(bus_packet);
                    }
                }
                break;
            case '2':
                if (parity_confirm_packet == 'O') {
                    parity_storage_mega2.push_back(bus_packet);
                }
                else if (parity_confirm_packet == 'N') {
                    Serial.print("ReadPacketsBUS/Confirm Parity - N case - Resend Again : "); Serial.println(bus_packet);
                    RS485HwSerial1.print(_latest_sending_packet);
                    delay(500);
                    continue;
                }
                else {
                    if (bus_packet_wo_parity != desired_packet) {
                        packet_storage_mega2.push_back(bus_packet);
                    }
                }
                break;
            case '3':
                if (parity_confirm_packet == 'O') {
                    parity_storage_mega3.push_back(bus_packet);
                }
                else if (parity_confirm_packet == 'N') {
                    Serial.print("ReadPacketsBUS/Confirm Parity - N case - Resend Again : "); Serial.println(bus_packet);
                    RS485HwSerial1.print(_latest_sending_packet);
                    delay(500);
                    continue;
                }
                else {
                    if (bus_packet_wo_parity != desired_packet) {
                        packet_storage_mega3.push_back(bus_packet);
                    }
                }
                break;
        }

        //9. Print bus serial packets
        if ((parity_confirm_packet == 'O') || (parity_confirm_packet == 'N')) {
            Serial.print("$$$ ReadPacketsBUS - Parity Packet : "); Serial.println(bus_packet);
        }
        else {
            Serial.print("$$$ ReadPacketsBUS - Serial Packet : "); Serial.println(bus_packet);
        }
        
        //10. Compare desired packets, is_return_true
        if (is_desired_packet_mode) {
            Serial.print("$$$ ReadPacketsBUS - is_desired_packet_mode : "); Serial.println(desired_packet);
            if(desired_packet == bus_packet_wo_parity) {
                Serial.print("ReadPacketsBUS - It is desired packet : "); Serial.println(desired_packet);
                is_return_true |= true;
                if (!ConfirmParityOrCheckSerial) {
                    check_serial_echo_packet = bus_packet_wo_parity;
                }
            }
            else {

                is_return_true |= false;
            }
        }
        else {
            Serial.print("$$$ ReadPacketsBUS - Non-is_desired_packet_mode : ");
            is_return_true = true;
        }
        _latest_coming_packet = bus_packet;
    }


    //11. ConfirmParity Case: There is a case that check serial echo comes in earlier than start of check serial packet function
    //Need to check whether it is in packet_storage
    //It is activated, when is_return_true is false
    if (!ConfirmParityOrCheckSerial && !is_return_true) {
        String packet_without_parity = "";
        bool isExisting = false;
        switch(expected_sender_id) {
            case '0':
                if (packet_storage_mega0.empty()) {
                    Serial.print("ReadPacketsBUS - CheckSerialPacket - Empty, 0, Return False : ");
                    return false;
                }
                else {
                    for (int i = 0; i < packet_storage_mega0.size(); i++) {
                        packet_without_parity = packet_storage_mega0.at(i);
                        packet_without_parity = packet_without_parity.substring(0, packet_without_parity.length() - 3);
                        if (packet_without_parity == desired_packet) {
                            Serial.print("ReadPacketsBUS - CheckSerialPacket - there is a same packet : ");
                            Serial.println(packet_without_parity);
                            Serial << packet_storage_mega0 << endl;
                            packet_storage_mega0.remove(i);
                            isExisting = true;
                            break;
                        }
                    }
                }
                break;
            case '1':
                if (packet_storage_mega1.empty()) {
                    Serial.print("ReadPacketsBUS - CheckSerialPacket - Empty, 1, Return False : ");
                    return false;
                }
                else {
                    for (int i = 0; i < packet_storage_mega1.size(); i++) {
                        packet_without_parity = packet_storage_mega1.at(i);
                        packet_without_parity = packet_without_parity.substring(0, packet_without_parity.length() - 3);
                        if (packet_without_parity == desired_packet) {
                            Serial.print("ReadPacketsBUS - CheckSerialPacket - there is a same packet : ");
                            Serial.println(packet_without_parity);
                            Serial << packet_storage_mega1 << endl;
                            packet_storage_mega1.remove(i);
                            isExisting = true;
                            break;
                        }
                    }
                }
                break;
            case '2':
                if (packet_storage_mega2.empty()) {
                    Serial.print("ReadPacketsBUS - CheckSerialPacket - Empty, 2, Return False : ");
                    return false;
                }
                else {
                    for (int i = 0; i < packet_storage_mega2.size(); i++) {
                        packet_without_parity = packet_storage_mega2.at(i);
                        packet_without_parity = packet_without_parity.substring(0, packet_without_parity.length() - 3);
                        if (packet_without_parity == desired_packet) {
                            Serial.print("ReadPacketsBUS - CheckSerialPacket - there is a same packet : ");
                            Serial.println(packet_without_parity);
                            Serial << packet_storage_mega2 << endl;
                            packet_storage_mega2.remove(i);
                            isExisting = true;
                            break;
                        }
                    }
                }
                break;
            case '3':
                if (packet_storage_mega3.empty()) {
                    Serial.print("ReadPacketsBUS - CheckSerialPacket - Empty, 3, Return False : ");
                    return false;
                }
                else {
                    for (int i = 0; i < packet_storage_mega3.size(); i++) {
                        packet_without_parity = packet_storage_mega3.at(i);
                        packet_without_parity = packet_without_parity.substring(0, packet_without_parity.length() - 3);
                        if (packet_without_parity == desired_packet) {
                            Serial.print("ReadPacketsBUS - CheckSerialPacket - there is a same packet : ");
                            Serial.println(packet_without_parity);
                            Serial << packet_storage_mega3 << endl;
                            packet_storage_mega3.remove(i);
                            isExisting = true;
                            break;
                        }
                    }
                }

                break;
        }
        if (isExisting) {
            check_serial_echo_packet = packet_without_parity;
            is_return_true = true;
        }
    }

    //12. If it is not desired packet mode, it always return true. If it is, it returns is_return_true
    if (is_desired_packet_mode) {
        return is_return_true;
    }
    else {
        return true;
    }
}

String RTOS_RS485::ReadBusSerialPacket() {
    Serial.println("ReadBusSerialPacket");
    String temp_process_packet = "";
    if (process_storage.empty()) {
        while(!ReadPacketsBUS("", false, false));
        if (process_storage.empty()) {
            Serial.println("ReadBusSerialPacket - Empty - Return """);
            return "";
        }
        else {
            temp_process_packet = process_storage.at(0);
            Serial.print("ReadBusSerialPacket - After ReadPacketsSlave Bus, Not Empty : "); Serial.println(temp_process_packet);
            process_storage.remove(0);
        }
    }
    else {
        temp_process_packet = process_storage.at(0);
        Serial.print("ReadBusSerialPacket - Not Empty : "); Serial.println(temp_process_packet);
        process_storage.remove(0);
    }


    String temp_process_packet_wo_parity = temp_process_packet.substring(0, temp_process_packet.length() - 3);
    switch(temp_process_packet[3]) {
        case '0':
            for (int i = 0; i < packet_storage_mega0.size(); i++) {
                if (packet_storage_mega0.at(i) == temp_process_packet) {
                    Serial.print("ReadBusSerialPacket - there is a same packet : ");
                    Serial.println(temp_process_packet);
                    Serial << packet_storage_mega0 << endl;
                    packet_storage_mega0.remove(i);
                    break;
                }
            }
            break;
        case '1':
            for (int i = 0; i < packet_storage_mega1.size(); i++) {
                if (packet_storage_mega1.at(i) == temp_process_packet) {
                    Serial.print("ReadBusSerialPacket - there is a same packet : ");
                    Serial.println(temp_process_packet);
                    Serial << packet_storage_mega1 << endl;
                    packet_storage_mega1.remove(i);
                    break;
                }
            }
            break;
        case '2':
            for (int i = 0; i < packet_storage_mega2.size(); i++) {
                if (packet_storage_mega2.at(i) == temp_process_packet) {
                    Serial.print("ReadBusSerialPacket - there is a same packet : ");
                    Serial.println(temp_process_packet);
                    Serial << packet_storage_mega2 << endl;
                    packet_storage_mega2.remove(i);
                    break;
                }
            }
            break;
        case '3':
            for (int i = 0; i < packet_storage_mega3.size(); i++) {
                if (packet_storage_mega3.at(i) == temp_process_packet) {
                    Serial.print("ReadBusSerialPacket - there is a same packet : ");
                    Serial.println(temp_process_packet);
                    Serial << packet_storage_mega3 << endl;
                    packet_storage_mega3.remove(i);
                    break;
                }
            }
            break;
    }
    return temp_process_packet_wo_parity;
}

String RTOS_RS485::ReadUSBSerialPacket() {
    String temp_string_command = "";
    if (Serial.available() > 0) {
        String temp_string_command = Serial.readStringUntil('/');
        return temp_string_command;
    }
    else {
        return temp_string_command;
    }
}

void RTOS_RS485::SerialFlush() {
    char temp_ch_read = '\0';
    delay(_serialDelay);
    while (RS485HwSerial1.available() > 0) {
        temp_ch_read = RS485HwSerial1.read();
    }
}

String RTOS_RS485::EchoGenerator(char sender_id, char motor_id, String string_command) {
    String stringPacket = _start_delimiter + String(_Mega_ID) + _start_delimiter + String(sender_id) + String(motor_id) + string_command;
    return stringPacket;
}

String RTOS_RS485::EchoGenerator(char sender_id, char motor_id, char char_command) {
    String stringPacket = _start_delimiter + String(_Mega_ID) + _start_delimiter + String(sender_id) + String(motor_id) + String(char_command);
    return stringPacket;
}
String RTOS_RS485::EchoGenerator(char sender_id, char motor_id, char char_command, char char_value){
    String stringPacket = _start_delimiter + String(_Mega_ID) + _start_delimiter + String(sender_id) + String(motor_id) + String(char_command) + String(char_value);
    return stringPacket;
}

String RTOS_RS485::EchoGenerator(char sender_id, char motor_id, char char_command, float float_value) {
    String stringPacket = _start_delimiter + String(_Mega_ID) + _start_delimiter + String(sender_id) + String(motor_id) + String(char_command) + String(float_value);
    return stringPacket;
}


String RTOS_RS485::EchoGenerator(char sender_id, char char_command) {
    String stringPacket = _start_delimiter + String(_Mega_ID) + _start_delimiter + String(sender_id) + String(char_command);
    return stringPacket;
}

String RTOS_RS485::EchoGenerator(char sender_id, char motor_id, char set_command1, char set_command2, float float_value) {
    String stringPacket = _start_delimiter + String(_Mega_ID) + _start_delimiter + String(sender_id) + String(motor_id) + String(set_command1) + String(set_command2) + String(float_value);
    return stringPacket;
}

String RTOS_RS485::EchoGenerator(char sender_id, char motor_id, char set_command1, char set_command2, int int_value) {
    String stringPacket = _start_delimiter + String(_Mega_ID) + _start_delimiter + String(sender_id) + String(motor_id) + String(set_command1) + String(set_command2) + String(int_value);
    return stringPacket;
}

// Use for CheckSerialPacket
String RTOS_RS485::CheckParityPacket(String expected_echo_packet) {
    Serial.print("CheckParityPacket - Parity Check Received ID : "); Serial.println(expected_echo_packet[3]);
    Serial.print("CheckParityPacket : "); Serial.println(expected_echo_packet);

    while(!ReadPacketsBUS(expected_echo_packet, true, false)); // should be false ConfirmParityOrCheckSerial
    String result_packet = check_serial_echo_packet;
    check_serial_echo_packet = "";
    return result_packet;
}



bool RTOS_RS485::CheckSerialPacket(String strExpectedEcho) {
    String strEcho = "";
    strEcho = CheckParityPacket(strExpectedEcho); // strEcho has no parity packet already being through packet processing

    Serial.println("-------------------------------------");
    Serial.print("CheckSerialPacket - Serial Echo : "); Serial.println(strEcho);
    Serial.print("CheckSerialPacket - Expected Echo: "); Serial.println(strExpectedEcho);

    if (strEcho != strExpectedEcho) {
        Serial.println("CheckSerialPacket - strEcho != strExpectedEcho");
        String output = "Check Serial - Different Echo : Received from ";
        String colon = ": ";
        output = output + colon + strEcho;
        Serial.print("Check Serial - Expected : "); Serial.println(strExpectedEcho);
        Serial.println(output);
        return false;
    }
    return true;
}

String RTOS_RS485::ParityGenerator(String Packet){
    int temp_ascii_value = 0;
    int temp_ascii_sum = 0;
    int temp_ascii_mod = 0;
    String parity_packet = "";

    for (int i = 0; i < Packet.length(); i++) {
        temp_ascii_value = Packet[i];
        temp_ascii_sum += temp_ascii_value;
    }
    temp_ascii_mod = temp_ascii_sum % 255;
    if (temp_ascii_mod < 16) {
        parity_packet = String(temp_ascii_mod, HEX);
        parity_packet = "0" + parity_packet;
    }
    else {
        parity_packet = String(temp_ascii_mod, HEX);
    }
    parity_packet = "*" + parity_packet;
    return parity_packet;
}

bool RTOS_RS485::ParityChecker(String Packet) {
    //Serial.println("PARITY CHECKER!!!");
    int temp_ascii_value = 0;
    int temp_ascii_sum = 0;
    int temp_ascii_mod = 0;
    String temp_parity_string = "";
    char temp_parity_buffer[3] = {'\0', };
    int temp_parity_integer = 0;

    int temp_location_delimiter = Packet.length() - 3;
    //  1. First Check : '*'?
    if (Packet[temp_location_delimiter] != '*') {
        //Serial.println("NO DELIMITER!!!");
        return false;
    } //location '*': length - 3

    // 2. Second Check : Ascii sum and mod
    for (int i = 0; i < temp_location_delimiter; i++) {
        temp_ascii_value = Packet[i];
        temp_ascii_sum += temp_ascii_value;
    }
    temp_ascii_mod = temp_ascii_sum % 255;

    // 3. String Hex to Int
    temp_parity_string = Packet.substring(temp_location_delimiter + 1);
    temp_parity_string.toCharArray(temp_parity_buffer, 3);
    temp_parity_integer = strtol(temp_parity_buffer, 0, 16);
    // 4. Third Check: ascii_mod == parity_buffer ?
    if (temp_parity_integer == temp_ascii_mod) {
        return true;
    }
    else {
        return false;
    }
}

bool RTOS_RS485::ParityProcessing(String packet_with_parity) {
    //It is only for normal serial packet, not parity confirmation packet.
    String temp_packet_parity_confirm = "";
    String temp_parity = "";
    char receiver_id = packet_with_parity[3];

    if (ParityChecker(packet_with_parity)) {
        Serial.println("ParityProcessing - Parity is Good");
        temp_packet_parity_confirm = PacketGenerator_485(receiver_id, 'O');
        temp_parity = ParityGenerator(temp_packet_parity_confirm);
        temp_packet_parity_confirm = temp_packet_parity_confirm + temp_parity + _end_delimiter;
        Serial.print("Echo Parity Check : "); Serial.println(temp_packet_parity_confirm);
        RS485HwSerial1.print(temp_packet_parity_confirm);
        delay(100);
        return true;
    }
    else {
        Serial.println("ParityProcessing - Parity is Bad");
        temp_packet_parity_confirm = PacketGenerator_485(receiver_id, 'N');
        temp_parity = ParityGenerator(temp_packet_parity_confirm);
        temp_packet_parity_confirm = temp_packet_parity_confirm + temp_parity + _end_delimiter;
        Serial.print("Echo Parity Check : "); Serial.println(temp_packet_parity_confirm);
        RS485HwSerial1.print(temp_packet_parity_confirm);
        delay(100);
        return false;
    }
}

bool RTOS_RS485::ConfirmParity(char receiver_id) {
    Serial.println("ConfirmParity");
    String expected_parity_echo = EchoGenerator(receiver_id, 'O'); //*1*2Y
    //KINDA NEW
    while(!ReadPacketsBUS(expected_parity_echo, true, true)); //Until Parity is exactly same with expected_parity_echo

    delay(200);

    switch (receiver_id) {
        case '0':
            if (parity_storage_mega0.empty()) {
                return false;
            }
            else {
                for (int i = 0; i < parity_storage_mega0.size(); i++) {
                    confirm_parity_echo = parity_storage_mega0.at(i);
                    confirm_parity_echo_wo_parity = confirm_parity_echo.substring(0, confirm_parity_echo.length() - 3);
                    if (confirm_parity_echo_wo_parity == expected_parity_echo) {
                        Serial.print("Index : "); Serial.println(i);
                        Serial.print("SendSerialPacket - ConfirmParity - there is a same packet : ");
                        Serial.println(confirm_parity_echo_wo_parity);
                        Serial << parity_storage_mega0 << endl;
                        parity_storage_mega0.remove(i);
                        delay(100);
                        Serial << parity_storage_mega0 << endl;
                        break;
                    }
                }
            }
            break;
        case '1':
            if (parity_storage_mega1.empty()) {
                return false;
            }
            else {
                for (int i = 0; i < parity_storage_mega1.size(); i++) {
                    confirm_parity_echo = parity_storage_mega1.at(i);
                    confirm_parity_echo_wo_parity = confirm_parity_echo.substring(0, confirm_parity_echo.length() - 3);
                    if (confirm_parity_echo_wo_parity == expected_parity_echo) {
                        Serial.print("Index : "); Serial.println(i);
                        Serial.print("SendSerialPacket - ConfirmParity - there is a same packet : ");
                        Serial.println(confirm_parity_echo_wo_parity);
                        Serial << parity_storage_mega1 << endl;
                        parity_storage_mega1.remove(i);
                        delay(100);
                        Serial << parity_storage_mega1 << endl;
                        break;
                    }
                }
            }
            break;
        case '2':
            if (parity_storage_mega2.empty()) {
                return false;
            }
            else {
                for (int i = 0; i < parity_storage_mega2.size(); i++) {
                    confirm_parity_echo = parity_storage_mega2.at(i);
                    confirm_parity_echo_wo_parity = confirm_parity_echo.substring(0, confirm_parity_echo.length() - 3);
                    if (confirm_parity_echo_wo_parity == expected_parity_echo) {
                        Serial.print("Index : "); Serial.println(i);
                        Serial.print("SendSerialPacket - ConfirmParity - there is a same packet : ");
                        Serial.println(confirm_parity_echo_wo_parity);
                        Serial << parity_storage_mega2 << endl;
                        parity_storage_mega2.remove(i);
                        delay(100);
                        Serial << parity_storage_mega2 << endl;
                        break;
                    }
                }
            }
            break;
        case '3':
            if (parity_storage_mega3.empty()) {
                return false;
            }
            else {
                for (int i = 0; i < parity_storage_mega3.size(); i++) {
                    confirm_parity_echo = parity_storage_mega3.at(i);
                    confirm_parity_echo_wo_parity = confirm_parity_echo.substring(0, confirm_parity_echo.length() - 3);
                    if (confirm_parity_echo_wo_parity == expected_parity_echo) {
                        Serial.print("Index : "); Serial.println(i);
                        Serial.print("SendSerialPacket - ConfirmParity - there is a same packet : ");
                        Serial.println(confirm_parity_echo_wo_parity);
                        Serial << parity_storage_mega3 << endl;
                        parity_storage_mega3.remove(i);
                        delay(100);
                        Serial << parity_storage_mega3 << endl;
                        break;
                    }
                }
            }
            break;
    }
    Serial.print("ConfirmParity - Parity Storage - "); Serial.print(receiver_id); Serial.print(" : "); Serial.println(confirm_parity_echo);

    //Strange Case - unlikely happened
    if (confirm_parity_echo_wo_parity != expected_parity_echo) {
        Serial.print("??? ConfirmParity - STR ECHO: ");
        Serial.println(confirm_parity_echo);
        Serial.print("??? ConfirmParity - STR ECHO - 3: ");
        Serial.println(confirm_parity_echo_wo_parity);
        Serial.print("??? ConfirmParity - Expected Parity ECHO : ");
        Serial.println(expected_parity_echo);
        return false;
    }
    return true;
}
