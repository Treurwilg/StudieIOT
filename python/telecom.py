#!/usr/bin/env python3
''' In dit programma wordt het Python-deel van de eindopdracht
    van de Udemy cursus RaspberryPi en Arduino uitgewerkt. Python op de RaspberryPi
    Het deel van Arduino is al klaar om commands te verwerken. 
    
    Arduino verzorgt startsituatie met deur gesloten, gele led aan,
    "Press button to call" op LCD, deur gesloten.
    Het deurprocess wordt op de arduino gestuurd afhankelijk van /open of /deny
    vanuit Python.
    Nadat de buzzer is overgegaan op de Arduino stuurt deze bericht "caller".
    Python hier wacht op bericht "caller" die het vertaalt naar "/open"? of
    "/deny"? op de Telegram-chat , nadat er een foto is gemaakt die ook naar
    de chat stuurt zodat de huismeester aan de hand van de foto kan kiezen.
    De van de huismeester wordt met respond:open of respond:deny beantwoord.
    Zover werkt dit programma nu. Nog toe te voegen is het nemen en verzenden naar
    de chat tegelijk met de vraag /open of /deny.
    Er worden nog testberichten naar de Shell gestuurd.
'''
import serial
import time
from telegram import Bot
from telegram.ext import Updater, CommandHandler
with open("/etc/environment", "r") as f:
    chat_id = f.read().rstrip()
chat_cmd_used = False


def start_handler(update, context):
    # print(update.effective_chat.id) # om voor volgende programma chat_id te krijgen
    context.bot.send_message(chat_id=update.effective_chat.id,
                             text="Hello from Python!")
    
def open_handler(update, context):
    global chat_cmd_used # om te voorkomen dat het command in de chat nogmaals gebruikt wordt.
    if ( chat_cmd_used == False):
        strToSend = "respond:open\n".encode('utf-8')
        ser.write(strToSend)
        context.bot.send_message(chat_id=update.effective_chat.id, text="Door opens")
        chat_cmd_used = True
    
    
def deny_handler(update, context):
    global chat_cmd_used # om te voorkomen dat het command in de chat nogmaals gebruikt wordt.
    if (chat_cmd_used == False):
        strToSend = "respond:deny\n".encode('utf-8')
        ser.write(strToSend)
        context.bot.send_message(chat_id=update.effective_chat.id, text="Keep closed")
        chat_cmd_used = True
    
with open("/home/jan/.local/share/.t_bot_token", "r") as f:
    telegram_token = f.read().rstrip()
bot = Bot(token=telegram_token)
updater = Updater(token=telegram_token) # to keep track of what happens in the chat
dispatcher= updater.dispatcher          # to send chat input to call back function
dispatcher.add_handler(CommandHandler('start', start_handler)) # is callback function
dispatcher.add_handler(CommandHandler('open', open_handler))
dispatcher.add_handler(CommandHandler('deny', deny_handler))
updater.start_polling() #in his own thread # we are being updated if something is sent
print("Telegram bot ready")

while True:
    try:
        ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1.0)
        print("Successfully connected to Serial.")
        break
    except serial.SerialException:
        print("Could not connect to Serial. Try again.")
        time.sleep(1)
time.sleep(3)
ser.reset_input_buffer()
print("Serial OK")

try:
    while True:
        while ser.in_waiting <= 0:
            time.sleep(0.01)
        alarm = ser.readline().decode('utf-8').rstrip()
        print(alarm)
        if (alarm == "caller"):
            print("Send message to chat")
            bot.send_message(chat_id=chat_id, text="/open or /deny")
            chat_cmd_used = False # na de nieuwe call kunnen de nieuwe commands gebruikt worden
            while ser.in_waiting <= 0:
                time.sleep(0.01)
            alarm = ser.readline().decode('utf-8').rstrip()
            bot.send_message(chat_id=chat_id, text="Door closed") # if toevoegen
            print(alarm)

except KeyboardInterrupt:
    print("Manager stopped telecom program.")
    bot.send_message(chat_id=chat_id, text="I stopped telecom service")
    ser.close()