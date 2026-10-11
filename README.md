# Automotive LIN Bus Master with Diagnostic Scheduling

An Embedded C reference implementation of a deterministic LIN master for automotive body-network concepts. It includes protected-ID and checksum helpers, a configurable schedule table, three simulated slave-node types, diagnostic-frame construction, timeout accounting and recovery that advances to later slots when a slave is missing. Python tools generate and analyze repeatable traffic traces.

> **Scope:** This is a portable software/reference model. It does not yet drive a physical LIN transceiver or generate the UART break/sync waveform on hardware. No physical timing, LIN conformance, or ECU validation is claimed.

## Architecture

    STM32 / NXP UART-LIN adapter (hardware integration)
                     |
             LIN master scheduler
                     |
          +----------+-----------+
          |          |           |
       Body node  Thermal node  Door node
          |          |           |
          +------ response / timeout ------+
                     |
          Diagnostics + statistics

## Implemented Features

### LIN protocol helpers
- Six-bit frame IDs and protected identifier parity.
- Classic and enhanced checksum calculation.
- Frame encode/validation helpers.
- Diagnostic frame IDs `0x3C` and `0x3D` use classic checksum in the modeled profile.

### Configurable deterministic schedule
- Fixed-size schedule table with periodic slot delays.
- Unconditional data slots.
- Diagnostic slot.
- Slot index/cycle tracking.
- Failed slot does not halt the schedule; the engine advances and records recovery.

### Three simulated slave nodes
- Body node.
- Thermal/sensor node.
- Door-control node.
- Configurable presence and response delay.
- Responses contain example signal bytes and validated checksums.

### Failure detection and recovery
- Missing response timeout.
- PID parity validation.
- Checksum validation.
- TX/RX/timeout/error/recovery counters.
- Recovery continues to the next configured slot.

### Diagnostics
- Diagnostic request construction on master request ID `0x3C` using the classic checksum.
- Diagnostic payload layout modeled as NAD, PCI length, service ID, and service data; targeted requests use the selected simulated slave's node ID.
- Diagnostic request counter. The scheduled diagnostic slot uses a sample broadcast NAD (`0x7E`); diagnostic slave responses are not simulated.

### Python tools
- `tools/lin_traffic_sim.py` generates repeatable schedule traces and can simulate a missing slave.
- `tools/analyze_trace.py` summarizes trace outcomes and estimates successful-slot ratio as a simple bus-utilization proxy.

## Repository Structure

    Automotive-LIN-Bus-Master-with-Diagnostic-Scheduling/
    +-- docs/
    |   +-- ARCHITECTURE.md
    |   +-- HARDWARE_INTEGRATION.md
    |   +-- PROTOCOL.md
    |   +-- VALIDATION.md
    +-- examples/lin_demo.c
    +-- include/
    |   +-- lin_master.h
    |   +-- lin_protocol.h
    |   +-- lin_schedule.h
    |   +-- lin_sim.h
    |   +-- lin_types.h
    +-- src/
    |   +-- lin_master.c
    |   +-- lin_protocol.c
    |   +-- lin_schedule.c
    |   +-- lin_sim.c
    +-- tests/test_lin.c
    +-- tools/
    |   +-- analyze_trace.py
    |   +-- lin_traffic_sim.py
    +-- .gitignore
    +-- LICENSE
    +-- Makefile
    +-- README.md

## Build and Run

Requirements: a C11 compiler such as GCC or Clang, GNU Make, and Python 3 for the simulator tools.

Build demo and C tests:

    make

Run the master demo:

    make demo

Run unit tests:

    make test

Generate a repeatable trace while node index 2 is missing:

    python3 tools/lin_traffic_sim.py --cycles 20 --missing-slave 2 --csv lin_trace.csv

Analyze the trace:

    python3 tools/analyze_trace.py lin_trace.csv

Clean compiled artifacts:

    make clean

## Suggested Demonstration

1. Start with all three simulated slaves present and inspect received-frame counters.
2. Mark the door node missing and rerun the schedule.
3. Observe a timeout for its slot, incremented recovery count, and continued execution of later slots.
4. Trigger diagnostic request construction and inspect its frame ID/data.
5. Introduce a corrupted PID/checksum in a test and verify rejection.
6. Export a Python-generated CSV trace and calculate missed-response rate.

## Hardware Integration: UART/LIN

The current reference implementation uses three simulated slave nodes. A physical STM32 or NXP implementation must supply the controller-specific hardware layer to:
- generate a LIN break and `0x55` sync byte followed by the protected identifier;
- transmit/receive with UART LIN mode or timer/UART support;
- use interrupts/DMA for frame bytes;
- timestamp response windows;
- detect timeout and checksum faults;
- continue the schedule after failed response;
- drive a proper LIN transceiver.

The portable model does not generate the electrical break waveform. Confirm exact LIN 2.x behavior against the specification and target MCU documentation before a compliance-oriented implementation.

## Test and Measurement Plan

Software tests are provided for PID validation, checksums, successful slave response, missing-slave timeout, schedule continuation and diagnostic payload construction. Run `make test` locally to execute them; this session verified repository contents but did not run the build. Hardware measurements should include:
- header break/sync/PID timing;
- slot-start jitter;
- response latency;
- missed-response rate;
- checksum-error rate;
- successful-slot ratio / bus utilization estimate;
- recovery behavior over repeated faults.

Do not present simulated values as measured physical bus performance.

## License

MIT License.
