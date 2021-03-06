/*
 * Copyright (C) 2014 Freek van Tienen <freek.v.tienen@gmail.com>
 *
 * This file is part of paparazzi.
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * @file subsystems/imu/imu_bebop.h
 * Interface for the Bebop magnetometer, accelerometer and gyroscope
 */


#ifndef IMU_BEBOP_H
#define IMU_BEBOP_H

#include "generated/airframe.h"
#include "subsystems/imu.h"

#include "peripherals/ak8963.h"
#include "peripherals/mpu60x0_i2c.h"

#ifndef BEBOP_GYRO_RANGE
#define BEBOP_GYRO_RANGE MPU60X0_GYRO_RANGE_1000
#endif

#ifndef BEBOP_ACCEL_RANGE
#define BEBOP_ACCEL_RANGE MPU60X0_ACCEL_RANGE_8G
#endif

// Set default sensitivity based on range if needed
#if !defined IMU_GYRO_P_SENS & !defined IMU_GYRO_Q_SENS & !defined IMU_GYRO_R_SENS
#define IMU_GYRO_P_SENS MPU60X0_GYRO_SENS[BEBOP_GYRO_RANGE]
#define IMU_GYRO_P_SENS_NUM MPU60X0_GYRO_SENS_FRAC[BEBOP_GYRO_RANGE][0]
#define IMU_GYRO_P_SENS_DEN MPU60X0_GYRO_SENS_FRAC[BEBOP_GYRO_RANGE][1]
#define IMU_GYRO_Q_SENS MPU60X0_GYRO_SENS[BEBOP_GYRO_RANGE]
#define IMU_GYRO_Q_SENS_NUM MPU60X0_GYRO_SENS_FRAC[BEBOP_GYRO_RANGE][0]
#define IMU_GYRO_Q_SENS_DEN MPU60X0_GYRO_SENS_FRAC[BEBOP_GYRO_RANGE][1]
#define IMU_GYRO_R_SENS MPU60X0_GYRO_SENS[BEBOP_GYRO_RANGE]
#define IMU_GYRO_R_SENS_NUM MPU60X0_GYRO_SENS_FRAC[BEBOP_GYRO_RANGE][0]
#define IMU_GYRO_R_SENS_DEN MPU60X0_GYRO_SENS_FRAC[BEBOP_GYRO_RANGE][1]
#endif

// Set default sensitivity based on range if needed
#if !defined IMU_ACCEL_X_SENS & !defined IMU_ACCEL_Y_SENS & !defined IMU_ACCEL_Z_SENS
#define IMU_ACCEL_X_SENS MPU60X0_ACCEL_SENS[BEBOP_ACCEL_RANGE]
#define IMU_ACCEL_X_SENS_NUM MPU60X0_ACCEL_SENS_FRAC[BEBOP_ACCEL_RANGE][0]
#define IMU_ACCEL_X_SENS_DEN MPU60X0_ACCEL_SENS_FRAC[BEBOP_ACCEL_RANGE][1]
#define IMU_ACCEL_Y_SENS MPU60X0_ACCEL_SENS[BEBOP_ACCEL_RANGE]
#define IMU_ACCEL_Y_SENS_NUM MPU60X0_ACCEL_SENS_FRAC[BEBOP_ACCEL_RANGE][0]
#define IMU_ACCEL_Y_SENS_DEN MPU60X0_ACCEL_SENS_FRAC[BEBOP_ACCEL_RANGE][1]
#define IMU_ACCEL_Z_SENS MPU60X0_ACCEL_SENS[BEBOP_ACCEL_RANGE]
#define IMU_ACCEL_Z_SENS_NUM MPU60X0_ACCEL_SENS_FRAC[BEBOP_ACCEL_RANGE][0]
#define IMU_ACCEL_Z_SENS_DEN MPU60X0_ACCEL_SENS_FRAC[BEBOP_ACCEL_RANGE][1]
#endif

extern float imu_bebop_pitch_offset;
extern bool imu_bebop_factory_calib;

/** Everything that is in the bebop IMU */
struct ImuBebop {
  struct Mpu60x0_I2c mpu;         ///< The MPU gyro/accel device
  struct Ak8963 ak;               ///< The AK8963 mag
};

extern struct ImuBebop imu_bebop;

extern float imu_bebop_filtered_temperature;
extern double gyro_x_sfe, gyro_y_sfe, gyro_z_sfe;
extern double accel_x_sfe, accel_y_sfe, accel_z_sfe;

extern void imu_bebop_init(void);
extern void imu_bebop_periodic(void);
extern void imu_bebop_event(void);

#endif /* IMU_BEBOP_H */
