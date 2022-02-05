/*
    Basic Code to run the OpenDrop V4, Research platform for digital microfluidics
    Object codes are defined in the OpenDrop.h library
    Written by Urs Gaudenz from GaudiLabs, 2020
*/

#include <SPI.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <OpenDrop.h>
#include <OpenDropAudio.h>

#include "hardware_def.h"


unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
    unsigned long int c;
    while(1) {
        c = a%b;
        if(c==0) { return b; }
        a = b;
        b = c;
    }
    return 0;
}


typedef struct task {
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task;


static task task1, task2, task3, task4, task5;
task *tasks[] = { &task1, &task2, &task3, &task4, &task5 };
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

const char start = -1;
unsigned short i;
unsigned long GCD = tasks[1]->period;


OpenDrop OpenDropDevice = OpenDrop();
Drop *myDrop = OpenDropDevice.getDrop();

bool FluxCom[16][8];
int ControlBytesIn[16];
int ControlBytesOut[24];
int readbyte;

int JOY_value;
int joy_x, joy_y;
int x, y;
int del_counter = 0;
int del_counter2 = 0;

bool SWITCH_state = true;
bool SWITCH_state2 = true;
bool idle = true;

bool Magnet1_state = false;
bool Magnet2_state = false;

int j = 0;


// 2. serial communication
enum Comm_States { SC_Init };

int TickFct_Comm(int state) {

    switch(state) {
        case SC_Init:
            if (Serial.available() > 0) {  // receive data from App
                readbyte = Serial.read();

                if (x < FluxlPad_width)
                    for (y = 0; y < 8; y++)
                        FluxCom[x][y] = (((readbyte) >> (y)) & 0x01);
                else
                    ControlBytesIn[x - FluxlPad_width] = readbyte;
                x++;

                digitalWrite(LED_Rx_pin,HIGH);
                if (x == (FluxlPad_width + 16)) {
                    OpenDropDevice.set_Fluxels(FluxCom);
                    OpenDropDevice.drive_Fluxels();
                    OpenDropDevice.update_Display();

//                    if ((ControlBytesIn[0] & 0x2) && (Magnet1_state == false)) {
//                        Magnet1_state = true;
//                        OpenDropDevice.set_Magnet(0, HIGH);
//                    };
//
//                    if (!(ControlBytesIn[0] & 0x2) && (Magnet1_state == true)) {
//                        Magnet1_state = false;
//                        OpenDropDevice.set_Magnet(0, LOW);
//                    };
//
//                    if ((ControlBytesIn[0] & 0x1) && (Magnet2_state == false)) {
//                        Magnet2_state = true;
//                        OpenDropDevice.set_Magnet(1, HIGH);
//                    };
//
//                    if (!(ControlBytesIn[0] & 0x1) && (Magnet2_state == true)) {
//                        Magnet2_state = false;
//                        OpenDropDevice.set_Magnet(1, LOW);
//                    };

                    for (x = 0; x < 24; x++)
                        Serial.write(ControlBytesOut[x]);

                    x = 0;
                };
            }
            else
                digitalWrite(LED_Rx_pin, LOW);

            state = SC_Init;
            break;

        default:
            state = SC_Init;
            break;
    }

    switch(state) {
        case SC_Init:
            break;

        default:
            break;
    }

    return state;
}


// 3. update display
enum Display_States { UD_Init };

int TickFct_Display(int state) {

    switch(state) {
        case UD_Init:
            del_counter--;

            if (del_counter < 0) {  // update Display
                OpenDropDevice.update_Display();
                del_counter = 1000;
            }

            state = UD_Init;
            break;

        default:
            state = UD_Init;
            break;
    }

    switch(state) {
        case UD_Init:
            break;

        default:
            break;
    }

    return state;
}


// 4. activate menu
enum Menu_States { AM_Init };

int TickFct_Menu(int state) {

    switch(state) {
        case AM_Init:
            SWITCH_state = digitalRead(SW1_pin);

            if (!SWITCH_state) {  // activate Menu
                OpenDropAudio.playMe(1);
                Menu(OpenDropDevice);
                OpenDropDevice.update_Display();
                del_counter2 = 200;
            }

            state = AM_Init;
            break;

        default:
            state = AM_Init;
            break;
    }

    switch(state) {
        case AM_Init:
            break;

        default:
            break;
    }

    return state;
}


// 5. activate reservoirs
enum Reservoir_States { AR_Init };

int TickFct_Reservoir(int state) {

    switch(state) {
        case AR_Init:
            SWITCH_state2 = digitalRead(SW2_pin);

            if (!SWITCH_state2) {  // activate Reservoirs
                if ((myDrop->position_x() == 15) && (myDrop->position_y() == 3)) {
                    myDrop->begin(14, 1);
                    OpenDropDevice.dispense(1, 1200);
                }

                if ((myDrop->position_x() == 15) && (myDrop->position_y() == 4)) {
                    myDrop->begin(14, 6);
                    OpenDropDevice.dispense(2, 1200);
                }

                if ((myDrop->position_x() == 0) && (myDrop->position_y() == 3)) {
                    myDrop->begin(1, 1);
                    OpenDropDevice.dispense(3, 1200);
                }

                if ((myDrop->position_x() == 0) && (myDrop->position_y() == 4)) {
                    myDrop->begin(1, 6);
                    OpenDropDevice.dispense(4, 1200);
                }

                if ((myDrop->position_x() == 10) && (myDrop->position_y() == 2)) {
//                    if (Magnet1_state) {
//                        OpenDropDevice.set_Magnet(0, HIGH);
//                        Magnet1_state = false;
//                    }
//                    else {
//                        OpenDropDevice.set_Magnet(0, LOW);
//                        Magnet1_state = true;
//                    }
                    while (!digitalRead(SW2_pin));
                }

                if ((myDrop->position_x() == 5) && (myDrop->position_y() == 2)) {
//                    if (Magnet2_state) {
//                        OpenDropDevice.set_Magnet(1, HIGH);
//                        Magnet2_state = false;
//                    }
//                    else {
//                        OpenDropDevice.set_Magnet(1, LOW);
//                        Magnet2_state = true;
//                    }
                    while (!digitalRead(SW2_pin));
                }
            }

            state = AR_Init;
            break;

        default:
            state = AR_Init;
            break;
    }

    switch(state) {
        case AR_Init:
            break;

        default:
            break;
    }

    return state;
}


// 6. navigate using joystick
enum Joystick_States { NJ_Init };

int TickFct_Joystick(int state) {

    switch (state) {
        case NJ_Init:
            JOY_value = analogRead(JOY_pin);  // navigate using Joystick

            if ((JOY_value < 950) & (del_counter2 == 0)) {
                if (JOY_value < 256) {
                    myDrop->move_right();
                    Serial.println("right");
                }
                if ((JOY_value > 725) && (JOY_value < 895)) {
                    myDrop->move_up();
                    Serial.println("up");
                }
                if ((JOY_value > 597) && (JOY_value < 725)) {
                    myDrop->move_left();
                    Serial.println("left");
                }
                if ((JOY_value > 256) && (JOY_value < 597)) {
                    myDrop->move_down();
                    Serial.println("down");
                }

                OpenDropDevice.update_Drops();
                OpenDropDevice.update();

                if (idle) {
                    del_counter2 = 1800;
                    idle = false;
                } else
                    del_counter2 = 400;

                del_counter = 1000;
            }

            if (JOY_value > 950) {
                del_counter2 = 0;
                idle = true;
            }

            if (del_counter2 > 0)
                del_counter2--;

            state = NJ_Init;
            break;

        default:
            state = NJ_Init;
            break;
    }

    switch(state) {
        case NJ_Init:
            break;

        default:
            break;
    }

    return state;
}


// 1. setup loop
// the setup function runs once when you press reset or power the board
void setup() {
    task1.state = start;
    task1.period = 1;                   // period of 1 ms
    task1.elapsedTime = task1.period;
    task1.TickFct = &TickFct_Comm;

    task2.state = start;
    task2.period = 1;                   // period of 1 ms
    task2.elapsedTime = task2.period;
    task2.TickFct = &TickFct_Display;

    task3.state = start;
    task3.period = 1;                   // period of 1 ms
    task3.elapsedTime = task3.period;
    task3.TickFct = &TickFct_Menu;

    task4.state = start;
    task4.period = 1;                   // period of 1 ms
    task4.elapsedTime = task4.period;
    task4.TickFct = &TickFct_Reservoir;

    task5.state = start;
    task5.period = 1;                   // period of 1 ms
    task5.elapsedTime = task5.period;
    task5.TickFct = &TickFct_Joystick;

    for(i = 1; i < numTasks; i++) {
        GCD = findGCD(GCD, tasks[i]->period);
    }


    Serial.begin(115200);

    OpenDropDevice.begin();

    // ControlBytesOut[23] = OpenDropDevice.get_ID();

    OpenDropDevice.set_voltage(240, false, 1000);

    OpenDropDevice.set_Fluxels(FluxCom);

    pinMode(JOY_pin, INPUT);

    OpenDropAudio.begin(16000);
    OpenDropAudio.playMe(2);
    delay(2000);

    OpenDropDevice.drive_Fluxels();
    OpenDropDevice.update_Display();
    Serial.println("Welcome to OpenDrop");

    myDrop->begin(7, 4);
    OpenDropDevice.update();
}


void loop() {
    for(i = 0; i < numTasks; i++) {
        if(tasks[i]->elapsedTime == tasks[i]->period) {
            tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
            tasks[i]->elapsedTime = 0;
        }
        tasks[i]->elapsedTime += GCD;
    }
}
