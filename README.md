# STM32 NTC Temperature Monitoring and Control (ADC + UART)

## Overview
This project demonstrates temperature measurement and threshold-based control using an **NTC thermistor** interfaced with an **STM32C0 series microcontroller**.  
The system reads analog voltage from an NTC voltage divider using the ADC, converts it into temperature using the **Beta equation**, and controls an LED based on a predefined temperature threshold.  
Measured temperature values are continuously sent to a PC via **UART** for monitoring and debugging.  

This project is implemented and tested using **Wokwi simulation** and STM32 HAL drivers.

---

## Features
- Analog temperature measurement using NTC thermistor  
- 12-bit ADC configuration with calibration  
- Temperature calculation using Beta parameter equation  
- Threshold-based LED control  
- UART serial output using `printf()` redirection  
- Bare-metal STM32 HAL programming (no RTOS)  

---

## Hardware / Simulation Setup
- **Microcontroller:** STM32C0 series  
- **Temperature Sensor:** 10kΩ NTC Thermistor (β = 3950)  
- **ADC Input:** PA0 (ADC Channel 0)  
- **LED Output:** PB6  
- **UART:** USART2  
  - TX: PA2  
  - RX: PA3  
- **Reference Voltage:** 3.3V  

Wokwi wiring is defined in `diagram.json`.

---

## Software Components Used
- STM32 HAL library  
- ADC (single-channel, polling mode)  
- GPIO (digital output)  
- UART (serial communication)  
- Math library (`log()` for thermistor calculation)  

---

## How the System Works
1. The NTC thermistor forms a voltage divider with a known reference resistor.  
2. The ADC samples the divider voltage on PA0.  
3. ADC raw value is converted to voltage and resistance.  
4. Temperature is calculated using the **NTC Beta equation**.  
5. If temperature exceeds the defined threshold (40 °C):  
   - LED turns ON  
   - Otherwise, LED turns OFF  
6. Temperature and LED status are printed via UART every 500 ms.  

---

## Temperature Calculation
The temperature is computed using the standard Beta equation:  
- Converts ADC value → Voltage  
- Voltage → Thermistor resistance  
- Resistance → Temperature (Kelvin → Celsius)  

This approach models real sensor behavior instead of using lookup tables.

---
