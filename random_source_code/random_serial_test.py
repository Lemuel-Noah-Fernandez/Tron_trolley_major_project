# This is a Python script to parse the example messages from a file.

import time
import serial
import struct
import traceback

MSG_HEADER_SIZE = 16
hi = 5
gyro_values = [10]
com_port = "COM4"
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
        #print("text message: " + str(text_bytes))
        str_msg = text_bytes.decode("utf-8")
        print(str_msg)
        if(str_msg[0] == "a"):
            aisle_num_serial = int(str_msg[7])
            return aisle_num_serial
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

def send_message(serialPort, message):
    
    msg = message + '\r'

    for i in msg:
        send_msg = i.encode("utf-8")
        serialPort.write(send_msg)
        time.sleep(0.01)
    
    return 


# main program entry point

if __name__ == '__main__':

    # msg = input("ready? ")
    # send_message(serialPort, "R ")

    # while(True):
    #     value = read_serial(gyro_values, serialPort)
    #     send_message(serialPort, "R ")
    #     time.sleep(0.01)
    #     if value != None:
    #         print(value)
    while True:
        msg = input("Enter a command: ")
        send_message(serialPort, msg)