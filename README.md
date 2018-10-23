## **Multiple sensors' data collecting by Arduino Uno and transfering via virtual RS-232**

### Goal
Collect data from different sensors and measure conflict. <br/>

### Description
step 1. Use six analog sensors (TMP36) for measuring temperature in same spot and converting them into voltage values. <br/>
Then connect TMP36s with analog inputs on an Arduino Uno R3 board, which is used as an analog-to-digital converter (ADC). <br/>
step 2. Next, the Uno translate those six voltage signal into digital data, and a periodic collecting program inside the Uno transmits them into a laptop through USB and virtual COM port. <br/>
step 3. In the laptop, a serial port access program reads the voltage data coming from the virtual COM port, and converts them into temperature values as well as saves those values to a text file with time-stamp.  <br/>
Finally, use Matlab for analyzing the conflict between temperature data from different sensors. <br/> <br/>

![Alt text]( architecture.png?raw=true "")<br/>

### Front-end data acquisition program
Arduino Uno R3 functions like an ADC as well as transmits the data to a laptop. A data acquisition program runs in Uno. This program reads six analog inputs periodically (in second interval), creates data frames, and sends those data frames to a laptop through USB and virtual COM port.  <br/>
![Alt text]( data_frame_format.png?raw=true "")<br/>

### Back-end receiving program
A back-end program (on laptop) inquires the virtual COM port periodically, gets every voltage value from those six sensors, translates them into temperature values, tags them with timestamp, and saves them into a text file for later Matlab analysis. <br/>
![Alt text]( screenshot01.jpg?raw=true "")<br/>
