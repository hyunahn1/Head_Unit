import RPi.GPIO as GPIO

class Motor():
	def __init__(self, motor_pin1=17, motor_pin2=27, pwm_pin=18):
		self.pwm = None
		self.motor_pin1 = motor_pin1
		self.motor_pin2 = motor_pin2
		self.pwm_pin = pwm_pin

	def init_motor(self):
		GPIO.setup(self.motor_pin1, GPIO.OUT)
		GPIO.setup(self.motor_pin2, GPIO.OUT)
		GPIO.setup(self.pwm_pin, GPIO.OUT)
		self.pwm = GPIO.PWM(self.pwm_pin, 1000)
		self.pwm.start(0)

	def set_speed(self, axis_value):
		speed = abs(axis_value) * 100
		if axis_value > 0.1:  #acceleration
			GPIO.output(self.motor_pin1, True)
			GPIO.output(self.motor_pin2, False) # sets for reverse
			self.pwm.ChangeDutyCycle(speed) # setting the speed/ acceleration

		elif axis_value < -0.1 : # reverse acceleration
			GPIO.output(self.motor_pin1, False)
			GPIO.output(self.motor_pin2, True)
			self.pwm.ChangeDutyCycle(speed)

		else :	# no acceleration -> stopping
			GPIO.output(self.motor_pin1, False)
			GPIO.output(self.motor_pin2, False)
			self.pwm.ChangeDutyCycle(0)

	def motor_cleanup(self):
		if self.pwm:
			self.pwm.stop()