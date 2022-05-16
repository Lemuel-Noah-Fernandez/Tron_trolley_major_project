# -*- coding: utf-8 -*-

# import external modules
import pygame

# import our defined classes
from location_class import Location
import serialisation

# initialise the 'game'
pygame.init()

# Set up constant colours
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
BLUE = (60, 125, 235)
RED = (230, 45, 45)

# Open a game screen
window_x = 750
window_y = 750
window_size = (window_x, window_y)
screen = pygame.display.set_mode(window_size)
pygame.display.set_caption('Tron Lab Map')

# initial coordinates for trolley (these should be fed in from microcontroller later)
initial_x = 375
initial_y = 375

# create the trolley sprite
trolley = Location(RED, initial_x, initial_y)

# create the group that will hold all locations (e.g. item locations)
all_locations = pygame.sprite.Group()
all_locations.add(trolley)

# Control how fast the screen updates (FPS)
clock = pygame.time.Clock()

def draw_map():
    all_locations.update()
    screen.fill(BLACK)
    draw_aisles()
    all_locations.draw(screen)
    pygame.display.flip()
    clock.tick(30)

def controls(events, player):
    exit_game = False
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
    dummy = 0

# little mini loop to control the trolley by arrow keys
# replace this with data from microcontroller when ready    
exit_game = False
while exit_game == False:
    why = serialisation.hi
    #print(serialisation.hi)
    print("loool {}".format(serialisation.gyro_values[0]))
    events = pygame.event.get()
    exit_game = controls(events, trolley)
    draw_map()
    
# Exit the game
pygame.quit()
raise SystemExit
    
    
