import RPi.GPIO as GPIO
import motor_control

motor_control.init_motor()

motor_control.set_speed(0.5)
time.sleep(1)
motor_control.set_speed(0)

motor_control.cleanup_motor()
GPIO.cleanup()