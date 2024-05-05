#Bibliothek zum ansteuern der GPIO Pins
from machine import Pin, PWM
#Bibliothek welche die Funktion enthält den Ablauf zu Pausieren
import time 
#der Servomotor ist am GPIO 4 angeschlossen
#die Arbeitsfrequenz ist 50Hz
servo = PWM(Pin(4), freq=50, duty=77)
#Variable für den Wert einer Pause zwischen den
#Schritten des Servomotors
PAUSE = 0.01
#Endlosschleife
while(True):
  #Schleife von 0 bis 100
  for duty in range(0,100):
    servo.duty(duty)
    time.sleep(PAUSE)
  #Schleife von 100 bis 0
  for duty in range(100,0, -1):
    servo.duty(duty)
    time.sleep(PAUSE)