
Scene:
    name: Main
    
    Resources:
        Zip: ['../media/packs/OgreCore.zip',
              '../media/packs/cubemapsJS.zip']
        FileSystem: ['../media/models',
                     '../media/primitives',
                     '../media/materials/textures',
                     '../media/materials/scripts']
                         
    SkyBox:
        material_name: 'Examples/CloudyNoonSkyBox'
        
    #Lights:
    #    light1:
    #        type: LT_POINT
    #        position: [0, 100, 100]
    ambient_light_colour: [1, 1, 1]
    
    Cameras:
        Main:
            position: [0, 0, 0]
            offset: [0, 1, 2]
    
    Robots:
        AUT: '../data/robots/aut.rml'
    
    Objects:
        gate:
            type: [Trigger, ram.sim.physics.Trigger]
            position: [0, 0, -3]
            Physical:
                mass: 0
                center_of_mass: [0, -0.15, 0]
                Shape:
                    type: box
                    size: [1, 1, 1]
                contact_information:
                    SubMat: ['GATE_ENTERED', 'GATE_EXITED']
    
        ground:
            type: [SceneObject, ram.sim.scene.SceneObject] 
            position: [0, -10, 0]
            Graphical:
                mesh: simple_terrain.mesh
                material: 'Simple/BeachStones'
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: simple_terrain.mesh
         
        water:
            type: [Visual, ram.sim.graphics.Visual]
            material:      
            orientation: [1, 0, 0, -90]
            
            Graphical:
                mesh: 'PLANE:water'
                width: 80
                height: 80
                normal: [0, 0, 1]
                material: 'Simple/Translucent'
                
        far_wall:
            type: [SceneObject, ram.sim.scene.SceneObject] 
            position: [0, -5, -40]
            
            Graphical:
                mesh: 'PLANE:far_wall'
                width: 80
                height: 20
                normal: [0, 0, 1]
                material: 'Simple/BumpyMetal'
            
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: far_wall
        
        rear_wall:
            type: [SceneObject, ram.sim.scene.SceneObject]
            position: [0, -5, 40]
            
            Graphical:
                mesh: 'PLANE:rear_wall'
                width: 80
                height: 20
                normal: [0, 0, -1]
                material: 'Simple/BumpyMetal'
                
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: rear_wall
        
                
        right_wall:
            type: [SceneObject, ram.sim.scene.SceneObject]
            position: [40, -5, 0]
            orientation: [0, 1, 0, -90]
            
            Graphical:
                mesh: 'PLANE:right_wall'
                width: 80
                height: 20
                normal: [0, 0, 1]
                material: 'Simple/BumpyMetal'
                
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: right_wall
        
        
        left_wall:
            type: [SceneObject, ram.sim.scene.SceneObject]
            position: [-40, -5, 0]
            orientation: [0, 1, 0, 90]
            
            Graphical:
                mesh: 'PLANE:left_wall'
                width: 80
                height: 20
                normal: [0, 0, 1]
                material: 'Simple/BumpyMetal'
                
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: left_wall