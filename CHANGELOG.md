# Changelog

## ToDo

- Read all values from ESP32
- Rewrite tasks
- Add gamma correction for linear brightness change.

## FixMe
- Correct standby color getter.
- Unexpected value changes (Check interrupts and unhandled expections)

## [1.0.4] - 2020-01-05

### Added

- Read color values from ESP32

## [1.0.3] - 2020-01-04

### Added

- Standby light control (Controller can slightly glow when wait for motion detection)
- Preview control on front end side

### Channged

- Rewrited control functions

## [1.0.2] - 2020-01-02

### Added

- Build-in ESP32 system LED as motion detection indicator


## [1.0.1] - 2019-12-31

### Added

- Threshold control via photoresistor (ADC)
- Schematic diagram

## [1.0] - 2019-12-29

### Added

- PWM control for RGBW LED stripe.
- Duration, fade in and fade out control 