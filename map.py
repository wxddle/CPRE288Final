# -*- coding: utf-8 -*-
"""

@author: Tilden Chen
"""

import turtle
import time
import math



turtle.setup(1920, 1080)

#turtle.goto((x,y)) (0,0) is center



roomba = turtle.Turtle()    #roomba turtle
roomba.color("black")
roomba.penup()
roomba.setheading(90)  
roomba.forward(25)


obj = turtle.Turtle()       #object turtle
obj.color("red")
obj.penup()
obj.goto((20,80))


edge = turtle.Turtle()      #edge turtle
edge.color("blue")
edge.penup()
edge.goto((10,80))

end = turtle.Turtle()       #end post turtle
end.color("green")
end.penup()
end.goto((0,80))

p = roomba.position()       #stores roomba position

time.sleep(2)

roomba.goto(p)


def ObjToRoomba():
    y = roomba.ycor()       #stores y-coordinate in variable y
    x = roomba.xcor()       #stores x-coordinate in variable x
    obj.goto((x,y))
    
def EdgeToRoomba():
    y = roomba.ycor()       #stores y-coordinate in variable y
    x = roomba.xcor()       #stores x-coordinate in variable x
    edge.goto((x,y))

degrees = 0.0               #first edge degree
c = 0.0                     #distance of object
width = 0.0                 #stores width of object
    
def NplaceObj():            #places object when roomba is facing N
    y = roomba.ycor()       #stores y-coordinate in variable y
    x = roomba.xcor()       #stores x-coordinate in variable x
    y += c*math.sin(degrees)
    x += c*math.cos(degrees)
    obj.goto((x,y))
    
def SplaceObj():            #places object when roomba is facing S
    y = roomba.ycor()       #stores y-coordinate in variable y
    x = roomba.xcor()       #stores x-coordinate in variable x
    y += -c*math.sin(degrees)
    x += -c*math.cos(degrees)
    obj.goto((x,y))
    
def EplaceObj():            #places object when roomba is facing E
    y = roomba.ycor()       #stores y-coordinate in variable y
    x = roomba.xcor()       #stores x-coordinate in variable x
    y += -c*math.cos(degrees)
    x += c*math.sin(degrees)
    obj.goto((x,y))
    
def WplaceObj():            #places object when roomba is facing W
    y = roomba.ycor()       #stores y-coordinate in variable y
    x = roomba.xcor()       #stores x-coordinate in variable x
    y += c*math.cos(degrees)
    x += -c*math.sin(degrees)
    obj.goto((x,y))

def drawObject():           #draws obstacle found in scan
    y = obj.ycor()       #stores y-coordinate in variable y
    x = obj.xcor()       #stores x-coordinate in variable x
    obj.goto((x,y))
    obj.down()
    obj.circle(width/2)
    obj.up()
    
def drawYEdge():
    y = roomba.ycor()       #stores y-coordinate in variable y
    x = roomba.xcor()       #stores x-coordinate in variable x
    EdgeToRoomba()
    edge.down()
    edge.goto((x,y+1000.0))
    EdgeToRoomba()
    edge.down()
    edge.goto((x,y-1000.0))
    edge.up()
    
def drawXEdge():
    y = roomba.ycor()       #stores y-coordinate in variable y
    x = roomba.xcor()       #stores x-coordinate in variable x
    EdgeToRoomba()
    edge.down()
    edge.goto((x+1000.0,y))
    EdgeToRoomba()
    edge.down()
    edge.goto((x-1000.0,y))
    edge.up()
    

    
    
roomba.goto(0,400)
time.sleep(1)
#drawXEdge()

degrees = 30.0               #first edge degree
c = 40.0                     #distance of object
width = 10.0                 #stores width of object

NplaceObj()
drawObject()


#turtle.forward(25)    

#disList = [50,40,60,100,150]

#for i in disList:
#    
#  spiral.dot()                
#  spiral.setpos(0,0)      
#  spiral.forward(i)          
#  spiral.right(-10) 

  
 
  
#turtle.bye()