# Introduction to Robotics (2023 - 2024)
Repository created for uploading (throughout the third year of faculty) the assignments from the 'Introduction to Robotics' course and laboratory, both held at the Faculty of Mathematics and Computer Science, part of University of Bucharest. Each homework will include requirements, implementation details, code and image files.

# Homework 1

### Description of task: 
  Use a separate potentiometer for controlling each color of the RGB LED: Red, Green, and Blue. This control must leverage digital electronics. Specifically, you need to read the potentiometer’s value with Arduino and then write a mapped value to the LED pins.

### Components used:
 - RGB LED x1
 - Potentiometers x3
 - Wires 
 - Breadboard x1
 - Resistance of 330 ohms x3
 - Arduino UNO x1


### Poza setup:

![rgb poza](https://github.com/MihaiC0stin/IntroductionToRobotics/assets/103452863/829ecc3a-8802-4d26-aa15-645ba80f48e1)



### Link video: https://youtu.be/dG-uEs4pkgs

# Homework 2

### Description of task: 
  Design a control system that simulates a 3-floor elevator using the Arduino platform. Here are the specific requirements:
  
    • LED Indicators: Each of the 3 LEDs should represent one of the 3 floors. The LED corresponding to the current floor should light up. Additionally, another LED should represent the elevator’s operational state. It should blink when the elevator is moving and remain static when stationary.

    • Buttons: Implement 3 buttons that represent the call buttons from the 3 floors. When pressed, the elevator should simulate movement towards the floor after a short interval (2-3 seconds).

    • Buzzer (optional for Computer Science, mandatory for CTI): The buzzer should sound briefly during the following scenarios:
                                                                                                                                  – Elevator arriving at the desired floor (something resembling a ”cling”).
                                                                                                                                  – Elevator doors closing and movement (pro tip: split them into 2 different sounds)
    
    • State Change & Timers: If the elevator is already at the desired floor, pressing the button for that floor should have no effect. Otherwise, after a button press, the elevator should ”wait for the doors to close” and then ”move” to the corresponding floor. If the elevator is in movement, it should either do nothing or it should stack its decision (get to the first programmed floor, open the doors, wait, close them and then go to the next desired floor).

    • Debounce: Remember to implement debounce for the buttons to avoid unintentional repeated button presses.

### Components used:
 - Yellow LED x3
 - Green LED x1
 - Buttons x3
 - Buzzer x1
 - Wires 
 - Breadboard x1
 - Resistance of 330 ohms x4
 - Arduino UNO x1


### Poza setup:

![Elevator Wannabe Setup](https://github.com/MihaiC0stin/IntroductionToRobotics/assets/103452863/272912db-f157-4cb4-931d-e9f2875ab434)




### Link video: https://youtube.com/shorts/I4C5-d9gzW0?feature=share

### What do I think it was the hardest task for this homework ?
  I think that this homework required much more time than the previous one, and probably the hardest part and the one that took me most of the time was to understand how to use millis so I can get some good "delays". My biggest mistake and problem that required most of the time was that I put everything in a for loop, so my code even with the millis "delays" was chancing the current floors too fast, or better to say, it stayed just 1 cycle between the floors so it did that action too fast for the change to be seen if I wanted to go more than 1 floor at a time. 

# Homework 3

### Description of task: 
  l use the joystick to control the position of the segment and ”draw” on the display. The movement between segments should be natural, meaning they should jump from the current position only to neighbors, but without passing through ”walls”.  The initial position should be on the DP. The current position always blinks (irrespective of the fact that the segment is on or off). Use the joystick to move from one position to neighbors (see table for corresponding movement). Short pressing the button toggles the segment state from ON to OFF or from OFF to ON. Long pressing the button resets the entire display by turning all the segments OFF and moving the current position to the decimal point. Interrupts are required.


### Components used:
 - Joystick x1
 - 7 segments display x1
 - Wires 
 - Breadboard x1
 - Resistance of 330 ohms x8
 - Arduino UNO x1


### Poza setup:
![7 segment display drawing setup](https://github.com/MihaiC0stin/IntroductionToRobotics/assets/103452863/625e9912-6f1b-4320-8fbc-5df4ab171c0d)





### Link video: https://youtu.be/cXhICiGPM08

# Homework 4

### Description of task: 
  Using the 4 digit 7 segment display and 3 buttons, implement a stopwatch timer that counts in 10ths of a second and has a save lap functionality.
  Display shows ”000.0”. When pressing the Start button, the timer hould start. During timer counter, each time you press the lap button, you should save that timer’s value in memory (not persistent, it is OK to be deleted upon reset), up to 4 laps (or more if you want), pressing the 5th time should override the 1st saved one. If you press the reset button while timer works, nothing happens. If you press the pause button, the timer stops.
In Pause Mode, the lap flag button doesn’t work anymore. Pressing the reset button resets you to 000.0. After reset, you can now press the flag buttons to cycle through the lap times. Each time you press the flag button, it takes you to the next saved lap. Pressing it continuously should cycle you through it continuously. Pressing the reset button while in this state resets all your flags and takes the timer back to ”000.0”.



### Components used:
 - Buttons x3
 - 4 7 segments display x1
 - Wires 
 - Breadboard x1
 - Resistance of 330 ohms x8
 - Arduino UNO x1


### Poza setup:
<img src = "https://github.com/MihaiC0stin/IntroductionToRobotics/assets/103452863/7a72b909-f0f5-4e29-b87e-a0168efced65" width=50% height=50%>




### Link video: https://youtu.be/EdleCymhbNs?si=ZAodPYVREG4-bMso


# Homework 5

### Description of task: 
Develop a ”Smart Environment Monitor and Logger” using Arduino.  This system will utilize various sensors to gather environmental data, log this data intoEEPROM, and provide both visual feedback via an RGB LED and user interaction through a Serial Menu. 

Menu Structure1:
  1.Sensor Settings // Go to submenu :
  
          1.1 Sensors Sampling Interval.  Here you should be prompted fora value between 1 and 10 seconds.  Use this value as a sampling ratefor the sensors.  You can read a separate value for each or have thesame for both.
          1.2 Ultrasonic Alert Threshold.  Here you should be promptedfor a threshold value for the ultrasonic sensor.  You can decide if thatis the min or max value (you can signal that something is too close).When sensor value                    exceeds the threshold value,  an alert should begiven.  This can be in the form of a message.  If the LED is set toAutomatic Mode (see section 4.2), it should also turn red if any of the sensors are outside the value.
          1.3 LDR Alert Threshold.  Here you should be prompted for athreshold  value  for  the  LDR  sensor.   You  can  decide  if  that  is  the min or max value (for example, it could signal that night is coming).When sensor value               exceeds the threshold value,  an alert should begiven.  This can be in the form of a message.  If the LED is set to Automatic Mode (see section 4.2), it should also turn red if any ofthe sensors are outside the value.
          1.4 Back// Return to main menu
          
  2. Reset Logger Data //  Should  print  a  message, to confirm to delete all  data.  Something like ”are you sure?”,  followed by the submenu with YES or NO. You can reset both sensor data at the same time, or you can do it individually:

          2.1 Yes.
          2.2 No.
     
  3. System Status// Check current status and health:
     
          3.1 Current Sensor Readings.  Continuously print sensor readingsat the set sampling rate, from all sensors.  Make sure you have a wayto exit this (such as pressing a specific key) and inform the user ofthis method through a                 message.
          3.2 Current Sensor Settings.  Displays  the  sampling  rate  andthreshold value for all sensors.
          3.3 Display Logged Data.  Displays last 10 sensor readings for allsensors.  (or be creative and do it another way).
          3.4 Back//  Return to Main menu.
     
  4. RGB LED Control// Go to submenu:
     
          4.1 Manual Color Control.  Set the RGB colors manually.  You decide how to input them, either by making an option for each channel, or by putting a string etc.  If you expect a specific format, makesure to inform the user.
          4.2 LED: Toggle Automatic ON/OFF.  If  automatic  mode  is ON, then the led color should be GREEN when all sensors value donot exceed threshold values (aka no alert) and RED when there is analert (aka ANY sensor value exceeds the threshold). When automatic mode is OFF, then the LED should use the last saved RGB values.
          4.3 Back// Return to main menu



### Components used:
 - Ultrasonic sensor x1
 - LDR sensor x1
 - RGB LED x1
 - Wires 
 - Breadboard x1
 - Resistance of 330 ohms x3
 - Resistance of 220 ohms x1
 - Resistance of 10k ohms x1
 - Arduino UNO x1


### Poza setup:
<img src = "https://github.com/MihaiC0stin/IntroductionToRobotics/assets/103452863/9526e990-b2aa-4e0d-9270-546059ce4523" width=50% height=50%>




### Link video: https://www.youtube.com/watch?v=8CXzV7KREtM




