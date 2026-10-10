# LIN Protocol Notes

- Frame IDs are six-bit values (0x00-0x3F).
- Protected Identifier (PID) contains ID plus parity P0/P1.
- Classic checksum is the inverted end-around-carry sum of data bytes.
- Enhanced checksum includes the PID; diagnostic IDs 0x3C/0x3D use classic checksum in the common LIN 2.x profile.
- The master schedule separates unconditional data frames from a diagnostic slot.

This project implements protocol concepts for education. Use the applicable LIN specification and vendor hardware documentation for a compliance-grade implementation.
