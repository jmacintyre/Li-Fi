#!/usr/bin/python
import serial
import syslog
import time
import binascii

#The following line is for serial over GPIO
port1 = '/dev/ttyACM1'
port2 = '/dev/ttyACM0'

f = open('project_webpage/data.html','w')

ard1 = serial.Serial(port1,9600,timeout=5)
ard2 = serial.Serial(port2,9600,timeout=5)
time.sleep(2) # wait for Arduino

i = 0
send_msg = '';

while (send_msg != 'quit'):
    # Serial write section

    ID = raw_input("Select ID (1-4): ")
    send_msg = raw_input("String to be sent: ")
    send_msg = bin(int(binascii.hexlify(send_msg), 16))
    send_msg = str(send_msg[2:])

    if(ID == "1"):
        send_msg = '101' + send_msg + '0'
    elif(ID == "2"):
        send_msg = '110' + send_msg + '0'
    elif(ID == "3"):
        send_msg = '111' + send_msg + '0'
    else:
        ID = "4"
        send_msg = '100' + send_msg + '0'


    ard1.flush()
    ard2.flush()
    print ("Python value sent: ")
    print (send_msg[:-1])
    ard1.write(send_msg)
    time.sleep(2) # I shortened this to match the new value in your Arduino code

    # Serial read section
    msg = str(ard2.read(ard2.inWaiting())) # read all characters in buffer
    time.sleep(0.1)
    print ("Message from arduino: ")
    msg = str(msg[3:])
    msg = '0b' + str(msg)
    print(msg)
    n = int((msg), 2)
    rec_msg = binascii.unhexlify('%x' % n)
    print(rec_msg)
    f.write(rec_msg + ' --- Recieved by Node #' + ID +'<br>') # python will convert \n to os.linesep
     # you can omit in most cases as the destructor will call it

f.close()
exit()
