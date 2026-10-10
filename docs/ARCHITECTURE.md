# Architecture

The master is split into protocol helpers, a deterministic schedule table, slave simulation and master orchestration. The C core builds on a desktop host but keeps boundaries suitable for STM32/NXP UART/LIN integration.

Header generation on a real UART/LIN controller requires a break field, sync byte 0x55, and protected identifier. The current portable model computes the protected identifier and frame checksum; the MCU driver must generate the actual break/sync waveform or use the controller's LIN mode.

The reference simulator uses three node types: body, thermal and door. A slot associated with a missing slave times out, increments fault/recovery counters, and the scheduler advances to the next slot instead of blocking the whole table.
