# Validation and Metrics

The C test suite covers protected identifier validation, checksum validation, a responding slave, a missing slave timeout and schedule continuation, and diagnostic request construction.

The Python simulator can generate a repeatable trace with a missing slave and summarize slot results. Track:
- sent/received frames;
- timeouts and recovered slots;
- schedule slot timing;
- bus utilization estimate;
- missed-response rate.

The simulator is not physical-bus timing validation. Use UART trace logs and a logic analyzer to measure real break/sync/PID timing, response windows and recovery behavior.
