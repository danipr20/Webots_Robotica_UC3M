#include "MyRobot.h"

//////////////////////////////////////////////

MyRobot::MyRobot() : Robot()
{
    // init default values
    _time_step = 64;
    // Variables odometria
    _x = _y = _theta = _x_ant = _y_ant = _theta_ant = 0.0; // robot pose variables
    _sr = _sl = _sl_ant = _sr_ant = 0.0;                   // displacement right and left wheels
    _x_goal = 16.4, _y_goal = 0,
    _theta_goal = atan2((_y_goal - _y), (_x_goal - _x)); // target pose

    // Variables de velocidad =0 al comienzo
    _left_speed = 0;
    _right_speed = 0;

    ir_frontal = 0.0, ir_izq = 0.0, ir_der = 0.0;
    i = 0;
    pared_iz = 0;
    pared_der = 0;
    girando=0;     

    //Inicializar variables para deteccion de personas.
    persona1_detectada = 0;
    persona1_evitada = 0;
    persona2_detectada = 0;
    
    //Configuramos el color verde
    posX[0]= 0;
    posX[1]= 0;
    posY[0]= 0;
    posY[1]= 0;
    rango = 0;
    rango1=0;

    //modos/pasos
    comienza_paso_2=0;
    comienza_paso_3=0;
    comienza_paso_4=0;
    comienza_paso_5=0;
    comienzo_vuelta_casa=0;


    // set mode to fordward
    _posicion = LIBRE;

    // get and enable the compass device
    _my_compass = getCompass("compass");
    _my_compass->enable(_time_step);

    // Iniciar Sensores de Distancia
    _distance_sensor[0] = getDistanceSensor("ds0");
    _distance_sensor[0]->enable(_time_step);
    _distance_sensor[1] = getDistanceSensor("ds2");
    _distance_sensor[1]->enable(_time_step);
    _distance_sensor[2] = getDistanceSensor("ds14");
    _distance_sensor[2]->enable(_time_step);

    // Iniciar Encoders
    _left_wheel_sensor = getPositionSensor("left wheel sensor");
    _right_wheel_sensor = getPositionSensor("right wheel sensor");
    _left_wheel_sensor->enable(_time_step);
    _right_wheel_sensor->enable(_time_step);

    // Iniciar Camaras
    _forward_camera = getCamera("camera_f");
    _forward_camera->enable(_time_step);
    _spherical_camera = getCamera("camera_s");
    _spherical_camera->enable(_time_step);

    // Iniciar Motores
    _left_wheel_motor = getMotor("left wheel motor");
    _right_wheel_motor = getMotor("right wheel motor");
}

//////////////////////////////////////////////

MyRobot::~MyRobot()
{
    // disable devices

    _left_wheel_motor->setVelocity(0.0);
    _right_wheel_motor->setVelocity(0.0);

    _my_compass->disable();

    for (int i = 0; i < NUM_DISTANCE_SENSOR; i++)
    {
        _distance_sensor[i]->disable();
    }

    _left_wheel_sensor->disable();
    _right_wheel_sensor->disable();
}

//////////////////////////////////////////////

void MyRobot::run()
{

    // set the motor position to non-stop moving
    _left_wheel_motor->setPosition(_infinity);
    _right_wheel_motor->setPosition(_infinity);
    
    //inicializamos variable de color enviada al robot
    int image_width_f = _forward_camera->getWidth();
    int image_height_f = _forward_camera->getHeight();

    while (step(_time_step) != -1)
    {
        const double *compass_val = _my_compass->getValues();

        compass_angle = convert_bearing_to_degrees2(compass_val);
        this->compute_odometry();
        this->print_odometry();

        _sl = encoder_tics_to_meters(_left_wheel_sensor->getValue());
        _sr = encoder_tics_to_meters(_right_wheel_sensor->getValue());
        
        
        // convert compass bearing vector to angle, in degrees
        ir_frontal = _distance_sensor[0]->getValue();
        ir_izq = _distance_sensor[1]->getValue();
        ir_der = _distance_sensor[2]->getValue();
        get_info();

       if (_x<1  && compass_angle<-20 && !comienza_paso_2){
       turn_right();

       if(compass_angle>=-26 ){comienza_paso_2=1;}
       }
       
       //fase 2 sigue paredes derecha
       if(comienza_paso_2 && !comienza_paso_3){
       cout<<"ejecuta paso 2 seguir derecha"<<endl;
      seguir_pared_derecha();
      if(_x>=9.2/*6.3  funciona en mundos 1,4,...*/){comienza_paso_3=1;}}
      
      //Fase 3 intentamos sortear la zona central y llegamos a la izquierda
      if(comienza_paso_3 && !comienza_paso_4){
       cout<<"ejecuta paso 3"<<endl;
    
         if (_y<-1.7 && compass_angle>=-125 && girando==0){
       turn_left();
            }
       else{girando=1 ;  
       if(_y<=-1.1){seguir_pared_izquierda(); }
        
       if(_y>=-1.1 && compass_angle>=-140){forward();}
            }
        if(_y>=2){comienza_paso_4=1;}

     /* if(_x>=12){comienza_paso_4=1;}*/
      }
     
            //Estamos en la izquierda, comenzamos el sigueparedes

      if(comienza_paso_4 && !comienza_paso_5){
       cout<<"ejecuta paso 4 seguir derecha"<<endl;
      seguir_pared_izquierda();
      if(_x>=18){comienza_paso_5=1;}}      
      
      
       //Busqueda de personas
       if(comienza_paso_5==1 && persona1_detectada == 0 && _x<=18.5){
       cout<<"ejecuta paso 5 seguir izquierda " <<endl;
      
      forward();} //antes era segui_pared_izquierda sin condicion de _x
      
      if(comienza_paso_5==1 && persona1_detectada == 0 && _x>17){
       cout<<"ejecuta paso 5 siguiente " <<endl;
      
      seguir_pared_izquierda();
      salida++;
      cout<< "salida: " << salida <<endl;
      if(salida >= 30 && salida <= 70){forward();}
      
      }
      //cout << "Size of forward camera image: " << image_width_f << ", " <<  image_height_f << endl;
      if (rango<31 && _x>=17.5 && detectar_color(_forward_camera->getImage(), image_width_f, image_height_f)) {
      salida=0;
      persona1_detectada = 1;
      
      cout<<"persona 1 detectada: " << persona1_detectada <<endl;
      
      if(rango<3) //fijamos la posicion encontrada de inicio a fin de la persona 1
      {
      posX[0]=_x;
      posY[0]=_y;
      posY[1]=posY[0]-2; //desde donde esta hacia delante
      if(_x<20){
      posX[1]=posX[0]+0.25;}
      if(_x>21){
      posX[1]=posX[0]-0.25;}
      }
      rango++;
      
      _left_speed = MAX_SPEED - 9;
      _right_speed = -MAX_SPEED + 9;
    
      }
      
      cout<<"rango: "<< rango << " rango1: " << rango1 <<endl;
      if( persona2_detectada == 0 && comienza_paso_5 && persona1_detectada == 1 && rango >= 31)
      {
      seguir_pared_izquierda();
      cout <<"Persona 1 detectada"<<endl;
      //si esta por salir del area sin detectar a persona 2
      if(_x <=17 && persona2_detectada==0){
      backward();
      turn_left();
      cout<< "retrocede y busca"<<endl;}
      }
      if (rango1<28 && _y < posY[1] && persona1_detectada==1 && detectar_color(_forward_camera->getImage(), image_width_f, image_height_f)){
      
      persona2_detectada = 1;
      
      cout<<"persona 2 detectada: " << persona2_detectada <<endl;
      
      rango1++;
      
      _left_speed = MAX_SPEED - 9;
      _right_speed = -MAX_SPEED + 9;
      }
      
      if(comienza_paso_5==1 && persona2_detectada == 1 && rango1 >= 28)
      {
      cout <<"Todas las personas encontradas"<<endl;
      comienzo_vuelta_casa=1;}

      if(comienzo_vuelta_casa==1){seguir_pared_izquierda();}
      
      
 
       
       
    _left_wheel_motor->setVelocity(_left_speed);
    _right_wheel_motor->setVelocity(_right_speed);
    
    }
}

/////////////////////////////
// METODOS DE CALCULO

double MyRobot::convert_bearing_to_degrees(const double *in_vector)
{
    double rad = atan2(in_vector[0], in_vector[2]);
    double deg = rad * (180.0 / M_PI);

    return deg;
}

void MyRobot::get_info()
{
cout << " Ir_der : " << ir_der << "Ir_frontal: " << ir_frontal << " Ir_izq: " << ir_izq << " angulo compass: "<<compass_angle<<endl;
}


//////////////////////////
// METODOS DE MOVIMIENTO

void MyRobot::forward()
{
    _left_speed = MAX_SPEED;
    _right_speed = MAX_SPEED;
}

void MyRobot::turn_left()
{

    _left_speed = -MAX_SPEED + 9;
    _right_speed = MAX_SPEED - 9;

}

void MyRobot::turn_right()
{

    _left_speed = MAX_SPEED - 9;
    _right_speed = -MAX_SPEED + 9;
}

void MyRobot::stop()
{
    _left_speed = 0;
    _right_speed = 0;
}
void MyRobot::esquivar()
{
    turn_right();
    seguir_contorno();
}
void MyRobot::seguir_contorno()
{
}
void MyRobot::desbloquear()
{
    void backward();
}
void MyRobot::backward()
{
    _left_speed = -MAX_SPEED;
    _right_speed = -MAX_SPEED;
}

///////////////////////////////////////////

// METODOS ODOMETRIA
////////////////////////////////////////////////////////

void MyRobot::compute_odometry()
{
    _inc_sl = _sl - _sl_ant;
    _inc_sr = _sr - _sr_ant;

    _theta = _theta_ant + (_inc_sr - _inc_sl) / WHEELS_DISTANCE;
    _x = _x_ant + ((_inc_sr + _inc_sl) / (2) * cos(_theta + ((_inc_sr - _inc_sl) / (2 * WHEELS_DISTANCE))));
    _y = _y_ant + ((_inc_sr + _inc_sl) / (2) * sin(_theta + ((_inc_sr - _inc_sl) / (2 * WHEELS_DISTANCE))));

    _x_ant = _x;
    _y_ant = _y;
    _theta_ant = _theta;

    _sl_ant = _sl;
    _sr_ant = _sr;
}

double MyRobot::convert_bearing_to_degrees()
{
    const double *in_vector = _my_compass->getValues();

    double rad = atan2(in_vector[0], in_vector[2]);
    double deg = rad * (180.0 / M_PI);

    return deg;
}

float MyRobot::encoder_tics_to_meters(float tics)
{
    return tics / ENCODER_TICS_PER_RADIAN * WHEEL_RADIUS;
}

void MyRobot::print_odometry()
{
    cout << "x:" << _x << " y:" << _y << " theta:" << _theta << endl;
    //cout << _x_goal << endl;
}

bool MyRobot::goal_reached()
{
    if (_x > _x_goal)
    {
        _left_wheel_motor->setVelocity(0);
        _right_wheel_motor->setVelocity(0);
        
        return true;
    }
    return false;
}

double MyRobot::convert_bearing_to_degrees2(const double *in_vector)
{
    double rad = atan2(in_vector[0], in_vector[2]);
    double deg = rad * (180.0 / M_PI);

    return deg;
}

void MyRobot::seguir_pared_izquierda()
{

     if (( ir_der < DISTANCIA_CHOQUE && ir_frontal < DISTANCIA_CHOQUE && ir_izq < DISTANCIA_CHOQUE) || (ir_izq > DISTANCIA_CHOQUE && ir_frontal < DISTANCIA_CHOQUE))
     {
         // movimiento de frente, ninguna pared cerca o pared a la izquierda
         _left_speed = MAX_SPEED-1;
         _right_speed = MAX_SPEED-1;

     }
     if ( ir_izq > DISTANCIA_CHOQUE)
     {
         pared_iz = 1;
         pared_der = 0;

     }
     if (ir_der > DISTANCIA_CHOQUE)
     {
         pared_der = 1;
         pared_iz = 0;


     }

     if (ir_izq < DISTANCIA_CHOQUE && pared_iz == 1)
     {
         // esquina hacia la izquierda, antes detectaba pared y ahora no

          _left_speed = -MAX_SPEED + 9;
        _right_speed = MAX_SPEED - 8;

     }
     
 
     if (( ir_der > DISTANCIA_CHOQUE) || (ir_frontal > DISTANCIA_CHOQUE) || (ir_izq > DISTANCIA_CHOQUE && ir_frontal > DISTANCIA_CHOQUE))
      {
          // gira a la derecha
    _left_speed = MAX_SPEED - 9;
    _right_speed = -MAX_SPEED + 9.2;

      }


}



void MyRobot::seguir_pared_derecha()
{
     if (( ir_der < DISTANCIA_CHOQUE && ir_frontal < DISTANCIA_CHOQUE && ir_izq < DISTANCIA_CHOQUE) || (ir_izq > DISTANCIA_CHOQUE && ir_frontal < DISTANCIA_CHOQUE) || (ir_der > DISTANCIA_CHOQUE && ir_frontal < DISTANCIA_CHOQUE))
     {
         // movimiento de frente, ninguna pared cerca o pared a la izquierda
         
         _left_speed = MAX_SPEED-1;
         _right_speed = MAX_SPEED-1;


     }
     if ( ir_der > DISTANCIA_CHOQUE)
     {
     
         pared_iz = 0;
         pared_der = 1;

     }
     if (ir_izq > DISTANCIA_CHOQUE)
     {
         
         pared_iz = 1;
         pared_der = 0;
     }

     if (ir_der < DISTANCIA_CHOQUE && pared_der == 1 && (ir_frontal < DISTANCIA_CHOQUE) )
     {
         // esquina hacia la derecha, antes detectaba pared y ahora no

    _left_speed = MAX_SPEED - 8;
    _right_speed = -MAX_SPEED + 9;

     }
     
 
     if (( ir_izq > DISTANCIA_CHOQUE) || (ir_frontal > DISTANCIA_CHOQUE) || (ir_der > DISTANCIA_CHOQUE && ir_frontal > DISTANCIA_CHOQUE))
      {
          // gira a la izquierda

        _left_speed = -MAX_SPEED + 9;
        _right_speed = MAX_SPEED - 9;
      }


}

//nuevo metodo de deteccion definido
bool MyRobot::detectar_color(const unsigned char *image, int width, int height)
{
    int sum = 0;
    unsigned char red, green, blue;
    double percentage_color = 0.0;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            green = _forward_camera->imageGetGreen(image, width, x, y);
            red = _forward_camera->imageGetRed(image, width, x, y);
            blue = _forward_camera->imageGetBlue(image, width, x, y);
            // Verificar si el píxel está cerca del color objetivo (verde)
            if (abs(red) < THRESHOLD &&
                abs(green) > THRESHOLD &&
                abs(blue) < THRESHOLD) {
                sum = sum + 1;
            }
        }
    }

    percentage_color = (sum / (float)(width * height)) * 100;
    cout << "Percentage of color in image: " << percentage_color << " sum: " << sum << endl;

    return (percentage_color > DETECTION_THRESHOLD);
}

/////