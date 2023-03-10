#This program acts as the main brain for the robot. It listens to commands from the gampad controller, and relays them in the correct format to the motor 
#controller module. It also keeps in constant communication with the ultrasonic sensors to make sure no object comes to close to the robot.
#
#Two threads are opened in order to simultaniously communicate with every module without sacrificing any speed. The two threads communicate via a queue.

from inputs import get_gamepad
from inputs import get_key
import serial
import time
import queue
from threading import Thread

mtr = serial.Serial(port='/dev/ttyUSB0', baudrate=115200, timeout=0.1)
uss = serial.Serial(port='/dev/ttyUSB1', baudrate=115200, timeout=0.1)
time.sleep(5) #sleeps so thaat the serial ports have time to perform proper handshaking

#this thread receives data from the gamepad and formats it before placing it into a queue for the second thread to use
def gamepad_receiver():
    deadband = 3
    middle = 128
    prev_time = time.time_ns()

    #the controller sends a range of 0-255 for the stick position
    #this function converts that range to a percentage that the motor module can use
    def calc_percent(state):
        if state < (middle - deadband):
            percent = (middle - state) / middle
        elif state > (middle + deadband):
            percent = (state - middle) / middle
        return int(percent * 100)

    #main loop in this thread
    #reads, formats, and adds data from the gamepad to a queue that the other thread will access
    while 1:
        events = get_gamepad()
        for event in events:
            if (time.time_ns() - prev_time >= 100000000) or ((event.state < middle + deadband) and (event.state > middle - deadband)):
                if event.code == "ABS_RZ":
                    if event.state < (128 - deadband):
                        #forward
                        q.put('w%03d\n' %  calc_percent(event.state))
                    elif event.state > (128 + deadband):
                        #reverse
                        q.put('s%03d\n' %  calc_percent(event.state))
                    else:
                        q.put('F000\n')
                elif event.code == "ABS_Z":
                    if event.state < (128 - deadband):
                        #left
                        q.put('a%03d\n' %  calc_percent(event.state))
                    elif event.state > (128 + deadband):
                        #right
                        q.put('d%03d\n' %  calc_percent(event.state))
                    else:
                        #stop
                        q.put('T000\n')
                elif event.code == "BTN_SOUTH":
                    q.put('1')
                elif event.code == "BTN_EAST":
                    q.put('2')
                elif event.code == "BTN_C":
                    q.put('3')
                elif event.code == "BTN_NORTH":
                    q.put('4')

                prev_time = time.time_ns()

#this thread communicates with all the modules on the robot
#it checks the queue to see if thread 1 had input any info from the gamepad and, if there is any data, it sends a message to the motor controller module
#it also stays in contact with the ultrasonic sensors to make sure no objects are in the path of the robot and, if there is any objects, it stops the robot
def communicator():
    state = 0
    dstop = 'F000'
    tstop = 'T000'

    def checkAllSens():
        uss.write(b'a')
        line = uss.readline()
        while not len(line) > 0:
            time.sleep(0.001)
            line = uss.readline()
        if len(line) > 0:
            data = line.decode('utf-8')
            if int(data) != -1:
                return True
            else:
                return False

    while 1:
        if not q.empty():
            com = q.get()
            print(com)
            mtr.write(com.encode('utf-8'))
            if 'w' in com:
                state = 1
            elif 's' in com:
                state = 2
            elif ('a' in com) or ('d' in com):
                state = 3
        if checkAllSens():
            mtr.write(dstop.encode('utf-8'))
            mtr.write(tstop.encode('utf-8'))
            state = 0

#the main function just opens up the above threads
if __name__ == '__main__':
    q = queue.Queue() #this is the queue that the two threads will be working from
    t1 = Thread(target=gamepad_receiver)
    t2 = Thread(target=communicator)
    t1.start()
    t2.start()
