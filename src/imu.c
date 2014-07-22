#include "quadcopter.h"
#include "simpletools.h"
#include "simplei2c.h"
#include <propeller.h>
#include <stdio.h>

i2c imu;

volatile signed short gx;
volatile signed short gy;
volatile signed short gz;
volatile signed short ax;
volatile signed short ay;
volatile signed short az;
volatile int lock;

void imuInit()
{
  lock = 0;
  i2c_open(&imu, PIN_IMU_SCL, PIN_IMU_SDA, 0);
  // Gyro initialization.
  // 22 -> 11011  Set internal clock and scale.
  // 21 -> 9      Set sample rate.
  // 23 -> 101    Trigger interrupt when new data is ready (extra int pins).
  // 62 -> 1      Set clock source.
  writeToRegister(&imu, GYRO_ADDR, 0x16, 0x1A);
  writeToRegister(&imu, GYRO_ADDR, 0x15, 0x09);
  writeToRegister(&imu, GYRO_ADDR, 0x17, 0x05);
  writeToRegister(&imu, GYRO_ADDR, 0x3E, 1);
  
  // Accel initialization.
  // 45 -> 1000, 100  Wake it from sleep, keep it awake.
  // 49 -> 01    Set the data range. 00->2, 01->4, 10->8, 11->16 (+- g).;
  writeToRegister(&imu, ACCL_ADDR, 0x2D, 16);
  writeToRegister(&imu, ACCL_ADDR, 0x2D, 8);
  writeToRegister(&imu, ACCL_ADDR, 0x31, 0);
  //writeToRegister(&imu, ACCL_ADDR, 0x1E, -25/-4); -25
  //writeToRegister(&imu, ACCL_ADDR, 0x1F, 12/-4);   12
  //writeToRegister(&imu, ACCL_ADDR, 0x20, 249/4);  249
}

void imuUpdate()
{
  while(lock==1);
  lock = 1;
  gx = (signed short) readValue(&imu, GYRO_ADDR, GYRO_REG_X, 0);
  gy = (signed short) readValue(&imu, GYRO_ADDR, GYRO_REG_Y, 0);
  gz = (signed short) readValue(&imu, GYRO_ADDR, GYRO_REG_Z, 0);

  ax = (signed short) readValue(&imu, ACCL_ADDR, ACCL_REG_X, 1);
  ay = (signed short) readValue(&imu, ACCL_ADDR, ACCL_REG_Y, 1);
  az = (signed short) readValue(&imu, ACCL_ADDR, ACCL_REG_Z, 1);
  lock = 0;
}

void getImuData(signed short* ngx, signed short* ngy, signed short* ngz, signed short* nax, signed short* nay, signed short* naz)
{
  while(lock==1);
  lock = 1;
  *ngx = gx;
  *ngy = gy;
  *ngz = gz;

  *nax = ax;
  *nay = ay;
  *naz = az;
  lock = 0;
}

void imuRun()
{
  while(1)
  {
    waitcnt(CNT + CLKFREQ/10);
    imuUpdate();
  }
}