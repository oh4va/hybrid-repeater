# hybrid-repeater
Mixed mode amateur radio repeater for analog and digital modes.

## Supported modes
* Analog FM (CTCSS 103.5 Hz)
* DMR (Connected to BrandMeister DMR Network, server BM2441)
* D-STAR (Connected to DCS010 O)
* Yaesu Fusion (No network connection)
* APRS AX.25 (Rx-only)

## Hardware
* Two Motorola (ex. Vertex) VX-2100 UHF Transceivers
* Raspberry Pi 3 running Pi-Star V4.0.0-RC4
* Repeater Builder STM32-DVM
* DMK Engineering URIx
* Self-made MMI (Multi-Mode Interface)
  * Bilateral Switch 4066
  * ATtiny85

## Software
* Pi-Star
* SvxLink
* DireWolf
* MMI (ATtiny85)

### MMI software running in ATtiny85

#### Main features
* Controls Bilateral Switch 4066 and transmitter PTT
* Selects transmitted signal from MMDVM or SvxLink
* Inputs: 
  * PTT signal from DMK Engineering URIx
  * PTT signal from MMDVM
* Outputs: 
  * Select transmitted audio from SvxLink
  * Select transmitted audio from MMDVM
  * Switch TX PTT ON/OFF
* Mode hang timer 20 seconds
* TX TOT timer 1 hour
