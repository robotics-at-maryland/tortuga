import testing
import ram.ai.new.stateMachine as stateMachine



class TestMachine(stateMachine.StateMachine):
    def __init__(self):
        super(TestMachine, self).__init__()

        # best practice, initialize states out here separately
        # set all the unique instances' variables correctly
        # then add the mapping
        # this is okay here because everything is hard coded in
        # the __init__
        self._stateMap = {'test1' : testing.testState1(self),
                          'test2' : testing.testState2(self),
                          'test3' : testing.testState3(self)}
