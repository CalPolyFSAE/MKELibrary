# ADC

## Theory

The MKE18F512 has three ADC modules, ADC0-2. Each ADC has up to 16 external inputs, and can also be used to measure internal voltages.

Each ADC has a number of channels, which represent different configurations which can be maintained in parallel. Because only channel 0 can be triggered in software, only channel 0 should be used in most cases. 

## Basic Use

1. Construct the ADC class using an instance of `adc_config_t`, or `NULL` for default configuration. Note that the configuration passed to `ConstructStatic` will be applied to all three ADC modules.

2. Create a local reference `adc`, as shown.

```
ADC::adc_config_t conf;
conf.clock_source = kCLOCK_IpSrcFircAsync;
ADC::ADC::ConstructStatic(&conf);

// Create local reference
ADC::ADC& adc = ADC::ADC::StaticClass();
```

3. Configure the specific ADC module. If `NULL` was used when calling ConstructStatic(), `NULL` can be used here as well to take default configuration. The configuration object `adc12_config_t` allows for low-level configuration including of clock source and voltage reference. See `fsl_adc12.h` for complete type. 

``` 
adc12_config_t conf0 = {kADC12_ReferenceVoltageSourceVref, kADC12_ClockSourceAlt0, kADC12_ClockDivider1, kADC12_Resolution12Bit, 17U, true};
adc.config_base(ADC0, conf0);
```

4. Autocalibrate the ADC. Ensure the calibration is successful. Try changing the voltage reference if calibration fails.

```
if(adc.calibrate(ADC0) != kStatus_Success) assert(0);
```

5. Configure channel 0 to use the desired input.

```
adc12_channel_config_t channel_config;
// Use ADC pin 13
channel_config.channelNumber = 13U;
channel_config.enableInterruptOnConversionCompleted = false;
adc.config_channel(ADC0, 0, &channel_config);
```

6. Do a conversion using the ADC.

```
data = adc.read(ADC_BASE, 0);
```

## Advanced Use

### Using interrupts

todo

### Using hardware triggering

todo
