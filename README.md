# sensor-firmware

## Project setup

Make sure you have the **nRF Connect** extension installed for VS Code with 3.0.2 toolchain SDK.

1. Initialize the project:

   ```bash
   west init -l app
   west update
   ```

2. Build and flash the app for `promicro_nrf52840` using J-Link through the **nRF Connect** extension. The J-Link programmer has to be connected with SWD to the board.
