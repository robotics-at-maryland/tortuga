
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
            path: '%(RAM_SVN_DIR)s/tools/simulator/data/robots/tortuga.rml'
            #position: [-27, -10, 0]
            #orientation: [0, 0, 1, -55]
            position: [-25, -1, 0]
            orientation: [0, 0, 1, 60]
            #position: [-22, -19, -3.05]
    
    Objects:
        course:
            type: [sim.vision.ICourse, sim.vision.Course]

            startType: [sim.vision.IPipe, sim.vision.Pipe]
            start: pipe1
            startPos: [-22, -19]
            startDepth: -3.05
            startHeading: 45

            pipe1:        
                buoy:
                    type: [sim.vision.IBuoy, sim.vision.Buoy]
                    distance: 6
                    depth: -1.425

            buoy:
                pipe2:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 2
                    depth: -3.05
                    heading: -45

            pipe2:
                barbedwire:
                    type: [sim.vision.IBarbedWire, sim.vision.BarbedWire]
                    distance: 7
                    depth: -0.953
                    heading: -5

            barbedwire:
                rightpipe:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 0
                    heading: 60
                    offset: -0.7
                    depth: -3.05
                leftpipe:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 2
                    heading: -35
                    offset: 0.7
                    depth: -3.05

            leftpipe:
                bin:
                    type: [ram.sim.object.IObject, sim.vision.BlackJackTable]
                    distance: 6
                    heading: 30
                    depth: -3.9
                    symbols: ['ship', 'tank', 'aircraft', 'factory']

            rightpipe:
                target:
                    type: [ram.sim.object.IObject, sim.vision.Target]
                    distance: 8
                    heading: -10
                    depth: -1.67

        course2:
            type: [sim.vision.ICourse, sim.vision.Course]

            startType: [sim.vision.IPipe, sim.vision.Pipe]
            start: pipe3
            startPos: [-22, 6]
            startDepth: -3.05
            startHeading: -60

            pipe3:        
                buoy2:
                    type: [sim.vision.IBuoy, sim.vision.Buoy]
                    distance: 6
                    depth: -1.425

            buoy2:
                pipe4:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 2
                    depth: -3.05
                    heading: 10

            pipe4:
                barbedwire2:
                    type: [sim.vision.IBarbedWire, sim.vision.BarbedWire]
                    distance: 7
                    depth: -0.953
                    heading: -5

            barbedwire2:
                rightpipe2:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 2
                    heading: 70
                    offset: -0.7
                    depth: -3.05
                leftpipe2:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 2
                    heading: -5
                    offset: 0.5
                    depth: -3.05

            leftpipe2:
                bin2:
                    type: [ram.sim.object.IObject, sim.vision.BlackJackTable]
                    distance: 6
                    heading: 20
                    depth: -3.9
                    symbols: ['ship', 'tank', 'aircraft', 'factory']

            rightpipe2:
                target2:
                    type: [ram.sim.object.IObject, sim.vision.Target]
                    distance: 8
                    heading: -30
                    depth: -1.67

        tank_walls:
            type: [ram.sim.scene.ISceneObject, ram.sim.scene.SceneObject]
            position: [0, 0, 0]
            orientation: [1, 0, 0, 90]
            
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
