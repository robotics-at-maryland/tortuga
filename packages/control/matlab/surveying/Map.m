classdef Map < dynamicprops
    %MAP Contains all data for all objects
    %   Stores Object data in a containers.Map()
    %   Stores an origin offset that translates the location data to the
    %   coordinate frame it was measured in.  This only affects the
    %   plotting of location data.
    %   
    %   originOffset.x - amount to shift in x direction
    %   originOffset.y - amount to shift in y direction
    
    methods
        function obj = Map(originOffsetX, originOffsetY)
            % Create Dynamic Properties (Instance Variables)
            obj.addprop('objectMap');
            obj.addprop('removedObjectMap');
            obj.addprop('originOffsetX');
            obj.addprop('originOffsetY');
            
            % Initialize the maps
            obj.objectMap = containers.Map();
            obj.removedObjectMap = containers.Map();
            
            % Check for Valid Input
            assert(isreal(originOffsetX));
            assert(isreal(originOffsetY));
            
            % Set Origin Offsets
            obj.originOffsetX = originOffsetX;
            obj.originOffsetY = originOffsetY;
            
            % Create an Origin Object
            origin = Object('Origin');
            origin.location.x = obj.originOffsetX;
            origin.location.sigx = 0;
            origin.location.y = obj.originOffsetY;
            origin.location.sigy = 0;
            
            % Add the Origin Object to the Map
            obj.objectMap('Origin') = origin;
        end
        
        function addObject(obj,name)
            obj.objectMap(name) = Object(name);
        end
        
        function removeObject(obj,name)
            obj.removedObjectMap(name) = objectMap(name);
            obj.objectMap.remove(name);
        end
        
        function o = getObject(obj,name)
            o = obj.objectMap(name);
        end
        
        function oM = getAllObjects(obj)
            oM = obj.objectMap;
        end
        
        function plot(obj)
        end
    end
    
end