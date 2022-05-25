# This is a Python script to parse the example messages from a file.

import time
import serial
import struct
import traceback

aisle_num_serial = 0
MSG_HEADER_SIZE = 16
hi = 5
gyro_values = [10]
com_port = "COM4"
serialPort = serial.Serial(port=com_port, baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
# serialString = ""  # Used to hold data coming over UART

def read_serial(gyro_values, serialPort):
    # Wait until there is data waiting in the serial buffer
    #print("yoot")
    if serialPort.in_waiting > 0:

        try:

            header_bytes = serialPort.read(MSG_HEADER_SIZE)
            trigger = b'aisle: '
            outer_count = 0
            flag = 0
            for i in header_bytes:
                try:
                    if i == trigger[outer_count]:
                        outer_count += 1
                    else:
                        outer_count = 0
                except:
                    outer_count = 0
                
                if flag == 1:
                    try:
                        number = int(chr(i))
                        return number
                    except:
                        continue

                if outer_count == 7:
                    flag = 1
                

            if len(header_bytes) < MSG_HEADER_SIZE:
                # must be out of messages
                return False

            header_data = struct.unpack(">H8sHHH", header_bytes)
            #print("header sentinels: " + str(hex(header_data[0])) + ", " + str(hex(header_data[4])))

            message_type = header_data[1].split(b'\0', 1)[0]  # remove the null characters from the string
            #print(message_type)
            #print("message size: " + str(header_data[2]))

            #print(header_data)

            if message_type == b"text":
                text_bytes = serialPort.read(header_data[2])
                #print("text message: " + str(text_bytes))
                str_msg = text_bytes.decode("utf-8")
                #print(str_msg)
                if(str_msg[0] == "a"):
                    aisle_num_serial = int(str_msg[7])
                    #print(str_msg)
                    return aisle_num_serial
        except Exception as e:
            # Logs the error appropriately. 
            print(traceback.format_exc())
    
    else:
        time.sleep(0.05)

# main program entry point

if __name__ == '__main__':
    #hi = 5
    #gyro_values = [10]
    #read_file('C:/Users/Stewart Worrall/Documents/data/test.hex')
    while True:
        read_serial(gyro_values)

    print(len(gyro_values))