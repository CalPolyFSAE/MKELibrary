# Structure
The CAN driver is a wrapper around the fsl_can driver provided by NXP.

## Layout
There are two classes that track and manipulate the state of two CAN peripherals in the MCU. The first class, CAN, is the primary driver used to interact with hardware. It provides all functionality in the form of member functions. The other class, mb_info, is an internal class used to configure the message buffer hardware. It should not be used outside the CAN driver. 

Functionality is split similar to how the hardware is configured. There are message buffers, which contain the information needed to send and receive CAN messages, and the main controller hardware. There are many message buffers and a single main controller per CAN peripheral. As a result of this, the CAN class has 16 static instances of mb_info per CAN peripheral. One to control each of the 16 physical message buffers.

## Hidden fsl_can structs
The can.h file does not include fsl_can.h

The fsl_can driver headers are very cluttered from the lack of namespaces in C. To prevent all of these declarations being included in various other places throughout the program, all structs stored for interaction with the NXP drivers are stored in private forward declared structs and dynamically instantiated. The forward declared structs are defined inside the .cpp file where the fsl_can.h file can be included.

# Usage
Driver must be constructed early in program startup using the standard ConstructStatic(Params...).

```
BSP::CAN::can_config ar;
BSP::CAN::CAN::ConstructStatic(&ar);
```

can_config contains two pointers to CAN bus configurations. 
The configurations contain peripheral settings. Leaving pointer to controller_config disables peripheral.


