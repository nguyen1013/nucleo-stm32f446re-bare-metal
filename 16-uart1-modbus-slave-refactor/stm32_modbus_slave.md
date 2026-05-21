# STM32 Modbus RTU Slave Documentation

## 1. Introduction

This document describes the design and usage of a Modbus RTU Slave implementation on STM32 microcontrollers. The system is built using a layered architecture separating hardware, protocol, and application logic.

## 2. Architecture

### Layers
- Application (sensor / register data)
- Modbus protocol (frame processing and dispatch)
- Port layer (UART + timer)
- Hardware

## 3. Communication Flow

1. UART receives data (interrupt)
2. Timer detects end of frame (T3.5)
3. Main loop processes frame
4. Modbus dispatches to handler

## 4. Frame Format

[Address][Function][Data][CRC]

## 5. Processing Steps

- Check length
- Check address
- Validate CRC
- Dispatch handler
- Send response or exception

## 6. Supported Function Codes

- FC04: Read Input Registers
- FC03: Read Holding Registers
- FC06: Write Single Register
- FC16: Write Multiple Registers

## 7. Application API

int MB_App_ReadInputRegisters(uint16_t addr, uint16_t qty, uint16_t *outBuf);

## 8. Error Handling

- CRC error → silent drop
- Invalid request → exception response

## 9. Conclusion

The system provides a clean, scalable and industrial-ready Modbus RTU slave implementation.
