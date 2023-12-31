

// File:          Max_Control.cpp
// Date:          28/11/23
// Description:   Por ahora poco
// Author:        Daniel Prieto Romero && Ythais Orzoco
// Modifications:

#ifndef MY_ROBOT_H_
#define MY_ROBOT_H_

#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/Compass.hpp>
#include <webots/PositionSensor.hpp>
#include <webots/GPS.hpp>
#include <webots/Camera.hpp>
#include <math.h>

// All the webots classes are defined in the "webots" namespace

using namespace std;
using namespace webots;

#define MAX_SPEED 10
#define DESIRED_ANGLE 0
#define NUM_DISTANCE_SENSOR 16
#define DISTANCE_LIMIT 200

enum Posicion
{

    LIBRE,
    BLOQUEO,
    PAR_IZ,
    PAR_DER,
    PAR_FRONT,
    ESQ_IZ,
    ESQ_DER,

};
class MyRobot : public Robot
{
public:
    /**
     * @brief Empty constructor of the class.
     */
    MyRobot();

    /**
     * @brief Destructor of the class.
     */
    ~MyRobot();

    /**
     * @brief Function with the logic of the controller.
     * @param
     * @return
     */
    void run();

    /**
     * @brief Function for converting bearing vector from compass to angle (in degrees).
     * @param in_vector Input vector with the values to convert.
     * @return The value in degrees.
     */

    double convert_bearing_to_degrees(const double *in_vector);

    // Metodos publicos
    void forward();
    void set_speed();
    void get_info();
    void stop();
    void turn_left();
    void turn_right();
    void esquivar();
    void seguir_contorno();
    void desbloquear();
    void backward();

private:
    // Atributos Generales
     int _time_step;

    const double _infinity = numeric_limits<double>::infinity();

        // velocities
    double _left_speed, _right_speed;

    // Atributos odometria
    double _left_speed, _right_speed, ir_frontal;

    float _x, _y, _x_goal, _y_goal, _x_ant, _y_ant, _theta_ant; // [=] meters

    float _theta, _theta_goal, _theta_compas; // [=] rad

    float _sr, _sl, _sr_ant, _sl_ant, _inc_sr, _inc_sl; // [=] meters



   


    // WORKING MODES
    int _posicion = 0;

    // Metodos privados (sensores)
    void compute_odometry();
    double convert_bearing_to_degrees();
    float encoder_tics_to_meters(float tics);
    double convert_bearing_to_degrees2(const double *in_vector);

    // Inicializar Sensors
    // Compass
    Compass *_my_compass;
    // Distance Sensors
    DistanceSensor *_distance_sensor[NUM_DISTANCE_SENSOR];

    // Motor Position Sensor
    PositionSensor *_left_wheel_sensor;
    PositionSensor *_right_wheel_sensor;

    // Camera sensors
    Camera *_forward_camera;
    Camera *_spherical_camera;

    // Motors
    Motor *_left_wheel_motor;
    Motor *_right_wheel_motor;
};

#endif
