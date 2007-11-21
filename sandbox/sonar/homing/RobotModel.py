from math import *

class RobotModel:
  def __init__(pingerx, pingery, tfirstping):
    self.px = self.py = self.vx = self.vy = self.ax = self.ay = 0
    self.theta = self.omega = self.alpha = 0
    self.t = 0;
    self.c = 1500;
    self.pingrate = 2;
    self.pingerx = pingerx
    self.pingery = pingery
    self.tfirstping = tfirstping
  
  def timeStep():
    self.px += self.vx * dt + 0.5 * self.ax * self.dt ** 2
    self.py += self.vy * dt + 0.5 * self.ay * self.dt ** 2
    self.vx += self.dt * self.ax
    self.vy += self.dt * self.ay
    self.theta += self.omega * dt + 0.5 * self.alpha * dt ** 2
    self.omega += self.alpha * dt
    self.t  += self.dt
    oldPingerParity = self.pingerParity
    self.pingerParity = int((t + self.tfirstping)/self.pingrate) % 2
    if oldPingerParity != self.pingerParity:
      self.tlastping = t
    
  def _getPingerRange():
    return sqrt((self.px - self.pingerx)**2+(self.py-self.pingery)**2)
  
  def getRangeRate():
    vdotp = self.vx + (self.px - self.pingerx) + self.vy * (self.py - self.pingery)
    return vdotp/self._getPingerRange()



