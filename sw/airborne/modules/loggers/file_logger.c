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
 *
 */

/** @file modules/loggers/file_logger.c
 *  @brief File logger for Linux based autopilots
 */

#include "file_logger.h"

#include <stdio.h>
#include "std.h"

#include "subsystems/imu.h"
#include "firmwares/rotorcraft/stabilization.h"
#include "firmwares/rotorcraft/guidance/guidance_indi.h"
#include "state.h"
#include "subsystems/actuators.h"
#include "generated/modules.h"

/** Set the default File logger path to the USB drive */
#ifndef FILE_LOGGER_PATH
#define FILE_LOGGER_PATH /data/video/usb
#endif

#ifndef FILE_LOGGER_DATETIME_NAME
#define FILE_LOGGER_DATETIME_NAME 0
#endif

#if FILE_LOGGER_DATETIME_NAME
#include <time.h>
#endif

/** The file pointer */
static FILE *file_logger = NULL;

/** Start the file logger and open a new file */
void file_logger_start(void)
{
  uint32_t counter = 0;
  char filename[512];

  // Check for available files
#if FILE_LOGGER_DATETIME_NAME
  time_t timer;
  char date_buffer[26];
  struct tm* tm_info;
  timer = time(NULL);
  tm_info = localtime(&timer);
  strftime(date_buffer, 26, "%Y_%m_%d__%H_%M_%S", tm_info);
  sprintf(filename, "%s/%s_%05d.csv", STRINGIFY(FILE_LOGGER_PATH), date_buffer, counter);
  while ((file_logger = fopen(filename, "r"))) {
    fclose(file_logger);
    counter++;
    sprintf(filename, "%s/%s_%05d.csv", STRINGIFY(FILE_LOGGER_PATH), date_buffer, counter);
  }
  file_logger = fopen(filename, "w");
#else
  sprintf(filename, "%s/%05d.csv", STRINGIFY(FILE_LOGGER_PATH), counter);
  while ((file_logger = fopen(filename, "r"))) {
    fclose(file_logger);
    counter++;
    sprintf(filename, "%s/%05d.csv", STRINGIFY(FILE_LOGGER_PATH), counter);
  }
  file_logger = fopen(filename, "w");
#endif
  uint8_t max_retries = 10;
  uint8_t cur_try = 0;
  while(file_logger == NULL && cur_try < max_retries){
	  cur_try++;
  }
  if (file_logger != NULL) {
    fprintf(
      file_logger,
      "counter,magneto_psi,euler_psi,rc_p,rc_q,rc_r,hrb_p, hrb_q, hrb_r\n"
      //"counter,RAW_mag_x, RAW_mag_y, RAW_mag_z, SCALED_mag_x, SCALED_mag_y, SCALED_mag_z, psi, state_0, state_1, state_2, state_3, state_4, state_5, state_6, state_7, state_8\n"
      // INDI LOG: "counter,pos_NED_x, pos_NED_y, pos_NED_z, filt_accel_ned_x, filt_accel_ned_y, filt_accel_ned_z, quat_i, quat_x, quat_y, quat_z,  sp_quat_i, sp_quat_x, sp_quat_y, sp_quat_z, sp_accel_x, sp_accel_y, sp_accel_z, accel_ned_x, accel_ned_y, accel_ned_z, speed_ned_x, speed_ned_y, speed_ned_z, imu_accel_unscaled_x, imu_accel_unscaled_y, imu_accel_unscaled_z, body_rates_p, body_rates_q, body_rates_r, actuaros_pprz_0, actuaros_pprz_1, actuaros_pprz_2, actuaros_pprz_3\n"
    );
    logger_file_file_logger_periodic_status = MODULES_RUN;
  }
}

/** Stop the logger an nicely close the file */
void file_logger_stop(void)
{
  if (file_logger != NULL) {
    fclose(file_logger);
    file_logger = NULL;
  }
  logger_file_file_logger_periodic_status = MODULES_IDLE;
}

/** Log the values to a csv file */
void file_logger_periodic(void)
{
  if (file_logger == NULL) {
    return;
  }
  static uint32_t counter = 0;
  struct FloatEulers* eulers = stateGetNedToBodyEulers_f();
  fprintf(file_logger, "%d,%f,%f,%d,%d,%d,%d,%d,%d\n",
            counter,
            magneto_psi_f,
            eulers->psi,
            ahrs_icq.rate_correction.p,
            ahrs_icq.rate_correction.q,
            ahrs_icq.rate_correction.r,
            ahrs_icq.high_rez_bias.p,
            ahrs_icq.high_rez_bias.q,
            ahrs_icq.high_rez_bias.r);
  /* MAGNETO LOG
  fprintf(file_logger, "%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
            counter,
            imu.mag_unscaled.x,
            imu.mag_unscaled.y,
            imu.mag_unscaled.z,
            MAG_FLOAT_OF_BFP(imu.mag.x),
            MAG_FLOAT_OF_BFP(imu.mag.y),
            MAG_FLOAT_OF_BFP(imu.mag.z),
            eulers->psi,
			mag_calib.state[0],
			mag_calib.state[1],
			mag_calib.state[2],
			mag_calib.state[3],
			mag_calib.state[4],
			mag_calib.state[5],
			mag_calib.state[6],
			mag_calib.state[7],
			mag_calib.state[8]);
			*/
/*  INDI LOG
  struct Int32Quat * quat       = stateGetNedToBodyQuat_i();
  struct NedCoor_f * pos        = stateGetPositionNed_f();
  struct NedCoor_f * accel_ned  = stateGetAccelNed_f();
  struct NedCoor_f * speed_ned  = stateGetSpeedNed_f();
  struct FloatRates* rates_body = stateGetBodyRates_f();
  fprintf(file_logger, "%d,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%f,%f,%f,%d,%d,%d,%d\n",
          counter,
          pos->x,
          pos->y,
          pos->z,
#if GUIDANCE_INDI_FILTER_ORDER == 2
          filt_accel_ned[0].o[0],
          filt_accel_ned[1].o[0],
          filt_accel_ned[2].o[0],
#else
          filt_accel_ned[0].lp2.o[0],
          filt_accel_ned[1].lp2.o[0],
          filt_accel_ned[2].lp2.o[0],
#endif
          quat->qi,
          quat->qx,
          quat->qy,
          quat->qz,
          stab_att_sp_quat.qi,
          stab_att_sp_quat.qx,
          stab_att_sp_quat.qy,
          stab_att_sp_quat.qz,
          sp_accel.x,
          sp_accel.y,
          sp_accel.z,
          accel_ned->x,
          accel_ned->y,
          accel_ned->z,
          speed_ned->x,
          speed_ned->y,
          speed_ned->z,
          imu.accel_unscaled.x,
          imu.accel_unscaled.y,
          imu.accel_unscaled.z,
          rates_body->p,
          rates_body->q,
          rates_body->r,
          actuators_pprz[0],
          actuators_pprz[1],
          actuators_pprz[2],
          actuators_pprz[3]
         );
*/
  counter++;
}
