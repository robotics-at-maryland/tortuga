
Scene:
    name: Main
    
    Resources:
        FileSystem: ['%(RAM_SVN_DIR)s/tools/simulator/data/media/materials/scripts',
                     '%(RAM_SVN_DIR)s/tools/simulator/data/media/materials/textures',
                     '%(RAM_SVN_DIR)s/tools/simulator/data/media/models',
                     '%(RAM_SVN_DIR)s/tools/simulator/data/media/models/primitives']
        
    Lights:
        light1:
            type: LT_POINT
            position: [-20, -20, 20]
            colour: [0.5, 0.5, 0.5]
    ambient_light_colour: [0.5, 0.5, 0.5]
    
    Cameras:
        Main:
            position: [0, 0, 0]
            offset: [0, 0, 17]

    Robots:
        Tortuga: 
            path: '%(RAM_SVN_DIR)s/tools/simulator/data/robots/tortuga.rml'
            position: [3, -4.5, 0]
            orientation: [0, 0, 1, -200]
    
    Objects:        
        pipe1:
            type: [sim.vision.IPipe, sim.vision.Pipe]
            position: [-3.2, -3.45, -4]
            orientation: [0, 0, 1, -45]
                
        buoy:
            type: [sim.vision.IBuoy, sim.vision.Buoy]
            position: [-4.572, -1.524, -2.9]
            orientation: [0, 0, 1, 45]

        pipe2:
            type: [sim.vision.IPipe, sim.vision.Pipe]
            position: [-5.5, 0, -4]
            orientation: [0, 0, 1, 60]

        barbedWire:
              type: [ram.sim.object.IObject, sim.vision.BarbedWire]
              position: [-3, 3, -2.55]
              orientation: [0, 0, 1, 15]

        pipe3:
            type: [sim.vision.IPipe, sim.vision.Pipe]
            position: [-1, 2.5, -4]
            orientation: [0, 0, 1, -45]

        pipe4:
            type: [sim.vision.IPipe, sim.vision.Pipe]
            position: [-1, 3.2, -4]
            orientation: [0, 0, 1, -30]

        blackJackTable:
            #type: [sim.vision.IBin, sim.vision.Bin]
            type: [ram.sim.object.IObject, sim.vision.BlackJackTable]
            position: [3.3528, 2.1336, -4.7]
            orientation: [0, 0, 1, -45]
            symbols: ['ship', 'tank', 'aircraft', 'factory']

        pinger:
            type: [sim.sonar.IPinger, sim.sonar.Pinger]
            position: [0, 0, -7.5]
            
        safe:
            type: [sim.vision.ISafe, sim.vision.Safe]
            position: [0, 0, -7]
        
        water:
            type: [ram.sim.graphics.IVisual, ram.sim.graphics.Visual]
            Graphical:
                mesh: 'PLANE:water'
                width: 15.24
                height: 15.24
                normal: [0, 0, 1]
                material: 'Simple/Translucent'
         
        tank:
            type: [ram.sim.scene.ISceneObject, ram.sim.scene.SceneObject] 
            position: [0, 0, -7.62]
            # The tank is upside down
            orientation: [0.707107, 0.707107, -0.000000, 180.000000]
            #orientation: [1, 0, 0, 180]
            
            Graphical:
                mesh: 'NBRFTank.mesh'
                # The tank is modeled in 1 unit = 1 inch
                scale: [0.0254, 0.0254, 0.0254]
                
            Physical:
                mass: 0
                Shape:
                    type: mesh
                    mesh_name: NBRFTank.mesh
