# KeyenceLaserSerialReader

This C++ program was made to read data from a Keyence Laser, the laser head used was an IL-300, connected with a single IL-1000 amplifier, which output through RS-232C Communication unit. It should work for any RS-232 serial commmunication and I've tested it with another device and the program still works.

To change the command being sent to the device, change the variable "testval" at the beginning of the code. To change the COMPort being written/read, change the end of variable ComPortName. The backslashes are there to solve a problem with communicating with ports higher than 9.

It was written in Visual Studio Community 2017 on Windows 10. If you get an error message about precompiled headers, disable them as this program doesn't use them. In VSC that is found under Project, Properties, C/C++, Precompiled Headers. In there, switch precompiled header to "Not Using Precompiled Headers".

It works by opening the serial port, sending a "read" command, then receiving the information from the sensor, and placing it in a .CSV file that gets created in whatever folder the program is in. To avoid any possible file deletion, place the program in its own folder, as to avoid erasing an imporant pre-existing .CSV Each reading is separated by a comma, which allows the data to be opened in Excel as a CSV, with each reading in its own cell.

The input to the laser needs to conclude with: "\r\n" or a similar command. Without that, the laser will not send any data back, usually not even error responses. Most devices do not require this and the program itself will work without these.

The output of the laser has many characters that aren't the actual reading, so they are removed from the string in lines 178 and 179. If you want the whole output of the device, remove those two lines.
