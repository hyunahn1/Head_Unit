import RPi.GPIO as GPIO

class Steering:
	def __init__(self, pin=22):
		self.pwm = None
		self.pin = pin
	
	def init_steering(self):
		GPIO.setup(self.pin, GPIO.OUT)
		self.pwm = GPIO.PWM(self.pin, 50)
		self.pwm.start(0)

	def set_angle(self, axis_value):
		if abs(axis_value) < 0.1:
			axis_value = 0
		angle = axis_value * 90
		duty = 7.5 + (angle / 90) * 5
		self.pwm.ChangeDutyCycle(duty)

	def steering_cleanup(self):
		if self.pwm:
			self.pwm.stop()




# duty = 7,5 + (angle / 90) *5
		# middle + (angle / max angle) * half_range
		# 7.5 = straight
		# angle/ 90 normalize to -1 1
		# * 5 -> duty-cycle range = 2.5 - 12.5 = 10 -> range from 1 - -1 so 10/2