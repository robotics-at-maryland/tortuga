
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
            position: [0, 200, 100]
            colour: [0.5, 0.5, 0.5]
    ambient_light_colour: [0.5, 0.5, 0.5]
    
    Cameras:
        Main:
            position: [5, 0, -5]
            offset: [0, 1, 2]
    
    Robots:
        AUT: 'data/robots/nxaut.rml'
    
    Objects:
        gate:
            type: [Trigger, sim.physics.Trigger]
            position: [5, 0, -8]
            Physical:
                mass: 0
                Shape:
                    type: box
                    size: [1, 1, 1]
                contact_information:
                    SubMat: ['GATE_ENTERED', 'GATE_EXITED']
    
        ground:
            type: [SceneObject, sim.scene.SceneObject] 
            position: [0, -5, 0]
            Graphical:
                mesh: nx.floor2.mesh
                material: 'nxogrefloor'
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: nx.floor2.mesh
         
        #water:
        #    type: [Visual, sim.graphics.Visual]
        #    material:      
        #    orientation: [1, 0, 0, -90]
            
        #    Graphical:
        #        mesh: 'PLANE:water'
        #        width: 80
        #        height: 80
        #        normal: [0, 0, 1]
        #        material: 'Simple/Translucent'
                
        far_wall:
            type: [SceneObject, sim.scene.SceneObject] 
            position: [0, -5, -40]
            
            Graphical:
                mesh: 'PLANE:far_wall'
                width: 80
                height: 20
                normal: [0, 0, 1]
                visible: False
                material: 'Simple/BumpyMetal'
            
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: far_wall
        
        rear_wall:
            type: [SceneObject, sim.scene.SceneObject]
            position: [0, -5, 40]
            
            Graphical:
                mesh: 'PLANE:rear_wall'
                width: 80
                height: 20
                normal: [0, 0, -1]
                material: 'Simple/BumpyMetal'
                visible: False
                
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: rear_wall
        
                
        right_wall:
            type: [SceneObject, sim.scene.SceneObject]
            position: [40, -5, 0]
            orientation: [0, 1, 0, -90]
            
            Graphical:
                mesh: 'PLANE:right_wall'
                width: 80
                height: 20
                normal: [0, 0, 1]
                material: 'Simple/BumpyMetal'
                visible: False
                
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: right_wall
        
        
        left_wall:
            type: [SceneObject, sim.scene.SceneObject]
            position: [-40, -5, 0]
            orientation: [0, 1, 0, 90]
            
            Graphical:
                mesh: 'PLANE:left_wall'
                width: 80
                height: 20
                normal: [0, 0, 1]
                material: 'Simple/BumpyMetal'
                visible: False
                
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: left_wall