
Scene:
    name: Main
    
    Resources:
        FileSystem: ['%(RAM_SVN_DIR)s/tools/simulator/data/media/materials/scripts',
                     '%(RAM_SVN_DIR)s/tools/simulator/data/media/materials/textures',
                     '%(RAM_SVN_DIR)s/tools/simulator/data/media/models',
                     '%(RAM_SVN_DIR)s/tools/simulator/data/media/models/primitives']
                         
    #SkyBox:
    #    material_name: 'Examples/CloudyNoonSkyBox'
        
    Lights:
        light1:
            type: LT_POINT
            position: [-20, -20, 20]
            colour: [0.5, 0.5, 0.5]
    ambient_light_colour: [0.5, 0.5, 0.5]
    
    Cameras:
        Main:
            position: [0, -22.82, 0]
            #position: [22.82, 0, 10]
            #offset: [0, 0, 2]
            offset: [0, 0, 70]
    
    Robots:
        Tortuga: 
            path: '%(RAM_SVN_DIR)s/tools/simulator/data/robots/tortuga3.rml'
            position: [-23, -15, 0]
            orientation: [0, 0, 1, -55]
            #position: [-25, -1, 0]
            #orientation: [0, 0, 1, 60]
            #position: [-22, -19, -3.05]
    
    Objects:

        course:
            type: [sim.vision.ICourse, sim.vision.Course]

            startType: [sim.vision.IPipe, sim.vision.Pipe]
            start: pipe1
            startPos: [-16, -24]
            startDepth: -4.2672
            startHeading: 35

            pipe1:        
                greenBuoy:
                    type: [sim.vision.IBuoy, sim.vision.Buoy]
                    distance: 9.5
                    depth: -2.75
                    offset: -0.75
                    Graphical:
                        material: Simple/Green

                yellowBuoy:
                    type: [sim.vision.IBuoy, sim.vision.Buoy]
                    distance: 9.5
                    depth: -2.75
                    offset: 0.75
                    Graphical:
                        material: Simple/Yellow

                redBuoy:
                    type: [sim.vision.IBuoy, sim.vision.Buoy]
                    distance: 9.5
                    depth: -2.5
                    Graphical:
                        material: Simple/Red
                    

            redBuoy:
                pipe2:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 2
                    depth: -4.2672
                    heading: -15
                    offset: 0

            pipe2:
                hedge1:
                    type: [sim.vision.IHedge, sim.vision.Hedge]
                    distance: 9
                    depth: -2.5
                    heading: -10

                hedge1Buoy:
                    type: [sim.vision.IBuoy, sim.vision.Buoy]
                    distance: 9
                    depth: -2.5
                    Graphical:
                        material: Simple/Red

            hedge1:
                pipe3:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 1
                    depth: -4.2672
                    heading: -90
                    offset: 0.5

                pipe4:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 1.5
                    depth: -4.2672
                    heading: -50
                    offset: -0.5

            pipe3:
                grapesBuoy:
                    type: [sim.vision.IBuoy, sim.vision.Buoy]
                    distance: 20
                    depth: -2.5
                    Graphical:
                        material: Simple/Red

            pipe4:
                blackJackTable:
                    type: [ram.sim.object.IObject, sim.vision.BlackJackTable]
                    distance: 11
                    depth: -4.25
                    heading: 15
                    symbols: ['axe', 'clippers', 'hammer', 'machete']

                caesarBuoy:
                    type: [sim.vision.IBuoy, sim.vision.Buoy]
                    distance: 11
                    depth: -3
                    offset: -3
                    Graphical:
                        material: Simple/Red

            blackJackTable:
                pipe5:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 1.5
                    depth: -4.2672
                    heading: -15
                    offset: 0

                pipe6:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 1
                    depth: -4.2672
                    heading: -90
                    offset: 1

            pipe5:
                hedge2:
                    type: [sim.vision.IHedge, sim.vision.Hedge]
                    distance: 7
                    depth: -2.5
                    heading: 0

                hedge2Buoy:
                    type: [sim.vision.IBuoy, sim.vision.Buoy]
                    distance: 7
                    depth: -2.5
                    Graphical:
                        material: Simple/Red

            hedge2:
                pipe7:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 1.5
                    depth: -4.2672
                    heading: 0
                    offset: 0

            pipe7:
                pinger:
                    type: [sim.sonar.IPinger, sim.sonar.Pinger]
                    distance: 10
                    depth: -4
                    heading: 0
                    offset: 0
                

        tank_walls:
            type: [ram.sim.scene.ISceneObject, ram.sim.scene.SceneObject]
            position: [0, 0, 0]
            orientation: [0.97028687514552081, 0.17108808730113592, 0.17108808730113592, 91.727979443648806]
            
            Graphical:
                mesh: 'TankWallsMesh.mesh'
                scale: [.3048, .3048, .3048]  # convert feet to meters
                material: 'CompElement/TankWalls'
            
            Physical:
                mass: 0
                Shape:
                    #type: box
                    #size: [0.6858, 0.2032, 0.2032]
                    type: mesh
                    mesh_name: 'TankWallsMesh.mesh'
        
#        water:
#            type: [ram.sim.graphics.IVisual, ram.sim.graphics.Visual]
#            position: [0, 0, -1]
#            orientation: [1, 0, 0, 90]
#            
#            Graphical:
#                mesh: 'WaterSurfaceMesh.mesh'
#                scale: [.3048, .3048, .3048]  # convert feet to meters
#                material: 'Simple/Translucent'
