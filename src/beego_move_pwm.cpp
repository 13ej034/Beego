#include <runCtrl.h>
#include <chrono>
#include <iostream>
#include <string>
#include <fstream> // file I/O

namespace agv
{
    class robot
    {
    private:
        RunCtrl run;
        const char *beego_port = "COM4"; //port number
        enum
        {
            MOTOR_ID_LEFT = 1, MOTOR_ID_RIGHT = 0
        }; // motor id
        int beego_wheel_velocity = -20;
        char beego_pwm = 200;
    public:
        void init();
        void move();
        double time();
        void end();
    };
}

void agv::robot::init()
{
    run.connect(beego_port);
    run.setWheelVel(MOTOR_ID_LEFT, beego_wheel_velocity);
    run.setWheelVel(MOTOR_ID_RIGHT, beego_wheel_velocity);
}

void agv::robot::move()
{
    run.setMotorPwm(MOTOR_ID_LEFT, beego_pwm);
    run.setMotorPwm(MOTOR_ID_RIGHT, beego_pwm);
}

double agv::robot::time()
{
    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();
    agv::robot::move();
    end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "time: " << elapsed << "[ms]" << std::endl;
    return elapsed;
}

void agv::robot::end()
{
    run.stop();
    run.disconnect();
}

int main(int argc, char **argv)
{
    // Output file
    std::string filename = "sampling_time.txt";
    std::ofstream writing_file;
    writing_file.open(filename, std::ios::out);

    std::cout << "Writing " << filename << "..." << std::endl;
    writing_file << "time[ms]" << std::endl;

    double time;

    agv::robot beego;
    beego.init();

    for (int i = 0; i < 60; i++)
    {
        //beego.move();
        time = beego.time();
        writing_file << time << std::endl;
    }
    
    beego.end();
    return 0;
}