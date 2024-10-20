# Raspberry Pico Greenhouse Controller

This project aims to control the CO<sup>2</sup>-levels of a green house using the Raspberry Pico W microcontroller.

The controller receives environmental data from external sensors using the Modbus interface and releases more CO<sup>2</sup> to the greenhouse as necessary.

The environmental data and the CO<sup>2</sup>-target level are reported to the ThingSpeak cloud service for long term monitoring.

User can also send new target levels remotely using the ThingSpeak TalkBack commands.
