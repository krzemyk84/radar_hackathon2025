#!/usr/bin/env python3

import time
import smbus
import pygame
import math

SLAVE_ADDR = 0x08 #Pi's I2C address
bus = smbus.SMBus(1)
KEY = 0x14

def decrypted_data(byte1,byte2):
	return (byte1^KEY)|((byte2^KEY)<<8)

def calculate_checksum(data):
	checksum = 0
	for byte in data:
		checksum ^= byte
		checksum_final = checksum % 256
	return checksum_final

def request_data_from_slave():
	try:
		data = bus.read_i2c_block_data(SLAVE_ADDR, 0, 5)
		
		print(data)
		
		encrypted_angle_low = data[0]
		encrypted_angle_high = data[1]
		angle = decrypted_data(encrypted_angle_low,encrypted_angle_high)
		
		encrypted_distance_low = data[2]
		encrypted_distance_high = data[3]
		distance = decrypted_data(encrypted_distance_low,encrypted_distance_high)
		decrypted_message = (angle,distance)
		check_data=(data[0],data[1],data[2],data[3])
		received_checksum = data[4]
		calculated_checksum = calculate_checksum(check_data)
		
		print (angle,distance)
		if calculated_checksum == received_checksum: 
			print(f"Received data:{decrypted_message}")
			return decrypted_message
		else:
			print("Error:Checksum error, data might be altered")
		
	except Exception as e:
		print(f"Error reading from I2C{e}")
		
def main():
	while True:
		#GUI 

		# Initialize Pygame
		pygame.init()

		# Define the serial port and baud rate (Ensure it matches the Arduino setting)
		#ser = serial.Serial('COM5', 9600, timeout=1)  # Replace 'COM3' with your port (Linux/macOS: '/dev/ttyUSB0')
		time.sleep(2)  # Allow time for the connection to establish

		# Window size
		WIDTH, HEIGHT = 800, 480
		screen = pygame.display.set_mode((WIDTH, HEIGHT))
		pygame.display.set_caption("120° Radar Scan")

		# Radar propertieswith 
		CENTER = (400,480)  # Radar center
		RADIUS = 480  # Radar radius
		SWEEP_SPEED = 7.5 # Speed of scanning (degrees per frame)
		#OBJECT_COUNT = 10  # Number of detected objects

		# Colors
		BLACK = (0, 0, 0)
		GREEN = (0, 255, 0)
		DARK_GREEN = (0, 100, 0)
		RED = (255, 0, 0)

		objects = []

		def add_object(angle,distance):
			try:
				"""Convert (angle, distance) to (x, y) and store with timestamp"""
				#x = CENTER[0] + distance * math.cos(math.radians(angle + 90))
				x = CENTER[0] + distance/200*450* math.cos(math.radians(angle))
				#y = CENTER[1] - distance/250*450 * math.sin(math.radians(angle + 90))
				y = CENTER [1] - distance/200*450 * math.sin(math.radians(angle))
				objects.append({"x": x, "y": y, "angle": angle, "distance": distance, "time": time.time(), "alpha": 255})
			except:
				print("No value input")

		# Simulate new object detection every 3 seconds
		last_object_time = time.time()
		# Convert (angle, distance) to (x, y)
		object_positions = []
#		for angle, distance in objects:
#			x = CENTER[0] + distance * math.cos(math.radians(angle))
#			y = CENTER[1] - distance * math.sin(math.radians(angle))
#			object_positions.append((x, y, angle, distance))

		def get_endpoint(angle, length):
			"""Returns the (x, y) endpoint for a given angle and length."""
			x = CENTER[0] + length * math.cos(math.radians(angle + 90))
			y = CENTER[1] - length * math.sin(math.radians(angle + 90))
			return int(x), int(y)

		# Main loop
		running = True
		angle = -60  # Start angle
		direction = 1 
		previous_x = 0
		previous_y = 0
		arc_centre_x, arc_centre_y = 400,480
		base_radius = 480
		arc_start_angle = 29
		arc_end_angle = 151
		
		start_rad = math.radians(arc_start_angle)
		end_rad = math.radians(arc_end_angle)
		
		while running:
			screen.fill(BLACK)
			# Draw radar semi-circle
			for i in range (5):
				arc_radius = base_radius - i * 90
				
				pygame.draw.arc(screen,DARK_GREEN,(arc_centre_x - arc_radius, arc_centre_y - arc_radius, 2*arc_radius, 2*arc_radius),start_rad,end_rad,2)
			pygame.draw.line(screen, DARK_GREEN, CENTER, get_endpoint(62, RADIUS), 2)  # -60° line
			pygame.draw.line(screen, DARK_GREEN, CENTER, get_endpoint(-62, RADIUS), 2)
		# Add new object every 0.2 seconds (for testing)
			if time.time() - last_object_time > 0.1:
				new_angle = -60 + (angle + 60) % 120  # Random in scanning range
				data = request_data_from_slave()
				try:
					Angle = data[0]
					Distance = data[1]
					add_object(Angle, Distance)
					last_object_time = time.time()
				except:
					print("No Value")

			# Draw detected objects with fading effect
			for obj in objects[:]:  # Iterate over a copy to modify list
				elapsed = time.time() - obj["time"]
				#print(obj)
				if elapsed > 0.2:  # Start fading after 0.1 second
					obj["alpha"] -= 30  # Reduce transparency

				if obj["alpha"] <= 0:  # Remove when fully faded
					objects.remove(obj)
					continue

				# Create a surface with transparency
				obj_surface = pygame.Surface((10, 10), pygame.SRCALPHA)
				obj_surface.fill((255, 0, 0, obj["alpha"]))  # Red with alpha transparency
				screen.blit(obj_surface, (obj["x"] - 5, obj["y"] - 5))  # Center object

				# Display angle and distance
				pygame.draw.rect(screen,BLACK,(550,400,250,80))
				font = pygame.font.SysFont("Arial",20)
				text = f"{int(obj['angle'])}° | {int(obj['distance'])} cm"
				text_surface = font.render(text, True, (0,255,0))
				screen.blit(text_surface, text_surface.get_rect(center=(700,460)))

			# Calculate scanning line position
				end_x = CENTER[0] + RADIUS * math.cos(math.radians(obj['angle']))
				end_y = CENTER[1] - RADIUS * math.sin(math.radians(obj['angle']))
				if previous_x is not None and previous_y is not None:
					pygame.draw.line(screen, BLACK, CENTER, (previous_x, previous_y),2)

			# Draw scanning line
				pygame.draw.line(screen, GREEN, CENTER, (end_x, end_y),2)
				previous_x = end_x
				previous_y = end_y
			# Update angle for sweeping effect
		#    angle += SWEEP_SPEED * direction
		#    if angle >= 60 or angle <= -60:
		#        direction *= -1  # Reverse direction
			angle += SWEEP_SPEED 
			if angle >= 60:
				angle = -60  # Reverse direction


			# Handle events
#			for event in pygame.event.get():
#				if event.type == pygame.QUIT:
#					running = False
			pygame.display.flip()
			pygame.time.delay(50)  # Control frame rate


		#input from Sensor



		time.sleep(1)

if __name__=="__main__":
	main()
	


