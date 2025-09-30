# Smart Parking Reminder System

## Project Overview

The Smart Parking Reminder System is designed to help users easily locate their parked vehicles in large shopping complexes. The system leverages a combination of hardware and software components to automate parking spot detection, vehicle identification, and user notification.

## Features
- Detects parked vehicles using ultrasonic sensors
- Uses an ESP32-CAM mounted on a servo motor to capture images of parked vehicles
- Extracts license plate numbers from images using OCR (Optical Character Recognition)
- Processes OCR on a web server for accurate license plate recognition
- Sends parking location notifications to users via WhatsApp API

## Hardware Components
- **Ultrasonic Sensors:** Detect the presence of vehicles in parking spots
- **ESP32-CAM:** Captures images of vehicles when a spot is occupied
- **Servo Motor:** Rotates the ESP32-CAM to face the occupied spot
- **Arduino:** Controls the sensors and servo motor

## Software Components
- **Arduino Code (`arduino.ino`):** Controls the ultrasonic sensors and servo motor
- **ESP32-CAM Code:** Captures images and communicates with the web server
- **Web Server (`server.py`):** Performs OCR on received images and extracts license plate numbers
- **WhatsApp API Integration:** Sends notifications to users with the location of their parked vehicle

## Workflow
1. A vehicle enters a parking spot and is detected by the ultrasonic sensor.
2. The Arduino triggers the servo motor to rotate the ESP32-CAM towards the occupied spot.
3. The ESP32-CAM captures an image of the vehicle and sends it to the web server.
4. The web server uses an OCR library to extract the license plate number from the image.
5. The system sends a WhatsApp message to the user with the parking location and vehicle details.

## Files in This Project
- `arduino.ino`: Arduino code for sensor and servo control
- `server.py`: Python web server for OCR and notification
- (Add your ESP32-CAM code file here, e.g., `esp32cam.ino`)

## Getting Started
1. Upload the Arduino code to your Arduino board.
2. Upload the ESP32-CAM code to your ESP32-CAM module.
3. Set up and run the Python web server (`server.py`).
4. Configure WhatsApp API credentials in the server code.
5. Power up the hardware and test the system in a parking environment.

## Requirements
- Arduino IDE or PlatformIO
- Python 3.x (for the web server)
- OCR library (e.g., Tesseract)
- WhatsApp API access (e.g., Twilio)

## License
This project is for educational purposes.
