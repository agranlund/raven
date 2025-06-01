# WCHISPTool_CMD 
ISP Command Tool for WCH MCU

WCHISPTool_CMD is used to operate WCH MCU, which supports flash erase/verify/download, dataflash erase/download etc.

The macOS directory contains src, lib and bin directories.

src: source file corresponding to command tool

lib: ISP development dynamic library and related header files

bin: executable target program

The bin/WCHISPTool_CMD can quickly experience the use of the WCHISPTool_CMD tool on macOS.

Use the command line tool as described in the macOS Introduction section of documentation.



### Development Overview
1. Open xcode to create a project for command line tools
2. Add src/IspCmdTool.cpp and lib directory to your xcode project directory.
3. Add relevant parameters in edit scheme according to the documentation.
4. Users could modify the source code,  the development functions and instructions were all included in the header files WCH55XISPDLL.H etc. 
5.  Compile the project, when the compilation instruction is executed successfully, users could operate with the new program.
6. If you're going to publish your command line, please follow the Apple documentation for notarization 
  https://developer.apple.com/documentation/security/notarizing_macos_software_before_distribution