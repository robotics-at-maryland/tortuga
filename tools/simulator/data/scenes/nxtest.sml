
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
            position: [-13, 6, 0]
            offset: [0, 0, 15]

    
    Robots:
        Tortuga: 
            path: '%(RAM_SVN_DIR)s/tools/simulator/data/robots/tortuga.rml'
            position: [-17, -1, 0]
            orientation: [0, 0, 1, 45]
    
    Objects:        
        course:
            type: [sim.vision.ICourse, sim.vision.Course]
            order: [pipe1, buoy, pipe2, blackJackTable, pipe3, pinger, safe]
            startPos: [-15, 1, -3.05] 
            
            pipe1:
                type: [sim.vision.IPipe, sim.vision.Pipe]
                heading: -45
                distance: 5
                depth: -3.05
            
            buoy:
                type: [sim.vision.IBuoy, sim.vision.Buoy]
                distance: 5
                depth: -2.9

            pipe2:
                type: [sim.vision.IPipe, sim.vision.Pipe]
                heading: 75
                distance: 7
                depth: -3.05

            blackJackTable:
                type: [ram.sim.object.IObject, sim.vision.BlackJackTable]
                distance: 7
                offset: 1
                depth: -3.05
                
            pipe3:
                type: [sim.vision.IPipe, sim.vision.Pipe]
                heading: 60
                distance: 13
                offset: 3
                depth: -3.05

            pinger:
                type: [sim.sonar.IPinger, sim.sonar.Pinger]
                depth: -4
                
            safe:
                type: [sim.vision.ISafe, sim.vision.Safe]
                depth: -3.1
        
        water:
            type: [ram.sim.graphics.IVisual, ram.sim.graphics.Visual]
            Graphical:
                mesh: 'PLANE:water'
                width: 40
                height: 40
                normal: [0, 0, 1]
                material: 'Simple/Translucent'
         
        ground:
            type: [ram.sim.scene.ISceneObject, ram.sim.scene.SceneObject] 
            position: [0, 0, -5]
            
            Graphical:
                mesh: 'PLANE:ground'
                width: 40
                height: 40
                normal: [0, 0, 1]
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
            position: [20, 0, -2]
            
            Graphical:
                mesh: 'PLANE:far_wall'
                width: 40
                height: 6
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
            position: [-20, 0, -2]
            
            Graphical:
                mesh: 'PLANE:rear_wall'
                width: 40
                height: 6
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
            position: [0, 20, -2]
            #orientation: [0, 1, 0, -90]
            
            Graphical:
                mesh: 'PLANE:right_wall'
                width: 40
                height: 6
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
            position: [0, -20, -2]
            #orientation: [0, 1, 0, 90]
            
            Graphical:
                mesh: 'PLANE:left_wall'
                width: 40
                height: 6
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