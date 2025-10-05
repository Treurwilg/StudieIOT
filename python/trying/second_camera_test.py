#!/usr/bin/env python3

from picamera2 import Picamera2
import time
import os

camera = Picamera2()
camera.resolution = (640, 480)
camera.rotation = 180

image_folder_name = "/home/jan/camera"
if not os.path.exists(image_folder_name):
    os.mkdir(image_folder_name)
time.sleep(2)

print("Take photo")
camera.start()
image_file_name = image_folder_name + "/new_image2.jpg"
camera.capture_file(image_file_name)



        
    
    
    
                                                                                                                                  