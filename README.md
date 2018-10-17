# MKELibrary


## Code to write

Organized by source library.

### PORT and GPIO

These functions should be a part of GPIO class.
See chapters 34, 35, and 36 in Reference.

Functions
- General pin configuration
  - Use PORT driver to configure pin function, pull direction, pull strength, etc.
    - Pass a configuration struct as argument.
    - Low priorite: confirm that configuration is valid (will depend on specific pin being configured, see datasheet)
- Enable pin interrupt
  - Pass by address the function to be called when pin interrupts
  - Also configure interrupt mode (reference p.779)
- Disable pin interrupt

GPIO Tick
- Determine which pin caused interrupt and respond accordingly.
  - See reg PORTx_ISFR (reference p.782)
- Clear interrupt, prepare to receive interrupt again if the interrupt has not been disabled.

### ADC

See chapter 37 in Reference, chapter 6 in SDK.
Note that ADC0, 1, and 2 do not have the same number of pins.

Functions
- Configuration
- Do conversion: no interrupt
  - Pass ADC number and channel number as arguments.
  - Don't interrupt after conversion complete
- Do conversion: interrupt
  - Pass ADC number, channel number, and address of where to put conversion. 
    - Store address in ADC static class
  - Enable specific ADC interrupt

Interrupt handlers
- Conversion finished
  - Move conversion value into the requested address if necessary

### I2C

« make this one better »

Functions
- Initial configure
- Transmit
- Enable receive, with/without interrupts

Interrupt handlers
- Receive 
  - Move packet data somewhere safe

### SPI

« also this one » 

- Todo similar to I2C

### FLEXCAN

« same » 

- Todo similar to I2C
