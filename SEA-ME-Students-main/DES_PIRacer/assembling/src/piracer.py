import RPi.GPIO as GPIO
from time import sleep
import pygame
import motor_control
import servo_control

pygame.init()

joystick = [pygame.joystick.Joystick(i)
			for i in range(pygame.joystick.get_count())]
j = joystick[0]
j.init()

GPIO.setmode(GPIO.BCM)

motor = motor_control.Motor()
steering = servo_control.Steering()
motor.init_motor()
steering.init_steering()

try:
	while True:
		pygame.event.pump()

		motor_axis = j.get_axis(1)  # joystick -> upwards can be negative -> use -j.get_axis(1)
		motor.set_speed(motor_axis)

		steering_axis = j.get_axis(0)
		steering.set_angle(steering_axis)

		sleep(0.02)

finally:
	motor.motor_cleanup()
	steering.steering_cleanup()
	GPIO.cleanup()
	pygame.quit()