# Info:
- Simm modules should be manufactured with a thickness of 1.2 mm
- They are probably be a tiny bit too wide but do fit in the socket ok. Would be good to shave off a millimeter or so in a future update.


### License:
Unless stated otherwise these hardware designs are licensed under CERN-OHL-P v2
See LICENSE.md or visit https://cern.ch/cern-ohl


### Sub projects:
- breakout
	- simple simm breakout module for debugging. untested.
- panel
	- template for merging two simms onto one board separated by mouse-bites
- programmer
	- Pico based programmer for ROM simm modules
- ram_16M55
	- 16MB 55ns RAM module
- ram_8M10
	- 8MB 10ns RAM module
- rom_16M55
	- module with SMD roms (recommended)
- rom_2M55
	- module for 4x socketed PLCC roms
- template
	- blank simm module template


### Pinout:

1. vcc     * 
2. a2
3. a3
4. a4
5. a5
6. a6
7. a7
8. a8
9. a9
10. gnd     *
11. d31
12. d30
13. d29
14. d28
15. d27
16. d26
17. d25
18. d24
19. gnd     *
20. d23
21. d22
22. d21
23. d20
24. d19
25. d18
26. d17
27. d16
28. gnd     *
29. d15
30. d14
31. d13
32. d12
33. d11
34. d10
35. d9
36. d8
37. gnd     *
38. d7
39. d6
40. d5
41. d4
42. d3
43. d2
44. d1
45. d0
46. gnd     *
47. a21
48. a20
49. a19
50. a18
51. a17
52. a16
53. gnd     *
54. a15
55. a14
56. a13
57. a12
58. a11
59. a10
60. a9
61. a8
62. gnd     *
63. b0
64. b1
65. b2
66. b3
67. nc      -
68. oe
69. we
70. nc      -
71. nc      -
72. vcc     *
