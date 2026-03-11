import RPi.GPIO as GPIO
import steering_control

GPIO.setmode(GPIO.BCM)
steering_control.init_steering()

# Teste Lenkung
steering_control.set_angle(-1.0)  # Ganz links
time.sleep(1)
steering_control.set_angle(0)     # Mitte
time.sleep(1)
steering_control.set_angle(1.0)   # Ganz rechts

steering_control.cleanup_steering()
GPIO.cleanup()