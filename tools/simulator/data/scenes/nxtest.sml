
Scene:
    name: Main
    
    Resources:
        Zip: ['%(RAM_SVN_DIR)s/tools/simulator/data/media/nxogre/tutorialmedia.zip']
        FileSystem: ['%(RAM_SVN_DIR)s/tools/simulator/data/media/nxogre']
                         
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
        AUT: '%(RAM_SVN_DIR)s/tools/simulator/data/robots/nxaut.rml'
    
    Objects:        
        pipe1:
            type: [sim.vision.IPipe, sim.vision.Pipe]
            position: [-15, 1, -3.05]
            orientation: [0, 0, 1, 45]
            
            Graphical:
                mesh: 'cube.1m.mesh'
                material: 'Simple/Orange'
                scale: [1.2192, 0.1524, 0.0254]
                
        buoy:
            type: [sim.vision.IBuoy, sim.vision.Buoy]
            position: [-11.46, 4.54, -1.5]
            orientation: [0, 0, 1, 45]
            
            Graphical:
                mesh: 'sphere.50cm.mesh'
                material: 'Simple/Red'
                scale: [0.15, 0.15, 0.15]
        
        pipe2:
            type: [sim.vision.IPipe, sim.vision.Pipe]
            position: [-7.92893, 8.07107, -3.05]
            orientation: [0, 0, 1, -35]
            
            Graphical:
                mesh: 'cube.1m.mesh'
                material: 'Simple/Orange'
                scale: [1.2192, 0.1524, 0.0254]
        
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