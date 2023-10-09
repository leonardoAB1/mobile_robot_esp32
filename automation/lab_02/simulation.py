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

class Robot:
    def __init__(self, startpos, robotImg, width):
        self.m2p=3779.52 # meters 2 pixels
        #robot dims
        self.w=width
        self.x=startpos[0]
        self.y=startpos[1]
        self.theta=0
        self.vl=0.01*self.m2p #meters/s
        self.vr=0.01*self.m2p

if __name__=="__main__":
    pygame.init()
    start=(200, 200) #start position
    dims=(600, 1200)
    running=True
    environment=Envir(dims)
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running=False
        pygame.display.update()
        environment.map.fill(environment.black)