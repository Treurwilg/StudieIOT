#!/usr/bin/env python3

with open("/etc/environment", "r") as f:
    id_string = f.read().rstrip()
    loc = id_string.index("=")
    chat_id = id_string.replace("CHAT_ID1=", "")
    print(loc)
    print(id_string)
    print (chat_id)