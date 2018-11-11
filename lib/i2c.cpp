/*
 * i2c.cpp
 *
 *  Created on: Nov 8, 2018
 *      Author: Arlette Olalde
 */
#include "i2c.h"
#include "fsl_lpi2c.h"

using namespace BSP::I2C;

I2C::I2C(const I2Cconfig& config): config(config)
{

}

void I2C::rx(callback_type& callback)
{

}
