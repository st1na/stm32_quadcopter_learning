# stm32_quadcopter_learning

Personal learning project for STM32-based quadcopter control software on STM32F405.

## Overview

This repository contains embedded firmware, STM32CubeIDE project files, peripheral configuration, and supporting source code used to explore quadcopter flight-control-related software development.

## Project goals

- practice embedded C development on STM32
- work with STM32CubeIDE / STM32CubeMX project structure
- explore sensor-fusion and flight-control concepts
- build familiarity with low-level firmware organization for UAV systems

## Repository contents

- `Core/` application source and headers
- `Drivers/` STM32 HAL and device support
- `Middlewares/` middleware components
- `USB_DEVICE/` USB device configuration and implementation
- `.ioc` STM32CubeMX configuration
- `.ld` linker scripts for STM32F405 target

## Third-party software

This project uses:
- Madgwick AHRS  
  License: GPLv3

## Notes

This is a personal learning project created for study and experimentation in embedded systems and quadcopter software development.
