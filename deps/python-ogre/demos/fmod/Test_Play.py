import FMOD
import sys
from msvcrt import * 

if __name__ == '__main__':
    # you can't create a system directly ..ie FMOD.System() is invalid
    system = FMOD.System_Create()
    
    system.init(1,0,FMOD.CastVoidPtr(0)) #  
    
    FMOD_LOOP_OFF = 1
    FMOD_HARDWARE = 2 * 16
    FMOD_SOFTWARE = 4 * 16
    
    # yet this works fine!!, However it's a function transformation case so has been "wrapped"
    result, sound1 = system.createSound( "../media/sounds/drumloop.wav", 33 , FMOD.CastVoidPtr(0)) 
    #result = sound1.setMode( FMOD_LOOP_OFF )

    result, sound2 = system.createSound( "../media/sounds/jaguar.wav", 64, FMOD.CastVoidPtr(0))
    result, sound3 = system.createSound( "../media/sounds/swish.wav", 32, FMOD.CastVoidPtr(0))

    print("===================================================================")
    print("PlaySound Example.  Copyright (c) Firelight Technologies 2004-2005.")
    print("===================================================================")
    print("\n")
    print("Press '1' to play a mono sound using hardware mixing")
    print("Press '2' to play a mono sound using software mixing")
    print("Press '3' to play a stereo sound using hardware mixing")
    print("Press 'Esc' to quit")
    print("")


    channel = None
    key = ' '
    while (ord(key) != 27):
        if (kbhit()):
            key = getch()
            if key == '1':
                result, channel = system.playSound(FMOD.FMOD_CHANNEL_FREE, sound1, False)
            if key == '2':
                result, channel = system.playSound(FMOD.FMOD_CHANNEL_FREE, sound2, False)
            if key =='3' :
                result, channel = system.playSound(FMOD.FMOD_CHANNEL_FREE, sound3, False)
        system.update() 
        
    
    result = sound1.release()
    result = sound2.release()
    result = sound3.release()
    result = system.close()
    result = system.release()



