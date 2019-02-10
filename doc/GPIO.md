# GPIO 

### Setup

Initialize class with no arguments and create a local reference like so:
```
gpio::GPIO::ConstructStatic();
gpio::GPIO& gpio = gpio::GPIO::StaticClass();
```

GPIO functions can now be called like `gpio.f()`.

### Basic Use

Most GPIO functions take only a port name and pin number as arguments.

```
// Set pin A1 high
gpio.set(gpio::PortA, 1);
```

##### As Output

A pin's data direction is set as an output when `gpio.out_dir()` is called, or the output level is configured using `gpio.set()`, `gpio.clear()`, or `gpio.toggle()`.
While a pin is an output, its logic level can be set using `gpio.set()` for logic high, `gpio.clear()` for logic low, or `gpio.toggle()` to toggle logic level.
`gpio.read()` can be used to read a pin's current value.

##### As input

A pin's data direction is set as input when `gpio.in_dir()` is called.
`gpio.read()` can be used to read a pin's current value.

### Interrupts

There is one interrupt associated with each port, but each pin can be configured to trigger it in different ways. 
Interrupt modes are enumerated in port_interrupt_t, including `kPORT_InterruptLogicZero` (interrupt whenever pin is low), `kPORT_InterruptRisingEdge` (interrupt on rising edge), and similar. Configuring and enabling a pin's interrupt is done using a single function call, i.e.

```
gpio.config_interrupt(gpio::PortA, 1, kPORT_InterruptRisingEdge);
```

To disable an interrupt, pass `kPORT_InterruptOrDMADisabled` as the final argument.

##### Writing Interrupts

The proper form of an interrupt handler takes no arguments and returns nothing:

```
void callback(void){
...
```

Set a function to be called by an interrupt by passing it into `config_function()`.

```
config_function(gpio::PortA, callback);
```

Because all pins in a port share the same interrupt, it is up to the user to determine which pin caused it, such as with `int_source()`:

```
void callback(void){
  while(int_source(gpio::PortA) < 32){

    i = int_source(gpio::PortA);

    if(i == 3){
      [do something]
        ack_interrupt(gpio::PortA, 3);
    } else if(i == 12) {
      [do something else]
        ack_interrupt(gpio::PortA, 12);
    } else ...
```

Note also that a pin interrupt must be acknowledged with `ack_interrupt()` after being handled. The interrupt function will called again instantly upon completion while there are unacknowledged interrupts. 

### Advanced configuration

Advanced configuration of a single pin is done using `gpio.config_pin()`. Options are held in a `port_pin_config_t` struct, and include pull, filtering, and others. Not all configurations are possible on every pin; reference the datasheet.

