import yaml

from state import *
from stateMachine import *

from utilStates import NestedState

def initFromYAML(machine, config):
    for k,v in config.get('states', {}).iteritems():
        curr = None
        if 'type' in v:
            typ = MetaState.getStateType(v['type'])
            init = v.get('init', {})
            print 'Creating ' + k
            curr = machine.addState(k, typ(**init))
        else:
            print 'Found ' + k
            curr = machine.getState(k)

        if isinstance(curr, NestedState):
            print 'Recursing into ' + k
            initFromYAML(curr.getInnerStateMachine(), v)

    for i in config.get('transitions', []):
        machine.addTransition(*i)

        print 'Added: ' + str(i)
