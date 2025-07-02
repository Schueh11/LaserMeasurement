import serial
import time
from datetime import datetime
import os
import threading

# Initialize the serial connection
ser = serial.Serial(
    port='COM7',
    baudrate=9600,
    timeout=1
)

# Global variables
textoutput = 0
textinput = 0
running = True
measurements_per_revolution = 0
micrometers_between_measurements = 0
seconds_between_measurements = 0

# Function to check if a number is a divisor of 2048
def is_divisor_of_2048(number):
    return 2048 % number == 0

# Function to check if a number is a positive whole number
def is_positive_whole_number(number):
    return number > 0 and isinstance(number, int)

# Function to read serial data
def read_serial():
    global textoutput, textinput, running
    filename = datetime.now().strftime("%Y-%m-%d_%H-%M-%S") + '.txt'
    with open(filename, 'a') as file:  # Open file in append mode
        while running:
            if ser.in_waiting > 0:
                data = ser.readline().decode('utf-8').rstrip()
                if data == "End":
                    running = False
                    print("The measurement is complete, the programme is now ended.")
                    os._exit(0)  # Exit the program with exit code 0
                values = [x.strip() for x in data.split(',')]
                if textinput == 0:
                    print(f"Received data: {data}")
                elif textinput == 1:
                    if len(values) == 4:
                        measurepoint, angle, diameter, power = values
                        print(f"Measurepoint: {measurepoint} | Angle: {angle} | Diameter: {diameter} | Power: {power}")
                        file.write(f"{measurepoint},{angle},{diameter},{power}\n")  # Write data to file
                        file.flush()  # Ensure data is written to file
                    else:
                        print(f"Invalid data received: {data}")

# Function to write serial data
def write_serial():
    global textoutput, textinput, running
    while running:
        if textoutput == 0:
            prompt = "How many diameters should be measured? Enter a whole number: "
        elif textoutput == 1:
            prompt = "Enter 2 to stop the movement: "
        elif textoutput == 2:
            prompt = "Enter 3 to start the measurement: "
        else:
            prompt = "Enter 0 to stop the measurement: "
        user_input = input(prompt)
        ser.write(user_input.encode('utf-8'))
        print(f"Sent data: {user_input}")
        if textoutput == 0:
            try:
                user_input = int(user_input)
                if is_positive_whole_number(user_input):
                    textoutput = 1
                else:
                    print("Error: Please enter a positive whole number.")
            except ValueError:
                print("Error: Please enter a valid whole number.")
        elif user_input == "2":
            textoutput = 2
        elif user_input == "3":
            textoutput = 3
            textinput = 1
        elif user_input == "0":
            if textoutput == 3:
                running = False  # Stop the program
                print("The measurement is complete, the programme is now ended.")
                os._exit(0)  # Exit the program with exit code 0
            textoutput = 0

try:
    # Main program
    print("Laser beam measurement program starting. Enter desired parameters.")
    while True:
        try:
            measurements_per_revolution = int(input("How many measurements per revolution? Enter a whole number which is a divisor of 2048: "))
            if is_divisor_of_2048(measurements_per_revolution):
                print(f"Valid input: {measurements_per_revolution}")
                break
            else:
                print("Error: The number entered is not a divisor of 2048.")
        except ValueError:
            print("Error: Please enter a valid whole number.")

    while True:
        try:
            micrometers_between_measurements = int(input("How many micrometers between measurements? Enter a positive whole number: "))
            if is_positive_whole_number(micrometers_between_measurements):
                print(f"Valid input: {micrometers_between_measurements}")
                break
            else:
                print("Error: Please enter a positive whole number.")
        except ValueError:
            print("Error: Please enter a valid whole number.")

    while True:
        try:
            seconds_between_measurements = int(input("How many seconds between measurements? Enter a positive whole number: "))
            if is_positive_whole_number(seconds_between_measurements):
                print(f"Valid input: {seconds_between_measurements}")
                break
            else:
                print("Error: Please enter a positive whole number.")
        except ValueError:
            print("Error: Please enter a valid whole number.")

    # Start reading and writing in separate threads
    read_thread = threading.Thread(target=read_serial)
    write_thread = threading.Thread(target=write_serial)

    write_thread.start()
    read_thread.start()

    write_thread.join()
    read_thread.join()
except KeyboardInterrupt:
    print("Program terminated")
finally:
    ser.close()
