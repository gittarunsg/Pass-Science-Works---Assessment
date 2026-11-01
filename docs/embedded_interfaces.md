# Mock embedded integration interface table

| Module | Power / ground | Signal voltage assumption | Host / connector | Ownership and caveat |
|---|---|---|---|---|
| MCU evaluation board | Board powered independently; common ground only when signal interface requires it | 3.3 V I/O assumed | USB-C carrying UART | Candidate must check the exact board's USB and I/O voltage behavior |
| FPGA evaluation board | Board supply per vendor; common ground for debug signals | 1.8 V I/O bank assumed | USB-JTAG | Pin bank may differ by board; use level shifting where target and adapter domains are incompatible |
| JTAG adapter | Usually host/USB powered; target reference voltage sensing varies | Target-referenced, board-specific | Assumed 10-pin ARM-style connector | Adapter owns TCK/TMS/TDI/TDO/reset as applicable; it is a debug interface, not interchangeable compute hardware |

```text
Host PC --USB/UART--> MCU board
Host PC --USB-------> JTAG adapter --level translation if required--> FPGA JTAG header
Board supplies remain separate; connect grounds only according to board and adapter manuals.
```

All connector/pin/voltage entries above are illustrative assumptions, not verified device pinouts. Mock adapters do not communicate with hardware. Safe next step: select exact board SKUs and revisions, verify manuals, check power direction and absolute maximum ratings, then review signal levels before connecting.
