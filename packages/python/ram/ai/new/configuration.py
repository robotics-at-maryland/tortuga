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
            curr = machine.addState(k, typ(**init))
        else:
            curr = machine.getState(k)

        if isinstance(curr, NestedState):
            initFromYAML(curr.getInnerStateMachine(), v)

    for i in config.get('transitions', []):
        machine.addTransition(*i)
