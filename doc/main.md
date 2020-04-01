# MKELibrary Documentation

Written according to MKELibrary v2.0

## What is MKELibrary?

MKELibrary contains all of the prerequisites for building a program to run on an MKE1xF microcontroller. In addition to low-level software adapted from the SDK, MKELibrary includes a number of libraries which can be used to simplify hardware interaction. 

## How to use a library

For every hardware library you use in your program, you need to begin with some initial setup. Here, the CAN library is used as an example. 

In abstract, every hardware library is represented by a class. To use the library, you will create a single instance of the class which will exist in the same location in memory throughout the runtime of your program. This ensures that any settings and any variables which represent the state of that hardware will be the same no matter where in the program they are observed from.

To gain access to a specific hardware library, include the header in which its class is defined: `#include "can.h"`. Note that all hardware libraries are defined in the `BSP` namespace, so it is useful to add `using namespace BSP;` after you include the header.

The initial setup of the hardware library typically happens in two stages: one stage before the class is initialized, and one after. The first configuration typically contains extremely low-level settings, and the defaults will almost always be fine.

The pre-initialization CAN configuration is done using a `canlight_config` struct. After changing whatever default settings are necessary, the struct is passed into the class constructor. The following lines show the form this typically takes.

```
can::canlight_config c;
can::CANlight::ConstructStatic(&c);
```

As explanation: `can` here is the namespace in which the CAN library is defined. `canlight_config` is the preinitialization settings struct; looking at `lib/can.h`, it is apparent that this struct only contains clock settings, with typical defaults assigned. `CANlight` is the actual name of the CAN library class. Finally, `ConstructStatic(&c)` invokes the `CANlight` constructor, creating a static instance of the class.

To access the actual hardware functions, it is necessary to create a local reference of the static class, like so: `can::CANlight& can = can::CANlight::StaticClass()`. This results in a local reference to the static class called `can`, the effect of which is that now CAN functions can be invoked as if it were a normal class: `can.tx(...)`, for example.

Typically, after the class is created, another setup step is necessary before the class can be used. In the case of CAN, this setup is done using a `canx_config` struct:

```
can::CANlight::canx_config can0config;
can0config.callback = CAN0CallbackFunction;
can.init(0, &can0config);
```

In this example, CAN bus 0 is initialized, using all the default settings in `canx_config`, with the exception of the callback function which is set to `CAN0CallbackFunction`. 

Once this setup step is finished, the CAN hardware can be used by calling other functions in the CAN library. In this case, the  only relevant function is the `tx()` function, which takes a `CANlight::frame` object:

```
can::CANlight::frame f;
// change the data in f as necessary...
can.tx(0, f);
```

Putting everything together, a complete `main.cpp` file might look like:

```
/* main.cpp: initialize the CAN driver and do nothing with it */
#include "MKE18F16.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "can.h"
using namespace BSP;

int main() {
    BOARD_InitBootClocks();
    BOARD_InitBootPins();

    can::canlight_config c;
    can::CANlight::ConstructStatic(&c);
	can::CANlight& can = can::CANlight::StaticClass();
	can::CANlight::canx_config cx;
	can.init(0, &cx);

    while(true);

}
```

Note that it is necessary to use the `StaticClass()` call in every function which contains calls to the library. For example, if there were written a custom `transmit_can()` function, the first line would probably be `can::CANlight& can = can::CANlight::StaticClass();`.

For specific information on practical implementations of each hardware library, see their associated documentation and demo code. 

## Changing MKELibrary

todo
