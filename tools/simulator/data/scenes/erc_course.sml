
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
            position: [0, 0, 0]
            offset: [0, 0, 60]

    
    Robots:
        Tortuga: 
            path: '%(RAM_SVN_DIR)s/tools/simulator/data/robots/tortuga.rml'
            position: [-23, 10, 0]
            orientation: [0, 0, 1, -45]
    
    Objects:        
        course:
            type: [sim.vision.ICourse, sim.vision.Course]
            
            startType: [sim.vision.IPipe, sim.vision.Pipe]
            start: pipe1
            startPos: [-18, 6]
            startDepth: 0
            startHeading: 60
            
            pipe1:
                buoy:
                    type: [sim.vision.IBuoy, sim.vision.Buoy]
                    distance: 8
                    depth: 0

            buoy:
                pipe2:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 3
                    depth: 0
                    heading: -80
           
            pipe2:
                barbedwire:
                    type: [sim.vision.IBarbedWire, sim.vision.BarbedWire]
                    distance: 7
                    depth: 0
                    heading: 25
                    
            barbedwire:
                rightpipe:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 3
                    heading: 45
                    offset: -1
                    depth: 0
                leftpipe:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 3
                    heading: -35
                    offset: 1
                    depth: 0
            
            rightpipe:
                target:
                    type: [ram.sim.object.IObject, sim.vision.Target]
                    distance: 8
                    heading: -30
                    depth: 0
            
            target:
                targetTablePipe:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 2
                    heading: -10
                    direction: -90
                    depth: 0
                targetSonarPipe:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 2
                    heading: -40
                    depth: 0
            
            leftpipe:
                blackJackTable:
                    type: [ram.sim.object.IObject, sim.vision.BlackJackTable]
                    distance: 9
                    heading: 10
                    depth: 0
            
            blackJackTable:
                tableSonarPipe:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 2
                    heading: 15
                    depth: 0
                    
                tableTargetPipe:
                    type: [sim.vision.IPipe, sim.vision.Pipe]
                    distance: 3
                    heading: 37.5
                    direction: 90
                    depth: 0
            
            tableSonarPipe:
                sonar:
                    type: [sim.vision.ISafe, sim.vision.Safe]
                    distance: 16
                    depth: 0
            
        #water:
        #    type: [ram.sim.graphics.IVisual, ram.sim.graphics.Visual]
        #    Graphical:
        #        mesh: 'PLANE:water'
        #        width: 50
        #        height: 25
        #        normal: [0, 0, 1]
        #        material: 'Simple/Translucent'
         
        ground:
            type: [ram.sim.scene.ISceneObject, ram.sim.scene.SceneObject] 
            position: [0, 0, -3.353]
            
            Graphical:
                mesh: 'PLANE:ground'
                width: 25
                height: 50
                normal: [0.0, 0.5, 0.5] #[0.0, 0.0365516170574, 0.999331766377]
                material: 'Simple/BumpyMetal'
                utile: 15
                vtile: 15
                
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: ground
                
        north_wall:
            type: [ram.sim.scene.ISceneObject, ram.sim.scene.SceneObject] 
            position: [12.5, 0, -2.134]
            
            Graphical:
                mesh: 'PLANE:far_wall'
                width: 50
                height: 4.267
                normal: [-1, 0, 0]
                upvec: [0, 0, 1]
                material: 'Simple/FlatMetal'
                utile: 12
                vtile: 3
            
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: far_wall
        
        south_wall:
            type: [ram.sim.scene.ISceneObject, ram.sim.scene.SceneObject]
            position: [-12.5, 0, -2.134]
            
            Graphical:
                mesh: 'PLANE:rear_wall'
                width: 50
                height: 4.267
                normal: [1, 0, 0]
                upvec: [0, 0, 1]
                material: 'Simple/FlatMetal'
                utile: 12
                vtile: 3
                
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: rear_wall
        
                
        west_wall:
            type: [ram.sim.scene.ISceneObject, ram.sim.scene.SceneObject]
            position: [0, 25, -2.134]
            #orientation: [0, 1, 0, -90]
            
            Graphical:
                mesh: 'PLANE:right_wall'
                width: 25
                height: 4.267
                normal: [0, -1, 0]
                upvec: [0, 0, 1]
                material: 'Simple/FlatMetal'
                utile: 12
                vtile: 3
                
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: right_wall
        
        
        east_wall:
            type: [ram.sim.scene.ISceneObject, ram.sim.scene.SceneObject]
            position: [0, -25, -1.219]
            #orientation: [0, 1, 0, 90]
            
            Graphical:
                mesh: 'PLANE:left_wall'
                width: 25
                height: 2.438
                normal: [0, 1, 0]
                upvec: [0, 0, 1]
                material: 'Simple/FlatMetal'
                utile: 12
                vtile: 3
                
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: left_wall