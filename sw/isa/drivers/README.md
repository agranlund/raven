## ISA card drivers

### gus
Gravis Ultrasound initialization. AMD Interwave version. May or may not work, or be needed for the non-pnp version.
Initializes the synth + mixer and sets up sensible default volumes.


### picogus
Atari version of pgusinit.


### NE2000/ethernec
This is the original and unmodified Ethernec drivers. The Hades version is compatible (ENEH)
The MagiC version works with mint if you rename ENEH.MIF to ENEH.XIF
Hardcoded to IO port $300.

todo: rewrite this and use interrupts instead of polling at 5ms intervals. Also ask ISA_BIOS before assuming port $300.

