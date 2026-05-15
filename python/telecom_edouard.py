#!/usr/bin/env python3
'''
    Dit is de vooral de versie die Edouard Renard van Udemy geeft als resultaat
    van de eindopdracht. Hij verschilt vooral van die van mij doordat Edouard alle
    logica op de Pi houdt en ik de logica van het "open en sluit deur"-proces op
    de Arduino gerealiseerd had. Hetgeen tot complicaties en meer werk leidde.

    In dit programma wordt het Python-deel van de eindopdracht
    van de Udemy cursus RaspberryPi en Arduino uitgewerkt. Python op de RaspberryPi
    Het deel van Arduino is klaar om commands te verwerken. 
    
    Nadat de buzzer is overgegaan op de Arduino stuurt deze bericht "button_pressed".
    Python wacht op bericht button_pressed" die het vertaalt naar "/open"? of
    "/deny"? op de Telegram-chat , nadat er een foto is gemaakt die ook naar
    de chat stuurt zodat de huismeester aan de hand van de foto kan kiezen.
    De respons van de huismeester wordt met door_open of door_deny naar de Arduino
    doorgestuurd.
'''
import serial
import time
from picamera2 import Picamera2
import os
from telegram import Bot
from telegram.ext import Updater, CommandHandler

last_time_button_pressed = time.time()
button_pressed_delay = 5.0

open_door_request = False
handling_door = False

# Init Serial
print("Trying to connect to serial")
while True:
    try:
        ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1.0)
        print("Successfully connected to Serial.")
        break
    except serial.SerialException:
        print("Could not connect to Serial. Retrying in 1 second.")
        time.sleep(1)
        
def send_to_arduino(text):
    str_to_send = text.rstrip() + "\n"
    ser.write(str_to_send.encode('utf-8'))

# Telegram callbacks
def start_handler(update, context):
    str_to_send = "Welcome " + update.effective_user.first_name + "!"
    context.bot.send_message(chat_id=update.effective_chat.id,
                             text=str_to_send)

def open_door_handler(update, context):
    global open_door_request
    global handling_door 
    if open_door_request and not handling_door:
        print("Opening the door")
        handling_door = True
        send_to_arduino("open_door")
        send_to_arduino("print_text:Door is opened.")
        send_to_arduino("play_buzzer:300,500")
        send_to_arduino("set_led:GREEN")
        bot.send_message(chat_id=update.effective_chat.id, text="Opening the door...")
        time.sleep(10)
        print("Closing the door.")
        send_to_arduino("close_door")
        send_to_arduino("print_text:Push button to call.")
        send_to_arduino("play_buzzer:200,500")
        send_to_arduino("set_led:YELLOW")
        open_door_request = False
        handling_door = False
    
def deny_access_handler(update, context):
    global open_door_request
    global handling_door
    if  open_door_request and not handling_door:
        print("Denying access")
        handling_door = True
        send_to_arduino("print_text:Access denied")
        send_to_arduino("play_buzzer:200,1000")
        send_to_arduino("set_led:RED")
        bot.send_message(chat_id=update.effective_chat.id, text="Denying access.")
        time.sleep(5)
        send_to_arduino("print_text:Push button to call")
        send_to_arduino("set_led:YELLOW")
        open_door_request = False
        handling_door = False
        context.bot.send_message(chat_id=update.effective_chat.id, text="Keep closed")
    
# Init Camera
print("Init Camera")
camera = Picamera2() # anders dan in de cursus
camera.resolution = (640, 480)
camera.rotation = 180
image_folder_name = "/home/jan/camera"
if not os.path.exists(image_folder_name):
    os.mkdir(image_folder_name)
image_file_name = image_folder_name + "/door_camera.jpg" # vorige photo is overschreven
camera.start()
print("Camera OK")

# Init Telegram Bot"
print("Init Telegram bot")
with open("/etc/environment", "r") as f:
    chat_id = f.read().rstrip()
with open("/home/jan/.local/share/.t_bot_token", "r") as f:
    telegram_token = f.read().rstrip()
bot = Bot(token=telegram_token)
updater = Updater(token=telegram_token) # to keep track of what happens in the chat
dispatcher= updater.dispatcher
dispatcher.add_handler(CommandHandler('start', start_handler))
dispatcher.add_handler(CommandHandler('open', open_door_handler, run_async=True))
dispatcher.add_handler(CommandHandler('deny', deny_access_handler, run_async=True))

# Wait after setup
print("Waiting for 3 seconds")
time.sleep(3) # ook nodig voor de camera
ser.reset_input_buffer()
updater.start_polling()
send_to_arduino("print_text:Push button to call.")
send_to_arduino("set_led:YELLOW")
print("Serial OK. Main loop is starting.")

# Run main loop
try:
    while True:
        time.sleep(0.01)
        if ser.in_waiting > 0:
            msg = ser.readline().decode('utf-8').rstrip()
            if msg == "button_pressed":
                time_now = time.time()
                if (time_now - last_time_button_pressed >= button_pressed_delay) and \
                    (not open_door_request) and (not handling_door):
                    last_time_button_pressed = time_now
                    print("Open door request. Taking a photo and sending it to chat.")
                    open_door_request = True
                    send_to_arduino("print_text:Please wait for a few seconds.")
                    camera.capture_file(image_file_name)
                    bot.send_message(chat_id=chat_id, text="Someone is ringing the bell!")
                    with open(image_file_name, 'rb') as photo:
                        bot.send_photo(chat_id=chat_id, photo=photo)
                    bot.send_message(chat_id=chat_id, text="Send /open or /deny")
            else:
                ser.reset_input_buffer()
            
except KeyboardInterrupt:
    print("---") # just nice for the developer
    print("Manager stopped telecom program.")
    ser.close()
    print("Stopping Telegram updater, wait a few seconds...")
    updater.stop()
    bot.send_message(chat_id=chat_id, text="I stopped telecom service")





    




 

    