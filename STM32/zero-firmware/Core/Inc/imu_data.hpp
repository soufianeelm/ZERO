/**
  ******************************************************************************
  * @file           : imu_data.hpp
  * @brief          : Code for IMU data structure definition
  ******************************************************************************
  */
#include <cstdint>

#pragma once

struct imu_data {
	float ax, ay, az, gx, gy, gz;
	uint32_t timestamp;
};
