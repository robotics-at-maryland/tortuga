# This is our config file
# It tells the program what kind of vehicle to create


GUI:
    # In Hz
    updateRate: 30
    guiConfigFile: ramsim.yml

Subsystems:
    VisionSystem:
        #type: SimVision
        type: IdealSimVision
        #cameraRate: 10
        depends_on: ["EventHub", "Vehicle", "Simulation"]

        # Recording Settings
        #maxRecordRate: 10 # Hz
        #forwardFile: forward.avi
        #downwardFile: downward.avi
        #forwardPort: 50000
        #downwardPort: 57002

#       RedLightDetector:       
#           intialMinPixels: 400
#           foundMinPixelScale: 0.85
#           lostMinPixelScale: 0.75
#           # About 20% of the total pixels
#           almostHitPixels: 61440.0
           
        DuctDetector:
            centerAlignedThreshold: 50
            alignedThreshold: 2
            minRedOverGreen: 0.9
            maxRedOverGreen: 1.1
            minRedOverBlue: 2.0
            minGreenOverBlueOnRedFailureForInsideDuct: 1.1
            minBlackPercent: 15
            maxBlackTotal: 100
            dilateIterations: 0
            erodeIterations: 0
            defaultGrowThreshX: 0.05
            defaultGrowThreshY: 0.05
            minXOverYToUpGrowThresh: 1.5
            uppedGrowThreshX: 0.5
            uppedGrowThreshY: 0.05
    SimSonar:
        type: SimSonar
        depends_on: ["Simulation", "Vehicle", "EventHub"]
        pingInterval: 2.0
           
    EventHub:
        type: EventHub
        
    # Queues events from background threads so they can be executed in python
    QueuedEventHub:
        depends_on: ["EventHub"]
        type: QueuedEventHub

    Vehicle:
        type: SimVehicle
        depends_on: ["Simulation", "EventHub"]

    # Exectures Motions
    MotionManager:
        type: MotionManager
        depends_on: ["Vehicle", "Controller", "QueuedEventHub", "EventHub"]

    # Handles Timers
    TimerManager:
        type: TimerManager
        depends_on: ["EventHub"]

    Ai:
        # Demo course without the barbed wire
        #taskOrder: ['ram.ai.course.Gate', 'ram.ai.course.PipeGate',
        #            'ram.ai.course.Light', 'ram.ai.course.Pipe1',
        #            'ram.ai.course.Bin', 'ram.ai.course.Pinger',
        #            'ram.ai.course.Octagaon']

        # Demo course with the barbed wire                    
        #taskOrder: ['ram.ai.course.Gate', 'ram.ai.course.PipeGate',
        #            'ram.ai.course.Light', 'ram.ai.course.Pipe2',
        #            'ram.ai.course.BarbedWire',
        #            'ram.ai.course.Pipe3', 'ram.ai.course.Bin']
        
        # Demo course with everything
        taskOrder: ['ram.ai.course.Gate', 'ram.ai.course.PipeGate',
                    'ram.ai.course.Light', 'ram.ai.course.Pipe2',
                    'ram.ai.course.BarbedWire',
                    'ram.ai.course.Pipe3', 'ram.ai.course.Bin',
                    'ram.ai.course.Pinger']
        type: AI

    StateMachine:
        type: StateMachine
        depends_on: ["EventHub", "QueuedEventHub", "Controller", "Vehicle", 
                     "MotionManager", "TimerManager", "VisionSystem", "Ai"]
                     
                     
        States:
            # Course wide settings
            ram.ai.course.Pipe:
                pipesToFind: 3
            ram.ai.course.Gate:
                pipeDelay: 0
       
            ram.ai.course.PipeGate:
                biasDirection: 0
                threshold: 45
         
            # Pipe AI
            ram.ai.pipe.BetweenPipes:
                forwardTime: 15
            ram.ai.pipe.Start:
                speed: 3
            ram.ai.pipe.Searching:
                speed: 5
                
    #RemoteController:
    #    type: RemoteController
    #    depends_on: ["Controller"]
    #    update_interval: -1
    #    yawGain: 50

    Controller:
        
        depends_on: ["Vehicle", "EventHub"]
        type: BWPDController
        holdCurrentHeading: 1
        holdCurrentDepth: 1
        #update_interval: 10
        ##################################
        ##GAINS SHOULD NEVER BE NEGATIVE##
        ##################################
        angularPGain: 10
        angularDGain: 1
        speedPGain: 3
        sidewaysSpeedPGain: 3

        #Select type of Rotational Gyro Bias Controller
        gyroPDType: 0
        
        #Select type of Depth Controller
        # set depthControlType = (1 for P control) 
        #                       (2 for observer control for a 2d depth state) 
        #                       (3 for observer control for a 4d depth state)
        #                       (4 for observer control with scaling for a 4d depth state)
        #                       (5 for PID control)
        depthControlType: 1
        dtMin: .001
        dtMax: .05
        #parameters for depth P controller
        depthPGain: 20
        #parameters for depth observer controller 
        depthKx: 34.06
        depthKy: 40.70
        depthLx: 43.53
        depthLy: 461.17


        #parameters for depth PID controller
        # PID 1
        #depthKp: 10
        #depthKi: 0.2
        #depthKd: 4

        # PID 2
        #depthKp: 60
        #depthKi: 0.5
        #depthKd: 20

        # PID 3
        depthKp: 80
        depthKi: 0.2
        depthKd: 30

        # PID 4
        #depthKp: 30
        #depthKi: 0.5
        #depthKd: 10
             

        depthSumError: 0
        depthPrevX: 0

        depthA1: 0
        depthA2: 1
        depthA3: 0
        depthA4: -.575
        depthBx: 0
        depthBy: 0.05
        depthCx: 1
        depthCy: 0


        depthOCGainK: [1, 2]
        depthOCGainL: [3, 4]

        # Observer Control for 4d depth state (controllers 3 and 4)
        # low gains:
        depthOCIA4: [-0.355, -539.9952, -1.6815, 0,
                      0, -8.4250, 1.0, 0,
                      0.05, -22.1456, -0.575, 0,
                      -0.355, -0.5952, -1.6815, 0]
        depthOCIB4: [539.4, 8.425, 22.1456, 0]
        depthOCIC4: [0, 0, 0, 1]
        # K gains: -2  L gains: -15
        #depthOCIA4: [-6, -70409, -199, 0,
        #               0, -45, 1.0, 0,
        #               0, -667, -1, 0,
        #              -6, -185, -199, 0]
        #depthOCIB4: [70224, 45, 667, 0]
        #depthOCIC4: [0, 0, 0, 1]
        # K gains: -2  L gains: -10
        #depthOCIA4: [-6, -20242, -240, 0,
        #              0, -29, 1.0, 0,
        #              0, -283, -1, 0,
        #             -6, -244, -248, 0]
        #depthOCIB4: [19998, 29, 283, 0]
        #depthOCIC4: [0, 0, 0, 1]

        #parameters for depth observer controller with integrator
        #depthOCIA4: [-0.355, -539.9952, -1.6815, 0, 
        #            0, -8.4250, 1.0, 0, 
        #            0.05, -22.1456, -0.575, 0, 
        #            -0.355, -0.5952, -1.6815, 0]
        #depthOCIB4: [539.4, 8.425, 22.1456, 0]
        #depthOCIC4: [0, 0, 0, 1]
        # K gains: -2       L gains: -15
        #depthOCIA4: [-6, -70409, -199, 0, 
        #            0, -45, 1.0, 0, 
        #            0, -667, -1, 0, 
        #            -6, -185, -199, 0]
        #depthOCIB4: [70224, 45, 667, 0]
        #depthOCIC4: [0, 0, 0, 1]
        # K gains: -2       L gains: -10
        #depthOCIA4: [-6, -20242, -248, 0, 
        #            0, -29, 1.0, 0, 
        #            0, -283, -1, 0, 
        #            -6, -244, -248, 0]
        #depthOCIB4: [19998, 29, 283, 0]
        #depthOCIC4: [0, 0, 0, 1]

        #parameters for ***discrete*** depth observer controller with integrator
        # K gains: -2      L gains: -100
        #depthOCIA4: [0, 12217, -469, 0, 
        #            0, 0, 0, 0, 
        #            0, -9, 0, 0, 
        #            0, 2932, 49, 1]
        #depthOCIB4: [-12221, 1, 9, -2937]
        #depthOCIC4: [0, 0, 0, 1]
        # K gains: -1      L gains: -100
        #depthOCIA4: [1, 10884, -494, 0, 
        #            0, 0, 0, 0, 
        #            0, -10, 0, 0, 
        #            0, 1246, 21, 1]
        #depthOCIB4: [-10884, 1, 10, -1247]
        #depthOCIC4: [0, 0, 0, 1]
        
        #parameters for depth LQG controller
        #parameters for depth LQG controller with integrator

        desiredSpeed: 0
        desiredDepth: 1
        desiredQuaternion: [0, 0, 0, 1]

#    RemoteController:
#        type: RemoteController
#        depends_on: ["Controller"]
#        update_interval: -1

    KeyboardController:
        depends_on: ["Controller"]
        type: KeyboardController

    Simulation:
        type: Simulation
        scene_path: ['%(RAM_SVN_DIR)s/tools/simulator/data/scenes']
        vehicleView: True
        #debug: 0
        
        Scenes:
            # Each scene must be on the given scene_path, this for example is 
            # in 'data/scenes/nxtest.xml'
            #Main: objectives/pipe.sml
            Main: objectives/pipe-threshold.sml
            #Main: objectives/bin.sml
            #Main: objectives/duct.sml
            #Main: objectives/multi_pipe.sml
            #Main: objectives/barbedwire.sml
            #Main: objectives/target.sml
            #Main: objectives/buoy.sml
            #Main: objectives/pinger.sml
            #Main: objectives/safe.sml
            #Main: transdec.sml
            #Main: nxtest.sml
            #Main: gen_course.sml
            #Main: safePing.sml
            #Main: shortCourse.sml
            #Main: nbrf_tank.sml
            #Main: erc_course.sml
        
        Graphics:
            Plugins:
                # TODO: Remove the extra paths
                search_path: ['%(PYTHON_OGRE_HOME)s/plugins',
                'C:\Libraries\PythonOgre\plugins',
                'C:\Developement\PythonOgre\plugins','C:\PythonOgre\plugins',
                '/home/lsinger/ram_deps/python-ogre-snapshot/plugins',
                '/opt/ram/local/lib/OGRE']
                
                plugins: ['RenderSystem_GL', 'Plugin_ParticleFX', 
                'Plugin_OctreeSceneManager']
                       
            RenderSystem:
                type: OpenGL
                
                OpenGL:
                    - [FSAA, '0']
                    - [RTT Preferred Mode, 'FBO']
                    - [VSync, 'No']
                    
        Input:
            KeyMap:
                # Camera Controls
                #W : CAM_FORWARD
                #A : CAM_LEFT 
                #S : CAM_BACK 
                #D : CAM_RIGHT
                #Q : CAM_UP 
                #E : CAM_DOWN
                #R : CAM_ZOOM_IN
                #F : CAM_ZOOM_OUT
                #V : CAM_TOGGLE_FOLLOW
                
                NUMPAD8 : CAM_FORWARD
                NUMPAD4 : CAM_LEFT 
                NUMPAD5 : CAM_BACK 
                NUMPAD6 : CAM_RIGHT
                NUMPAD7 : CAM_UP 
                NUMPAD9 : CAM_DOWN
                ADD : CAM_ZOOM_IN
                SUBTRACT : CAM_ZOOM_OUT
                NUMPAD2 : CAM_PITCH_UP
                NUMPAD0 : CAM_PITCH_DOWN
                NUMPAD1 : CAM_YAW_LEFT
                NUMPAD3 : CAM_YAW_RIGHT
                
                # Vehicle Controls
                #I : THRUST_FORE
                #K : THRUST_BACK
                #G : THRUST_KILL
                #J : TURN_LEFT
                #L : TURN_RIGHT
                #U : SURFACE
                #O : DIVE
                #Y : PITCH_UP
                #H : PITCH_DOWN
                #Z : ROLL_PORT
                #X : ROLL_STARBOARD
                #N : STRAFE_LEFT
                #M : STRAFE_RIGHT
                
                # Misc.
                CTRL+B : SCREENSHOT
