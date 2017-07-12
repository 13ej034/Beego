#include <runCtrl.h>
#include <vutils.h>
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
        const char *beego_port = "COM3"; //port number
        enum
        {
            MOTOR_ID_LEFT = 1, MOTOR_ID_RIGHT = 0
        }; // motor id
        int beego_wheel_velocity_left = 2;
        int beego_wheel_velocity_right = -2;
        char pwm_left = 0;
        char pwm_right = 0;
        unsigned short int enc_left;
        unsigned short int enc_right;
    public:
        void init();
        void setMotorMode();
        void setMotorRotation_left(char &pwm);
        void setMotorRotation_right(char &pwm);
        void move(double U);
        void move_wv(double U);
        void wheelOdom();
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
    run.setWheelVel(MOTOR_ID_LEFT, beego_wheel_velocity_left);
    run.setWheelVel(MOTOR_ID_RIGHT, beego_wheel_velocity_right);
}

void agv::robot::setMotorRotation_left(char &pwm)
{
    if (pwm < 0)
    {
        beego_wheel_velocity_left = -2;
        agv::robot::setMotorMode();
    }
    else
    {
        beego_wheel_velocity_left = 2;
        agv::robot::setMotorMode();
    }
}

void agv::robot::setMotorRotation_right(char &pwm)
{
    if (pwm < 0)
    {
        beego_wheel_velocity_right = -2;
        agv::robot::setMotorMode();
    }
    else
    {
        beego_wheel_velocity_right = 2;
        agv::robot::setMotorMode();
    }
}


void agv::robot::move(double U)
{
    agv::robot::convertToPWM(U, pwm_left, pwm_right);
    std::cout << "left= " << abs(+pwm_left) << " right= " << abs(+pwm_right) << std::endl;
    run.setMotorPwm(MOTOR_ID_LEFT, abs(pwm_left));
    run.setMotorPwm(MOTOR_ID_RIGHT, abs(pwm_right));
    agv::robot::wheelOdom();
}

void agv::robot::move_wv(double U)
{
    run.setWheelVel(MOTOR_ID_LEFT, U);
    run.setWheelVel(MOTOR_ID_RIGHT, U);
    agv::robot::wheelOdom();
}

void agv::robot::wheelOdom()
{
    run.getEncoderValue(MOTOR_ID_LEFT, &enc_left);
    run.getEncoderValue(MOTOR_ID_RIGHT, &enc_right);
    std::cout << "[left, right] = [" << enc_left << "," << enc_right << "]" << std::endl;
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
    //for (int i = 80; i > 0; --i)
    //{
    //    run.setMotorPwm(MOTOR_ID_LEFT, i);
    //    run.setMotorPwm(MOTOR_ID_RIGHT, i);
    //}
    run.stop();
    run.disconnect();
}

void agv::robot::convertToPWM(double U, char &left, char &right)
{
    left = (int)U;
    right = (int)-U;
    //agv::robot::setMotorRotation_left(left);
    //agv::robot::setMotorRotation_right(right);
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
    double U = 1;

    agv::robot beego;
    beego.init();

    //HANDLE hPipe;
    //char szBuff[32];
    //DWORD dwNumberOfBytesRead;

    //hPipe = CreateNamedPipe(
    //    L"\\\\.\\pipe\\pwm",
    //    PIPE_ACCESS_DUPLEX,
    //    PIPE_TYPE_BYTE | PIPE_WAIT,
    //    3,
    //    0,
    //    0,
    //    100,
    //    NULL);

    //std::cout << "Connecting client ..." << std::endl;

    //if (hPipe == INVALID_HANDLE_VALUE || !ConnectNamedPipe(hPipe, NULL))
    //{
    //    CloseHandle(hPipe);
    //    std::cout << "Connect client : false" << std::endl;
    //    return 1;
    //}

    //std::cout << "Connect client : Success" << std::endl;

    //while (ReadFile(hPipe, szBuff, sizeof(szBuff), &dwNumberOfBytesRead, NULL)){
    //    szBuff[dwNumberOfBytesRead] = '\0';
    //    U = atof(szBuff);
    //    beego.move(U);
    //}

    //FlushFileBuffers(hPipe);
    //DisconnectNamedPipe(hPipe);
    //CloseHandle(hPipe);
 
    while (U < 100)
    {
        beego.move(U);
        //beego.move_wv(U);
        beego.wheelOdom();
        U++;
    }

    beego.end();
    return 0;
}