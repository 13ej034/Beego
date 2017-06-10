#include <runCtrl.h>
#include <chrono>
#include <iostream>

void move_process();
void measure_time();

void move_process()
{
    // Connect beego
    RunCtrl run;
    const char *beego_port = "COM5"; // port number
    run.connect(beego_port);

    // Setting beego
    enum{ MOTOR_ID_LEFT = 1, MOTOR_ID_RIGHT = 0 }; // motor id
    run.setWheelVel(MOTOR_ID_LEFT, 20);
    run.setWheelVel(MOTOR_ID_RIGHT, -20);

    run.setMotorPwm(MOTOR_ID_LEFT, 100);
    run.setMotorPwm(MOTOR_ID_RIGHT, 100);
}

 inline void measure_time()
{
    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();
    move_process();
    end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "time: " << elapsed << "[ms]" << std::flush;
}

int main(int argc, char **argv)
{
    //move_process();
    for (int i = 0; i < 6; i++)
    {
        measure_time();
    }
    return 0;
}