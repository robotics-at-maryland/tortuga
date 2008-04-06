
Scene:
    name: Main
    
    Resources:
        Zip: ['%(RAM_SVN_DIR)s/tools/simulator/data/media/nxogre/tutorialmedia.zip']
        FileSystem: ['%(RAM_SVN_DIR)s/tools/simulator/data/media/nxogre','%(RAM_SVN_DIR)s/tools/simulator/data/media/models']
                         
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
            position: [-80, 0, 10]
            #offset: [0, 0, 2]
            offset: [-2, 0, 1]
    
    Robots:
        AUT: '%(RAM_SVN_DIR)s/tools/simulator/data/robots/nxaut.rml'
    
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
        
        buoy1:
            type: [sim.vision.IBuoy, sim.vision.Buoy]
            position: [-15, 1, -1]
            
            Graphical:
                mesh: 'sphere.50cm.mesh'
                material: 'Simple/Red'
                scale: [0.15, 0.15, 0.15]
        
        tank_walls:
            type: [ram.sim.scene.ISceneObject, ram.sim.scene.SceneObject]
            position: [0, 0, 0]
            orientation: [1, 0, 0, 90]
            
            Graphical:
                mesh: 'TankWallsMesh.mesh'
                scale: [.3048, .3048, .3048]  # convert feet to meters
                material: 'TankWallConcrete'
            
            Physical:
                mass: 0
                Shape:
                    #type: box
                    #size: [0.6858, 0.2032, 0.2032]
                    type: mesh
                    mesh_name: 'TankWallsMesh.mesh'
        
        water:
            type: [ram.sim.graphics.IVisual, ram.sim.graphics.Visual]
            position: [0, 0, 0]
            orientation: [1, 0, 0, 90]
            
            Graphical:
                mesh: 'WaterSurfaceMesh.mesh'
                scale: [.3048, .3048, .3048]  # convert feet to meters
                material: 'Simple/Translucent'
