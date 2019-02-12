# TUC Course: Analysis and Design (Synthesis) of Telecommunication Systems
## Project: Distributed Smoke Detection using Lasers

### Development Team Members
Andrianakis Manos
Fotakis Tzanis
Giannakopoulos Orestis
Kotridis Giorgos
Neonakis Paulos
Nichoritis Thanos
Ouroutzoglou Michail
Piskopos Dionisis
Polyzos Christos
Topalis Thanasis

### Professor Assistants
Giannelos Vaggelis
Vougioukas Giorgos

## Supervisor Professor
Bletsas Aggelos

# Introduction
---

## Semester Project Overview
The purpose of this project is to create a wireless sensor network in order to detect smoke in the TUC ECE's department building canteen and pinpoint its source.

### Main idea
Smoke has the ability to diffuse and absorb light, thus, by utilizing cheap sensors such as photodiodes, phototransistors or even photovoltaic panels, the impinging light’s intensity can be tracked and from its waveform in respect to time the existence of smoke can be inferred.
In order to cover a large amount of the available plane, lasers can be used in combination with mirrors as shown in Fig.1. By dividing the plane into cells/tables of 2 rows and 3 columns (see Fig.1), detection of smoke in a specific cell/table can be done by combining data from multiple sensors (e.g., row-1 and column-2 could detect smoke from table-2). 

### Project Structure
Work was split into 3 working groups, Hardware, Middleware and Software, in order to divide the workload among the project members. 
The system includes five sensor nodes (slaves, depicted in Fig.2-right), one base node (master, depicted in Fig.3) and one server PC (laptop, depicted in Fig.4). 
    • Slave: Controls laser diode, measures laser intensity and sends the measurement to the master node.
    • Master: Gives instructions to slave nodes so that they get synchronized to make the laser intensity measurements and to report them back on specific time slots. After the collection of all the measurements, a message to the server is sent with the collected data.
    • Server: Collects measured data, runs the detection algorithm on the collected data and updates the website.

![Network Top View](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/01-NetworkTopView.png "Network Top View")
Figure 1: Initial idea. Top view.

![Slave Node](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/02a-SlaveNode.png "Slave Node")
![Mirror](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/02b-Mirror.png "Mirror")
Figure 2: Slave Node on the left, mirror on the right.

![Master Node](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/03-MasterNode.png "Master Node")
Figure 3: Master Node.

![Server Laptop](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/04a-ServerLaptop.png "Server Laptop")
![Server RPi](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/04b-ServerRPi.png "Server RPi")
Figure 4: Server on a laptop on the left, Server on a Raspberry Pi Zero W on the right.

![Fully Connected System](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/05-FullyConnectedSystem.png "Fully Connected System")
Figure 5: Fully connected System.

### Technical Difficulties
Due to environmental difficulties (walls prone to machinery vibrations, mounting issues, etc) that will be discussed below, time limitations became significant to the project’s overall development. As a result, the initial goal of detecting smoke on a grid was transformed into detecting smoke on a serial basis.

# Implementation
---

## Hardware

In this section we shall describe the analog part of our system. 

First of all, we shall analyze the basic hardware, as well as the installation components, that compose the system’s slave nodes.
Components:
    • Diode Laser (red)
    • Photovoltaic Panel
    • Power Supply
    • Mirror reflectors
    • Switching circuit
    • Optical filter
    • Control Unit & Radio
    • Case
    • Mounting Materials

### Diode Laser
For the purpose of our project, Diode Lasers (red) were the most power efficient and low cost. Additionally, red light has the biggest wavelength (compared with green and blue) thus, it is the least harmful for the human eye.

### Photovoltaic Panel
The photovoltaic panel is connected to the ADC (Analog to Digital Converter) of the Daughter Card in order to digitalize the signal.
Power Supply
The power supply was a major and very challenging problem we had to solve. Our first approach was to use conventional batteries. However, this approach did not provide a strictly constant DC voltage as long as the batteries were quickly discharged. So we had to make a regulator circuit to overcome this problem. Instead of this we used power banks, which were low cost, efficient, rechargeable and provided the desired DC constant power supply.
Mirror reflectors
Used to redirect the light beam to the desired destination.

### Switching Circuit

![Laser Amplifier Circuit](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/06-LaserAmplifierCircuit.png "Laser Amplifier Circuit")
Figure 6: Laser amplifier circuit.

This circuit was used for duty-cycle purposes, in the interest of minimizing the power consumption of the project. For the implementation of the switching circuit we used the following components:
    • 10 kΩ resistance
    • 2N3904 transistor
The switch circuit is represented in Fig. 6. In order to use the transistor as a switch circuit, we connected a 3,3 V output provided by the used target board via a resistance to the Base of the transistor, while connecting the output of the transistor (Collector) to the negative polarity of the diode laser. As a result, we manage a power consumption approximately to 100 mW for each laser circuit contained in a grid.

### Optical filter
In order to eliminate the ambient light’s intensity inside the slave’s case and only let the laser pass through, optical filters were used. An optical filter is a semi-transparent material that absorbs specific light wavelengths. The ones used in this project were filters that only allowed red pass through, which is the color of the used lasers. Such a filter could be purchased as a sheet of paper. However, for the purposes of this project and because of time shortage, several layers of red colored transparent tape were used in the place of the optical filter. Although, an optical filter cannot cut off the whole ambient light (ambient light also includes red wavelengths) and at the same time let the red laser pass through, it was a good enough solution, cutting off most of the other wavelengths.

### Control Unit & Radio
The slave node’s Control Unit & Radio is comprised of a Silicon Labs C8051F321 Daughter-Card, an 8-bit microcontroller mounted on a small development board with various IO ports (Buttons, IO pins, MicroUSB connector, etc), and a Silicon Labs CC2500 Radio Shield, which is an 8-bit microcontroller with radio modulation capabilities and I2C communication. It is responsible for collecting the needed measurements through its ADC (Analog to Digital Converter) component (see Middleware section), processing them and sending them to the master node and, also, switching its laser’s state for maintaining the system’s low power consumption characteristics.

### Case
Second handed Electrical Junction Boxes were used to accommodate all slave’s components. Two holes were drilled on its front face, one for holding the laser diode and one for letting the laser beam enter the box and hit the photovoltaic panel. The photovoltaic panel was placed as far from the hole as possible in order to eliminate the ambient light hitting it for reasons that will be discussed in the middleware section. For further elimination, the aforementioned optical filter was placed on top of that hole.

### Mounting Materials
Mounting hardware not only inside the slave’s case but also on the walls without establishing permanent installations (e.g. using holes and/or screws) that could damage the building, was a difficult, time and resource consuming task. Solutions such as 3D printed bases were tested in combination with materials such as Blu Tack and double sided tapes. Despite their light weight, stability problems surfaced. Therefore, the lasers' beams were diverted from the target. At last, the slaves cases were mounted on the walls using double sided tape and the reflection mirrors using only Blu Tack, which helped to easily redirect the laser beams.

### Slave Node Overview
Fig. 7 shows the wiring between the components we already discussed. Concerning the power supply of the circuit we used the daughter-card’s pins to provide power for both the circuit and the positive polarity of the laser diode. The daughter-card feeds the laser positive polarity and the switching circuit’s enable signal with VCC = 3.3V. Finally, the daughter-card’s power was provided by a 5V, 2600 mAh power bank which, in paper, could last for more than 18-hours of continuous use.
 
![Opened Slave Node](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/07-OpenedSlaveNode.png "Opened Slave Node")
Figure 7: Inside a slave’s casing.

### Master Node Overview
The base station/master node is comprised of a Silicon Labs C8051F320 DevKit, an 8-bit microcontroller mounted on a development board with various IO ports (Buttons, IO pins, DB9 connector, power supply connector, etc), a Silicon Labs CC2500 Radio Shield, an 8-bit microcontroller with radio modulation capabilities and I2C communication, a 5V power adapter and a Serial to USB adapter. The master node exchanges data with all slave nodes via the aforementioned radio shield. The collected data is then forwarded to the server via the DevKit’s serial port (utilizing its UART capabilities), which is then adapted to a USB port through the Serial to USB adapter. The latter is needed because, at the time of writing this report, the serial port is being deprecated and obsolete from every modern PC and also the Laptop/Raspberry Pi was used as a server.

## Middleware

### Connecting The Devices
The main objective was to collect the voltage values from each of the five slaves to the master node in order to forward them to the software to be processed for the detection. Achieving a reliable communication between one slave and the master was the first aim. Afterwards, the master node’s code was adjusted in order to control the rest.
The implementation began by writing code for the exchange of simple packets between the master and the slaves. RF-related registers were fine-tuned with the aid of the smartRF studio 7, which is an application that takes as input the desired design parameters and returns code that configures the nodes’ radios accordingly.
Every device can be used in one of the two available modes, receiver mode and transmitter mode, at a time. Checking for good communication between devices, one was set in transmitter mode and the other in receiver mode. As soon as they exchanged a single packet they would both switch modes and begin a new packet exchange. This process of exchanging packets would continue until a packet was lost. This method is called ping pong because a single packet is sent from one device to another and back again, like a ball of ping pong. 

### Handling the Data
After a reliable communication between devices has been established, the next step was to measure voltage values given from a power supply using the daughter-card’s analog-in pins and converting them into bytes, which will be later sent to the master. For this procedure an ADC (analog to digital converter) is required, an electronic component inside the slave’s microcontroller, which by comparing a port’s voltage to a set reference voltage, returns a measurement from 0 to 1023 of how close the port’s voltage is to the reference’s voltage. This method is called analog to digital conversion, because it accepts an analog, continuous signal and converts it into a discrete number referring to a specific moment of time. Following the addition of the appropriate functionality for the ADC part, the results of the converter were put in the tx Buffer (the buffer where the values that we want to be sent are into) in order to be transmitted to the master. After receiving the values from the slave, it was needed to view if the values from the power supply were sent correctly and in order to the master. Functionality extension had be made on the master, so that serial port communication capabilities could be used via the UART protocol. In order to view the received data from the PC, the Hercules application was used.

### Communication protocol
According to the initial plan, the master node was supposed to send a request to the first slave, waiting to receive a measurement, and then proceed to the next. After the implementation of this protocol, we came to the conclusion that this was not the ideal method as the whole procedure took around 2.5 seconds (500ms for each slave). This period was way too big for a smoke detection task, so a new, faster protocol had to be designed.
The final communication protocol expects the master to broadcast to all slaves an instruction to take a measurement. After the needed period of time for a measurement to be made (approximately 200ms), the master, starting from the first slave, sends a packet to request for that measurement and when received it continues to the next one, until it has collected all slaves’ measurements. That method was proved to be fast enough for our implementation as the elapsed time is being reduced to around 500ms.
Although, another issue occurred, as in unpredicted time, the communication ended suddenly, because a transmitted packet was lost. In this case, the procedure crashed as the master waited forever for a packet to be received. For that reason, a timeout routine was created which leads to a timeout after a short, pre defined period of time of 100ms, in order to avoid system crashes. When a timeout occurs, master continues its execution and requests the next node’s measurement. As a result, even in the case that a slave node is being lost from the network (e.g. battery fully discharged, change in environment leading to poor network connection, etc), the system’s execution will continue with no impact, except for always sending the last received measurement of the lost node.

### Measurements
To get a sense of the photovoltaic’s behaviour under the ambient light and the selected laser beam, measurements were made using one of the lab’s oscilloscope. Because most of the ambient light was generated from fluorescent lamps, which their operating principle is charging mercury to emit ultraviolet light to the tube’s gas, which it then emits visible light, a frequency of around 100 Hz was detected. Trying to remove that frequency, as well as, the ADC’s measurement noise, an average of 2048 measurements was computed, which was then sent to the master.
As soon as the first real world test took place at the desired area revealing the realistic measurements that had to be taken under consideration, a slight change in the measurement’s functionality had to be made in order to increase the sensitivity. The previous measurement method disadvantage was that it only measured ambient light’s intensity (which cannot be avoided) in combination with the laser’s intensity. However, the wanted measurement was (if possible) to measure only the laser’s intensity. So, to eliminate the ambient light’s intensity, it had to be measured and subtracted from the combination of ambient light and laser. The final measurement method works as follows: 
The laser is turned on, while the ADC takes 1024 measurements which are summed up to a variable, SUM, representing a measurement of the combination of the ambient light and the laser. Afterwards, the laser is turned off, while the ADC takes another 1024 measurements, which represent a measurement of the current ambient light. Those measurements are then subtracted from the SUM variable. Finally, the SUM is divided by 1024 to calculate the measurements’ arithmetic mean, which represent a close approximation of the laser’s intensity. To further eliminate ambient light’s intensity which caused the photovoltaic to operate on its nonlinear area, an optical filter, which only lets red (the laser’s color) to pass through, was used as discussed above.

### Master Node Algorithm Flow Chart
![Master Node Algorithm Flow Chart](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/08-MasterNodeAlgorithmFlowChart.png "Master Node Algorithm Flow Chart")
Figure 8

### Slave Node Algorithm Flow Chart
![Slave Node Algorithm Flow Chart](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/09-SlaveNodeAlgorithmFlowChart.png "Slave Node Algorithm Flow Chart")
Figure 9

## Software
The software team was responsible for fetching the data from the master node, keeping a log of them, and plotting them on MATLAB-like environment for the user to overwatch. In addition to that, accurate real-time detection of smoke is offered through simple digital signal processing, as well as the ability to project this information on a website, reachable from throughout the campus, alongside (un)pleasant sound effects as a notification/warning for the end user.

### Website & Data Handling
The website’s backend was created using the DJango framework, a high level web framework for rapid development, clean and pragmatic design. As it uses Python, it was the most suitable for the project’s needs because the smoke detection algorithm could be easily implemented inside the backend’s features. Last but not least, the data collection from the master node is done using the UART protocol through the PC’s serial port. 
On server launch, a new thread is created with the purpose of reading the serial port and storing its data into a FIFO buffer, from which the detection algorithm could make the needed calculations. Whenever a new calculation is ready, it is stored in a variable which can be sent to all the website’s connected clients. This thread also logs the received data, the timestamp of each measurement and the result of the detection. This can be quite useful since it allows the user to examine events in the past, even if the system is left unsupervised.
Another thread is also spawned in order to plot the received waveforms of each sensor on the screen.
The reason why separate threads are needed is because some of the aforementioned functionalities are blocking, meaning that the server would not continue its execution if single threaded.
The website’s frontend was created using plain HTML5 and CSS3 for the user interface as well as jQuery for dynamically updating the states of the predefined measured areas. In more detail, when the website is accessed, a periodic routine is triggered every 8 second to request from the server the current areas’ states. A json response is returned from the backend containing the needed data, which are stored in the previously mentioned variable. 
Whenever smoke is detected in an area, the corresponding area on the website turns from green to red and a sound effect is played, in order to catch the attention of the users.

### Observations On Data
Several factors were considered before deciding what is a good way to approach the detection. 
The ambient light introduced bias on the sensors. Normally this wouldn’t pose a problem, however considering the day/night cycle as well as the lights turning on/off, this was a problem. In addition to that, if the smoke settled in the room, the ambient light as well as the laser rays would be affected by a slow, time-dependant factor (e.g., day/night transition, smoke clearing out). These effects are different on each sensor considering their different location in the room and their electronic characteristics (e.g., panels are physically very different from each other).
The above observations are proof that the detection method needs to be able to adjust on its own, for each sensor, to a slow changing environment.
In contrast to the previous, instability on the part of the mirrors, due to vibrations etc., coupled with the thermal noise of the ADC, flickering can be noticed on the measurements. This small fluctuation needs to be filtered out and not be considered in the detection as it can potentially cause false positives.
Thus, it is now clear that the constant (or very slow), and fast components of the received waveforms must be discarded.

### DSP & Detection
Having said that, the digital signal processing part is now a lot more straightforward. By performing Discrete Time Fourier Transform (DTFT) on small time frames of the received waveforms, frequencies (such as the above) can be cropped in order to detect smoke from the rest of the signal. When there is no smoke, the intermediate frequencies (higher than 0 less than 1, see Fig. 11 left) offer small contribution to the overall signal. However, when smoke passes through the laser ray, a sudden drop at the sensor level occurs (also see in Fig. 11 right, how the intermediate frequencies get significant amplitude). A heuristic metric was developed by calculating the arithmetic mean of the absolute value of the intermediate frequencies. Through trial and error, a heuristic threshold value was identified and used as a means of checking whether there is smoke or not. Surprisingly, this method works quite good, all things considered.

![Real Time Monitoring Sensor Level](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/10-RealTimeMonitoringSensorLevel.png "Real Time Monitoring Sensor Level")
Figure 10: Real-time monitoring of Sensor Level.

![Smoke Detection Algorithm Figure](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/11-SmokeDetectionAlgorithmFigure.png "Smoke Detection Algorithm Figure")
Figure 11: The top figures are time windows without and with smoke. The bottom figures are their corresponding FFT waveforms. 

![Smoke Detection On Time And On Frequency](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/12-SmokeDetectionOnTimeAndOnFrequency.png "Smoke Detection On Time And On Frequency")
Figure 12: Waveform of sensor level (top), alongside the FFT based metric compared to the heuristic threshold (bottom). 

![Site UI No Smoke](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/13-SiteUINoSmoke.png "Site UI No Smoke")
Figure 13: Website view without smoke.

![Site UI With Smoke](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/14-SiteUIWithSmoke.png "Site UI With Smoke")
Figure 14: Website view with smoke.

# Future Work
---
## Hardware
Low cost laser modules come with low quality, poor focus performance and endurance. In addition, generic mirrors affects laser beams’ focus and intensity over multiple reflections. Overall, the received beam on the photovoltaic panel loses its property of being a “beam” as it is getting distorted after all the needed reflections and its initial low focus. Optimizing the laser’s wavelength by selecting the one that gets the greatest absorption from gases that need to be detected, even other than smoke or even invisible to the human eye such as CO or dangerous flammable gases. Furthermore, optimized optical filters, better mounting solutions and smaller casing could improve sensitivity and ease of installation. 

## Middleware
Power consumption can be reduced using the minimum required  power output of transmitter in order to meet the requirements of  sensitivity of receiver for a successful communication. Also, lower data rate could lead to power efficiency.

## Software
Due to time constraints, the detection had to be performed in a simple straightforward way. The result is that we came up with a heuristic metric and a heuristic threshold that may require fine-tuning overtime. In the future, neural networks could be facilitated in order to make the system smarter and enabling it to pinpoint the exact location of the smoke, by comparing values of multiple (with possibly intersecting rays) sensors. Another benefit of this approach would be that we can drop the heuristic metrics and thresholds, thus making the detector much more robust and independent (in terms of fine-tuning).

# Links
---
Youtube Demo Video: https://youtu.be/7rDN7ZuSLMw
GitHub Repository: https://github.com/TFotakis/TUC-Synthesis

# The Team
---
![The Team](https://raw.githubusercontent.com/TFotakis/TUC-Synthesis/master/Project/Documentation/15-TheTeam.png "The Team")