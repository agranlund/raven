## ISA card drivers

### gus
Gravis Ultrasound initialization. AMD Interwave version. May or may not work, or be needed for the non-pnp version.
Initializes the synth + mixer and sets up sensible default volumes.


### picogus
Atari version of pgusinit.


### NE2000/ethernec
This is the original and unmodified Ethernec drivers. The Hades version is compatible (ENEH.MIF)
To use it with Mint, copy it to the Mint folder and rename to ENEH.XIF. It is hardcoded to look for the network card at port $300.
todo: rewrite this and use interrupts instead of polling at 5ms intervals. Also ask ISA_BIOS before assuming port $300.

