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

