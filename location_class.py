# -*- coding: utf-8 -*-

import pygame

BLACK = (0,0,0)
DOT_DIAM = 10
DOT_RAD = DOT_DIAM / 2

class Location(pygame.sprite.Sprite):
    def __init__(self, colour, init_x, init_y):
        # Call the parent class (Sprite) constructor
        super().__init__()
        
        # create the dot surface
        self.image = pygame.Surface([DOT_DIAM, DOT_DIAM])
        self.image.fill(BLACK)
        self.image.set_colorkey(BLACK)
        
        # draw the dot to the dot surface (this does not draw to the map)
        pygame.draw.circle(self.image, colour, [DOT_RAD, DOT_RAD], DOT_RAD)
        
        # get the rectangle covering the dot surface
        self.rect = self.image.get_rect()
        
        # set initial position
        self.rect.x = init_x
        self.rect.y = init_y
        
    def move_left(self, pixels):
        self.rect.x -= pixels
    
    def move_right(self, pixels):
        self.rect.x += pixels
        
    def move_up(self, pixels):
        self.rect.y -= pixels
        
    def move_down(self, pixels):
        self.rect.y += pixels
        
    def set_position(self, pos_x, pos_y):
        self.rect.x = pos_x
        self.rect.y = pos_y