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
        #Text
        self.font=pygame.font.Font("freesansbold.ttf", 50)
        self.text=self.font.render("default", True, self.white, self.black)
        self.textRect=self.text.get_rect()
        self.textRect.center=(dimensions[1]-600, dimensions[0]-100)

    def write_info(self, Vl, Vr, theta):
        txt=f"Vr = {Vr} Vl = {Vl} thetha={int(math.degrees(theta))}"
        self.text=self.font.render(txt, True, self.white, self.black)
        self.map.blit(self.text, self.textRect)


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
        self.maxspeed=0.02*self.m2p
        self.minspeed=0.02*self.m2p
        #graphics
        self.img=pygame.image.load(robotImg)
        self.rotated=self.img
        self.rect=self.rotated.get_rect(center=(self.x, self.y))
    
    def draw(self, map):
        map.blit(self.rotated, self.rect)

    def move(self, event=None):
        if event is not None:
            if event.type == pygame.KEYDOWN:
                if event.key==pygame.K_RIGHT:
                    self.vl+=0.001*self.m2p
                elif event.key==pygame.K_LEFT:
                    self.vl-=0.001*self.m2p
                elif event.key==pygame.K_DOWN:
                    self.vr+=0.001*self.m2p
                elif event.key==pygame.K_UP:
                    self.vr-=0.001*self.m2p
        self.x+=((self.vl+self.vr)/2)*math.cos(self.theta)*dt
        self.y-=((self.vl+self.vr)/2)*math.sin(self.theta)*dt
        self.theta+=(self.vr-self.vl)/self.w*dt

        self.rotated=pygame.transform.rotozoom(self.img, math.degrees(self.theta), 1)
        self.rect=self.rotated.get_rect(center=(self.x, self.y))

if __name__=="__main__":
    pygame.init()
    start=(200, 200) #start position
    dims=(600, 1200)
    running=True
    environment=Envir(dims)

    robot=Robot(start, r"automation\lab_02\diff_robot.png", 0.194*3779.52)
    #simulation loop
    dt=0
    lasttime=pygame.time.get_ticks()
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running=False
            robot.move(event)

        dt=(pygame.time.get_ticks()-lasttime)/1000
        lasttime=pygame.time.get_ticks()

        pygame.display.update()
        environment.map.fill(environment.black)

        robot.move(event)
        robot.draw(environment.map)
        environment.write_info(int(robot.vr), int(robot.vl), int(robot.theta))