# Introduction To Robotics (2022-2023)
A repository to learn the basics of robotics for the course of Introduction To Robotics (3rd year, Faculty of Mathematics and Informatics, University of Bucharest) 
This includes: homework requirements, implementation details (code, image files, description).

## Homework 1
### Requirements
**Components**
> RBG  LED  (1  minimum),  potentiometers  (3  minimum),resistors and wires (per logic)

**Technical Task**
> Use a separat potentiometer in controlling each of the color of the RGB led (Red, Green and Blue). The control must be done with digital electronics(aka you must read the value of the potentiometer with Arduino, and write a mapped value to each of the pins connected to the led).

### Solution
* The circuit diagram:
<img src="./homework/homework_1/RGBLED_circuit.jpeg">

* The physical setup:
<img src="./homework/homework_1/RGBLED_setup.jpeg">

* The code can be found [here](homework/homework_1/RGBWithPotentiometer.ino)
* The video showcasing the functionality can be found [here](https://youtu.be/d9IHBvmgFHg)


## Homework 2
### Requirements
**Components**
> 5 LEDs (2 red, 2 green and 1 yellow), 1 buzzer, 1 button, resistors and wires (per logic)

**Technical Task**
> Build the traffic lights for a crosswalk. Use 2 LEDs to represent the traffic lights for people (red and green) and 3 LEDs to represent the traffic lights for cars (red, yellow and green). Below see the states it needs to go through.

| State number | Lights for cars | Lights for pedestrians | Sound | Duration |
| --- | --- | --- | --- | --- |
| `1` (default) | Green | Red | No sound | Indefinite, changed by pressing the button, then lasts **8** seconds |
| `2` (yellowForDrivers) | Yellow | Red | No sound | **3** seconds |
| `3` (freeForPedestrians) | Red | Green | Beep at constant interval | **8** seconds |
| `4` (pedestriansHurry) | Red | Green (blinking) | Beeping at faster interval | **4** seconds |

### Solution
* The circuit diagram:
<img src="./homework/homework_2/trafficLights_circuit.jpeg">

* The physical setup:
<img src="./homework/homework_2/trafficLights_setup.jpeg">

* The code can be found [here](homework/homework_2/trafficLights.ino)
* The video showcasing the functionality can be found [here](https://youtu.be/yoPrPJzQH1o)
