/**
  ******************************************************************************
  * File Name          : imu_data.hpp
  * Description        : Code for IMU data structure definition
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <cstdint>

#pragma once

struct imu_data {
	float ax, ay, az, gx, gy, gz;
	uint32_t timestamp;
};
