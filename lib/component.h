/*
 * component.h
 * 
 * Description: Highest level description structures for hardware. These are used
 *              in the driver api implementation. The api is aware of these objects,
 *              but does not define them. The component file allows for implementation
 *              to 
 * 
 *  Created on: Mar 5, 2019
 *      Author: Hunter Borlik
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_


#include "MKE18F16.h"
#include "fsl_clock.h"

/* declare the can api */
namespace hal::can {
    class can_api;


    /* CAN object type */
    struct can_object_t {
        clock_ip_name_t 	clock;      // CLOCK
        IRQn				irqn;       // IRQ to attach at
        IRQn				irqn_err;   // CAN ERROR IRQ
        CAN_Type* 			base;       // Hardware peripheral base address
        can_api* 	        driver;     // pointer to driver
    };

    /* Can config type */
    struct can_config_t {
        uint32_t 			baud			= 1000000;
        bool 				enableLoopback	= false;
        bool 				enableSelfWake	= false;
    };

}

#endif /* COMPONENT_H_ */
