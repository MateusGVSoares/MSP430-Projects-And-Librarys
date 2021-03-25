# 																					MSP430 Real Time Clock									

​	Project of a <u>Real Time Clock</u> for the **MSP430 Micro Controller Family**, developed using **IAR IDE**,in the **Third Year of CEFET-MG Electronics Course**

------

### 																		

- ### Developers :

  - Mateus Gonçalves Soares

  - Gustavo Gonçalves Pereira Alcantra

  - Ana Clara Cammini

    


- ### Description :

  - The project is a **real time working clock** with 4 operation modes:

    - **Clock / Calendar** : <u>Current day and time are displayed in the LCM.</u>

    - **Configuration** : <u>Enables the user to make changes in the Clock / Calendar mode variables, such as day,hour and others.</u>

    - **Timer** : <u>Displays an initial valor, and when it`s counting is started, decreases the valor till it reaches 0, and then shows the initial valor again.</u> 

    - **Chronometer** : <u>Displays an initial valor of 00:00:00 and when it's triggered, increases the valor till it reaches the limit of 23:59:59, reseting the valor after that occurs.</u>
    
  - The clock is generated using the peripherals of **MSP430**.

  - **Peripherals** :

    - *Auxiliary Timer Block* : 
  - Uses an external clock to generate a clock wave with specific configurable proprieties.
      
    - *Timer_0 Block* :
      - 	Receives the signal from Auxiliary Timer Block to make an adjustable counting for generate a clock wave. 
    - *Capture / Compare Block (Part of Timer_0 Block)* :
      - Uses the signal of Auxiliary Timer Block and interruptions to generate a clock wave with 1 Hertz frequency.
    
  - A four by four membrane keyboard is used for the keyboard block.

  - **Interruptions are only used for generating clock**.

  - The same I/O port is used for the LCM and Keyboard.

  - The clock input is P2.6 (XIN).

    

- ### Specifications : 

  - **Micro Controller** : MSP430F1121

  - **Language** : C

  - **IDE** : IAR EW for MSP  (<u>7.20</u>)

  - **Simulation Software** : Proteus 8 Demonstration <u>( 8.11 )</u> 

    

- ### Diagram :

![](C:\Users\mateu\Desktop\Códigos JK - GIT - Porfólio\Projects\Trabalho Final - Ana C - Gustavo G - Mateus G\Diagram for GITHUB.png)

