# -*- coding: utf-8 -*-

# import external modules
import pygame
import supermarket
import math
import time

# import our defined classes
from location_class import Location
import serialisation

# First runs search function 
found = False
while found == False:
    [aisle_name, aisle_row, search_word, found] = supermarket.main_search()
aisle_num = int(aisle_name[6])
print(aisle_num)
print(type(aisle_num))

# initialise the 'game'
pygame.init()

# Set up constant colours
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
BLUE = (60, 125, 235)
RED = (230, 45, 45)

# Open a game screen
# Dimensions of room are roughly (750, 450), so I've multiplied those numbers by 1.7
window_x = 1275
window_y = 765
window_size = (window_x, window_y)
screen = pygame.display.set_mode(window_size)
pygame.display.set_caption('Tron Lab Map')

# initial coordinates for trolley (these should be fed in from microcontroller later)
initial_x = 100
initial_y = 575

# Product coordinates
if aisle_row <= 4:
    product_x = 300 + (200 * (aisle_num-1))
    product_y = 25 + (75 * aisle_row)  
else:
    product_x = 370 + (200 * (aisle_num-1))
    product_y = 25 + (75 * (aisle_row - 4))

# Create the product sprite
product = Location(BLUE, product_x, product_y)

# create the trolley sprite
trolley = Location(RED, initial_x, initial_y)

# create the group that will hold all locations (e.g. item locations)
all_locations = pygame.sprite.Group()
all_locations.add(trolley)
all_locations.add(product)

# Control how fast the screen updates (FPS)
clock = pygame.time.Clock()

def draw_map():
    all_locations.update()
    screen.fill(BLACK)
    draw_aisles()
    all_locations.draw(screen)
    pygame.display.flip()
    clock.tick(30)

def controls(events, player, aisle):
    exit_game = False

    # Updating trolley position
    player.set_position(135 + (aisle * 200), 575)

    # Loop through all actions taken by user
    for event in events:
        # If user clicks close button exit game
        if event.type == pygame.QUIT:
            exit_game = True
        elif event.type == pygame.KEYDOWN:
            # Use arrow keys to move left and right
            if event.key == pygame.K_ESCAPE:
                exit_game = True
            elif event.key == pygame.K_LEFT:
                player.move_left(15)
            elif event.key == pygame.K_RIGHT:
                player.move_right(15)
            elif event.key == pygame.K_UP:
                player.move_up(15)
            elif event.key == pygame.K_DOWN:
                player.move_down(15)
    return exit_game

def draw_aisles():
    for i in range(5):
        pygame.draw.rect(screen, WHITE, pygame.Rect(((i+1)*200), 0, 100, 350), 0)  

def send_message(serialPort, message):
    msg = message + '\r'

    for i in msg:
        send_msg = i.encode("utf-8")
        serialPort.write(send_msg)
        time.sleep(0.01)
    
    return 
  
exit_game = False
aisle_num_serial = 0
while exit_game == False:
    # Writing to serial port
    send_message(serialisation.serialPort, "L {}: Aisle {}".format(search_word, aisle_num))
    time.sleep(0.1)
    send_message(serialisation.serialPort, "S {}".format((aisle_num_serial)))
    time.sleep(0.1)
    
    # Reading from serial port
    aisle_num_serial = serialisation.read_serial(serialisation.gyro_values, serialisation.serialPort)
    if aisle_num_serial != None:
        print(aisle_num_serial)
    else:
        aisle_num_serial = 0
    
    events = pygame.event.get()
    exit_game = controls(events, trolley, aisle_num_serial)

    # Calculating the gyro angle needed
    tan_angle = ((575 - product_y)/(product_x - (100 + (aisle_num_serial * 250))))
    if (product_x - (100 + (aisle_num_serial * 250))) >= 0:
        angle = math.degrees(math.atan(tan_angle))
    else:
        angle = 180 + math.degrees(math.atan(tan_angle))
    if angle <= 25:
        angle = 50
    elif 26 < angle < 75:
        angle = 30
    elif 75 < angle < 115:
        angle = 19
    elif 115 < angle < 150:
        angle = 6
    else:
        angle = 1
    send_message(serialisation.serialPort, "A {}".format(angle))
    draw_map()
    
# Exit the game
pygame.quit()
raise SystemExit
    
    
