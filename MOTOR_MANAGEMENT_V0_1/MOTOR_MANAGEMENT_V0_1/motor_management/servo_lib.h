/**
\file     motor_management/servo_lib.h
\ingroup  servo_lib
\brief    Implementation of the Servo control.
\author   Benjamin Marty
*/


#ifndef SERVO_LIB_H
#define SERVO_LIB_H

/** \brief Init Function for Servo 1
*/
void init_servo_1( void );

/** \brief Control Function to position the Servo

  \param Servo position in grad (0-180)
*/
void control_servo_1( char grad );


/** \brief Init Function for Motor 1
*/
void init_motor_1( void );

/** \brief Control Function to set the Motor Speed

  \param Motor speed in grad (0-180)
*/
void control_motor_1( int power );


#endif /* SERVO_LIB_H */