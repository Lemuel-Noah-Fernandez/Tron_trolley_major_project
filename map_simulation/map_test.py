# -*- coding: utf-8 -*-

# import external modules
import pygame
import super_alt
import math
import time

# import our defined classes
from location_alt import Location
#import serialisation
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

def search_and_run():
    # initial coordinates for trolley (these should be fed in from microcontroller later)
    initial_x = 100
    initial_y = 575
    
    # First runs search function 
    [aisle_name, aisle_row] = super_alt.main_search()
    aisle_num = int(aisle_name[6])
    print(aisle_num)
    print(type(aisle_num))

    # initialise the 'game'
    pygame.display.set_caption('Tron Lab Map')
    pygame.init()
    screen = pygame.display.set_mode(window_size)

    # Product coordinates
    #product_x = 350 + (200 * (aisle_num-1))
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
    return product_x, product_y, all_locations, clock, trolley, aisle_num, screen

def draw_map():
    all_locations.update()
    screen.fill(BLACK)
    draw_aisles()
    all_locations.draw(screen)
    pygame.display.flip()
    clock.tick(30)

def controls(events, player, aisle):
    exit_game = False
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

# TODO
# draw the aisles of the tron lab as white lines onto the map
# pygame.draw.line function should do it, just gotta get dimensions of some sort
# screen size can be adjusted if needed to make this easier
def draw_aisles():
    for i in range(5):
        pygame.draw.rect(screen, WHITE, pygame.Rect(((i+1)*200), 0, 100, 350), 0)
        

# little mini loop to control the trolley by arrow keys
# replace this with data from microcontroller when ready    
[product_x, product_y, all_locations, clock, trolley, aisle_num, screen] = search_and_run()
exit_game = False
aisle_num_serial = 0
angle = 0
counter = 0
while exit_game == False:
    counter += 1
    if counter % 100 == 0:
        aisle_num_serial += 1
    #serialisation.serialPort.write(b'5\n')
    #aisle_num_serial = serialisation.read_serial(serialisation.gyro_values, serialisation.serialPort)
    #if aisle_num_serial != None:
        #print(aisle_num_serial)

    events = pygame.event.get()
    exit_game = controls(events, trolley, aisle_num_serial)
    tan_angle = ((575 - product_y)/(product_x - (100 + (aisle_num_serial * 250))))
    if (product_x - (100 + (aisle_num_serial * 250))) >= 0:
        angle = math.degrees(math.atan(tan_angle))
    else:
        angle = 180 + math.degrees(math.atan(tan_angle))
    draw_map()

    # Break if the aisle has been reached
    if aisle_num_serial == aisle_num:
        font = pygame.font.Font(None, 100)
        text = font.render('Aisle reached!', True, (255,255,255))
        rect = text.get_rect(center=screen.get_rect().center)
        screen.blit(text, rect)
        pygame.display.flip()
        time.sleep(5)
        counter, aisle_num_serial = 0, 0
        [product_x, product_y, all_locations, clock, trolley, aisle_num, screen] = search_and_run()
        #break
# Exit the game
pygame.quit()
raise SystemExit