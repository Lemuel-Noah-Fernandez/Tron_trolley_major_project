# This is a Python script to parse the example messages from a file.

import time
import serial
import struct
import traceback

MSG_HEADER_SIZE = 16
hi = 5
gyro_values = [10]
com_port = "COM8"
serialPort = serial.Serial(port=com_port, baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
# serialString = ""  # Used to hold data coming over UART

def read_packet(f, gyro_values):
    header_bytes = f.read(MSG_HEADER_SIZE)

    if len(header_bytes) < MSG_HEADER_SIZE:
        # must be out of messages
        return False

    header_data = struct.unpack(">H8sHHH", header_bytes)
    #print("header sentinels: " + str(hex(header_data[0])) + ", " + str(hex(header_data[4])))

    message_type = header_data[1].split(b'\0', 1)[0]  # remove the null characters from the string
    #print(message_type)
    #print("message size: " + str(header_data[2]))

    if message_type == b"text":
        text_bytes = f.read(header_data[2])
        print("text message: " + str(text_bytes))
        str_msg = text_bytes.decode("utf-8")
        print(str_msg[7])
        if(str_msg[0] == "a"):
            aisle_num_serial = int(str_msg[7])
    elif message_type == b"gyro":
        gyro_bytes = f.read(header_data[2])
        gyro_data = struct.unpack(">hhhhH", gyro_bytes)
        print("gyro message: " + str(gyro_data[1]) + ", " + str(gyro_data[2]) + ", " + str(gyro_data[3]) + ", time=" + str(gyro_data[4]))
        #print("\n")
        #gyro_values[0] = 2
        
        gyro_values.append(gyro_data[1])
        gyro_values.append(gyro_data[2])
        gyro_values.append(gyro_data[3])
        
    """
    elif message_type == b"buttons":
        buttons_bytes = f.read(header_data[2])
        print("buttons message: " + str(hex(buttons_bytes[1])) + ", time=" + str(buttons_bytes[2]))
    """
    return True


def read_file(file_name):
    # open the file
    with open(file_name, "rb") as f:
        while True:
            try:
                if not read_packet(f):
                    break
            except Exception as e:
                print(traceback.format_exc())
                break
                # Logs the error appropriately. 
    

def read_serial(gyro_values, serialPort):
    # Wait until there is data waiting in the serial buffer
    #print("yoot")
    if serialPort.in_waiting > 0:

        try:
            if not read_packet(serialPort, gyro_values):
                print("yeet")
        except Exception as e:
            # Logs the error appropriately. 
            print(traceback.format_exc())
    
    else:
        time.sleep(0.05)



# main program entry point
"""
if __name__ == '__main__':
    #hi = 5
    #gyro_values = [10]
    #read_file('C:/Users/Stewart Worrall/Documents/data/test.hex')
    while True:
        read_serial(gyro_values)

    print(len(gyro_values))
    """