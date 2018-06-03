# OvenTemp
A portable, battery powered temperature monitor for ovens that don't have temperature displays.


## Requirements

1. Refreshes temperature every second during cooking
2. Is easily readable from 10 feet away
3. Turns on automatically at 120 F
4. Has a battery life of a month of normal use
   - Normal use is four uses in a week with two hours of active oven time each
   - 32 hours active, ~656 hours idle

## Current Measurements

To determine if I've met spec, I must profiled the current consumption under
various modes. I then can use those numbers to determine my expected battery
life.

### Arduino power measurements

With the Arduino architecture, I just wanted to get a proof of concept working
as well as a baseline for expected power consumption. I never go into low power
states and the Arduino code is pretty inefficient in it's design.

 * max brightness, no optimizations
      - Max: 47.692 mA
      - Min: 10.0 mA
      - Ave: 26.203 mA
 * min brightness, no optimizations
      - Max: 35.385 mA
      - Min: 8.4615 mA
      - Ave: 14.886 mA
 * min brightness, power down thermocouple
      - Max: 33.462 mA
      - Min: 8.4615 mA
      - Ave: 13.686 mA


### Arduino battery life calculation

Active current consumption = 26.203 mA
Idle current consumption   = 14.886 mA

Power consumption per oven use (2 hour active time) = (26.203 mA) * (2 Hours)
                                                    = 52.406 mAh
Expected weekly power consumption (4 oven uses)     = (52.406 mAh * 4) + (14.886 mA * 160 Hours)
                                                    = **2,591.3 mAh**

The Arduino architecture as it stands will barely last one week given my 2,700 mAh
battery.


### STM32F446 power measurements

After getting a baseline from the Arduino, I moved over to an STM32F446 dev board
I had laying around. This ARM chip from ST is a Cortex M3 (TODO: VERIFY) and is
not explicitly designed for power efficiency. ST has an L series for that, but I
didn't have one of those dev boards available at the time.

With this architecture I opted for a much more non-blocking design, allowing the
chip to enter low power states much more easily while things like ADC reads from
the thermocouple were running. The dev board also has much less peripherals on the
board than the Arduino taking up power.
