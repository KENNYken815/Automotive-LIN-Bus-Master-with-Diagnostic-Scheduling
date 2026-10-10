# Hardware Integration

Suggested target: STM32 UART peripheral with LIN mode or NXP S32K LIN UART/USART plus a LIN transceiver.

Hardware adapter responsibilities:
- configure baud rate and break detection/generation;
- transmit break + 0x55 sync + PID;
- switch direction between master header and slave response window;
- collect response bytes via interrupt/DMA;
- timestamp frame completion;
- validate checksum and timeout;
- avoid blocking the scheduler during a missed response.

Keep schedule decisions and protocol checks in the portable C core; put UART registers, DMA setup and RTOS notifications in target-specific files.
