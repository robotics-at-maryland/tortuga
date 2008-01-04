
Scene:
    name: Main
    
    Resources:
        Zip: ['data/media/nxogre/tutorialmedia.zip']
        FileSystem: ['data/media/nxogre']
                         
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
            position: [-18, 0, 0]
            offset: [-2, 0, 1]
    
    Robots:
        AUT: 'data/robots/nxaut.rml'
    
    Objects:
#        gate:
#            type: [Trigger, ram.sim.physics.Trigger]
#            position: [-18, 0, 0]
#            Physical:
#                mass: 0
#                Shape:
#                    type: box
#                    size: [1, 1, 1]
#                contact_information:
#                    SubMat: ['GATE_ENTERED', 'GATE_EXITED']
         
        water:
            type: [Visual, ram.sim.graphics.Visual]
            
            Graphical:
                mesh: 'PLANE:water'
                width: 40
                height: 40
                normal: [0, 0, 1]
                material: 'Simple/Translucent'
         
        ground:
            type: [SceneObject, ram.sim.scene.SceneObject] 
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
            type: [SceneObject, ram.sim.scene.SceneObject] 
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
            type: [SceneObject, ram.sim.scene.SceneObject]
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
            type: [SceneObject, ram.sim.scene.SceneObject]
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
            type: [SceneObject, ram.sim.scene.SceneObject]
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