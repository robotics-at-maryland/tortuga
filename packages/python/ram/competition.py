import ram.core as core
import math

class ICompetitionObjective(core.Interface):
    pass

class StartingGate(object):
    core.implements(ICompetitionObjective)

    def __init__(self):
        # Gate Releated Members
        self._through_gate = False
        self._gate_entered_velocity = None

    def _entered_gate(self, vehicle):
        self._gate_entered_velicty = vehicle.velocity

    def _left_gate(self, vehicle):
        if not self._through_gate:
            velA = self._gate_entered_velicty 
            velB = vehicle.velocity
            angle =  math.acos((velA.absDotProduct(velB)) / 
                           (velA.length() * velB.length()))    
            if math.degrees(angle) < 10:
                print 'Straight Through!'
            self._through_gate = True
