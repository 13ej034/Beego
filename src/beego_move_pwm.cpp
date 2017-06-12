#include <runCtrl.h>
#include <chrono>
#include <iostream>
#include <string>
#include <fstream> // file I/O
#include <windows.h> // IPC


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
        int beego_wheel_velocity = 2;
        char pwm_left = 0;
        char pwm_right = 0;
    public:
        void init();
        void setMotorMode();
        void setMotorRotation(char &pwm);
        void move(double U);
        void convertToPWM(double U, char &left, char &right);
        double time();
        void end();
    };
}

void agv::robot::init()
{
    run.connect(beego_port);
    agv::robot::setMotorMode();
}

void agv::robot::setMotorMode()
{
    run.setWheelVel(MOTOR_ID_LEFT, beego_wheel_velocity);
    run.setWheelVel(MOTOR_ID_RIGHT, beego_wheel_velocity);
}

void agv::robot::setMotorRotation(char &pwm)
{
    if (pwm < 0)
    {
        beego_wheel_velocity = -2;
        agv::robot::setMotorMode();
    }
    else
    {
        beego_wheel_velocity = 2;
        agv::robot::setMotorMode();
    }
}

void agv::robot::move(double U)
{
    agv::robot::convertToPWM(U, pwm_left, pwm_right);
    std::cout << "left= " << abs(+pwm_left) << " right= " << abs(+pwm_right) << std::endl;
    run.setMotorPwm(MOTOR_ID_LEFT, abs(pwm_left));
    run.setMotorPwm(MOTOR_ID_RIGHT, abs(pwm_right));
}

double agv::robot::time()
{
    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();
    //agv::robot::move();
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

void agv::robot::convertToPWM(double U, char &left, char &right)
{
    left = 100 - (int)U;
    right = 100 + (int)U;
    agv::robot::setMotorRotation(left);
    agv::robot::setMotorRotation(right);
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
    double U;

    agv::robot beego;
    beego.init();

    HANDLE hPipe;
    char szBuff[32];
    DWORD dwNumberOfBytesRead;

    hPipe = CreateNamedPipe(
        L"\\\\.\\pipe\\pwm",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        3,
        0,
        0,
        100,
        NULL);

    std::cout << "Connecting client ..." << std::endl;

    if (hPipe == INVALID_HANDLE_VALUE || !ConnectNamedPipe(hPipe, NULL))
    {
        CloseHandle(hPipe);
        std::cout << "Connect client : false" << std::endl;
        return 1;
    }

    std::cout << "Connect client : Success" << std::endl;

    while (ReadFile(hPipe, szBuff, sizeof(szBuff), &dwNumberOfBytesRead, NULL)){
        szBuff[dwNumberOfBytesRead] = '\0';
        U = atof(szBuff);
        beego.move(U);
    }

    //for (int i = 0; i < 60; i++)
   //{
        //beego.move();
        //time = beego.time();
        //writing_file << time << std::endl;
    //}
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    beego.end();
    return 0;
}