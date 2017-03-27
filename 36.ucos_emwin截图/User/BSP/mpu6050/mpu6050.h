#ifndef __BSP_MPU6050_H
#define __BSP_MPU6050_H

#include  "stm32f4xx.h"

typedef struct
{
  int16_t   MPU6050_Gyro_Raw[3];
  int16_t   MPU6050_Accel_Raw[3];
	float     MPU6050_Pitch;
  float     MPU6050_Roll;
  float     MPU6050_Yaw;
  float     MPU6050_Temperature;
  unsigned long  MPU6050_Step_Count;
  unsigned long  MPU6050_Walk_Time;
}MPU6050_DATA_T;


uint8_t mpu6050_init(void);
void MPU6050_DataUpdate(MPU6050_DATA_T *mpu6050_data);
#endif  //__BSP_MPU6050_H
