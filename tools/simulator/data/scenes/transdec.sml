
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
            position: [-27, -10, 0]
            #position: [-22, -19, -3.05]
    
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
        
#        buoy1:
#            type: [sim.vision.IBuoy, sim.vision.Buoy]
#            position: [-15, 1, -1]

        
        pipe1:
            type: [sim.vision.IPipe, sim.vision.Pipe]
            position: [-22, -19, -3.05]
            orientation: [0, 0, 1, -45]
                
        pipe2:
            type: [sim.vision.IPipe, sim.vision.Pipe]
            position: [-12, -28, -3.05]
            orientation: [0, 0, 1, 0]

        pipe3:
            type: [sim.vision.IPipe, sim.vision.Pipe]
            position: [2, -28, -3.05]
            orientation: [0, 0, 1, -45]
                
        pipe4:
            type: [sim.vision.IPipe, sim.vision.Pipe]
            position: [11, -37, -3.05]
            orientation: [0, 0, 1, 45]

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
