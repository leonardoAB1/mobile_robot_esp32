import pygame
import math

class Envir:
    def __init__(self, dimensions):
        #colors
        self.black=(0,0,0)
        self.white=(255, 255, 255)
        self.green=(0,255, 0)
        self.blue=(0,0,255)
        self.red=(255, 0, 0)
        self.yellow=(255, 255, 0)
        #map dimensions
        self.height=dimensions[0]
        self.width=dimensions[1]
        #windows settings
        pygame.display.set_caption("Differential drive robot")
        self.map=pygame.display.set_mode((self.width, self.height))