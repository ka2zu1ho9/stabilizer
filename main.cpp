#include "mbed.h"

DigitalOut led[] = {
    DigitalOut (LED1),
    DigitalOut (LED2),
    DigitalOut (LED3)
};

DigitalIn my_switch(p26);

enum analog_pin_number {
    potentiometer = 0,
    gyro_1 = 1, // not use
    gyro_2 = 2  // not use
};

AnalogIn analog[] = {
    AnalogIn (p15),
    AnalogIn (p16), // not use
    AnalogIn (p17), // not use
};

PwmOut pwm[] = {
    PwmOut (p21),
    PwmOut (p22) // not use
};

RawSerial pc(USBTX,USBRX,115200);

Timer my_time;

// Unit
const float milli = 1.0/(float)(10*10*10);
const float micro = 1.0/(float)(10*10*10*10*10*10);

// potentiometer status
const float potentio_init_time = 10.0;
const float potentiometer_max_angle = 300.0;

// pwm statuss
const float pwm_period = 20.0; // msec
const float freaquency = 4.0; //Hz

// function defininition
float read_potentiometer(size_t potentio_num);
void set_angle(size_t servo_num ,float angle);


// global variable
int loop_count = 0;

float temporary_angle = 0;
float shift_angle = 0;

float now_time = 0;
float last_time = 0;

float servo_angle = 0;

int main() {
    pwm[0].period(pwm_period * milli); // servo period 15msec

    servo_angle = 0;
    set_angle(0, servo_angle);

    led[0].write(0);
    led[1].write(0);

    my_time.start();
    while(my_time.read() <= potentio_init_time){
        temporary_angle += read_potentiometer(0);
  
        led[0].write(1);
        led[1].write(1);
        loop_count ++;
    }
    shift_angle = temporary_angle / loop_count;
    loop_count = 0;
    my_time.stop();
    my_time.reset();

    led[0].write(0);
    led[1].write(0);

    while(1) {
        if(my_switch.read() == 1){
            my_time.start();
            led[0].write(1);
            led[1].write(0);

            now_time = my_time.read();

            servo_angle = 10.0*sin(2*3.1415926535*freaquency*now_time);
            set_angle(0, servo_angle);

            pc.printf("%7f,%7f,%7f\r\n", now_time, -1*read_potentiometer(0),servo_angle);

            if(now_time > 20.0){
                break;
            }
        }else{
            led[0].write(0);
            led[1].write(1);
        }
    }
    led[0].write(0);
    led[1].write(0);
    led[2].write(1);
}

float read_potentiometer(size_t potentio_num){
    return potentiometer_max_angle / 2 - potentiometer_max_angle*analog[potentio_num].read() - shift_angle;
}

void set_angle(size_t servo_num, float angle){
    float pulse;
    
    pulse = 1500.0 + 800 * angle / 135.0;
    
    pwm[servo_num].pulsewidth(pwm_period*milli - pulse*micro);
}