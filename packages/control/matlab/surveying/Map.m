classdef Map < dynamicprops
    %MAP Contains all data for all objects
    %   Stores Object data in a containers.Map()
    %   Stores an origin offset that translates the location data to the
    %   coordinate frame it was measured in.  This only affects the
    %   plotting of location data.
    %
    %   originOffset.x - amount to shift in x direction
    %   originOffset.y - amount to shift in y direction
    
    properties
        img = imread('transdec_above.jpeg');
    end
    
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
            origin.location.xobj = obj.originOffsetX;
            origin.location.sigxobj = 0;
            origin.location.yobj = obj.originOffsetY;
            origin.location.sigyobj = 0;
            
            % Add the Origin Object to the Map
            obj.objectMap('Origin') = origin;
        end
        
        function addObject(obj,name)
            obj.objectMap(name) = Object(name);
        end
        
        function removeObject(obj,name)
            obj.removedObjectMap(name) = obj.objectMap(name);
            obj.objectMap.remove(name);
        end
        
        function o = getObject(obj,name)
            k = obj.objectMap.keys();
            [m n] = size(k);
            for i = 1:n
                if(strcmp(char(k(i)),name))
                    o = obj.objectMap(name);
                    break;
                else
                    o = Null();
                end
            end
        end
        
        function oM = getAllObjects(obj)
            oM = obj.objectMap;
        end
        
        function updateResults(map, object)
            updated = 0;
            if(isa(object,'Object') && ~strcmp(object.name,'Origin'))
                object.updateLocation();
                numObjects = map.objectMap.Count;
                for i = 1:int32(numObjects)
                    oKeys = map.objectMap.keys;
                    curObject = map.objectMap(char(oKeys(i)));
                    if ~(strcmp(curObject.name,object.name))
                        numMes = curObject.measurements.Count;
                        for j = 1:int32(numMes)
                            mKeys = curObject.measurements.keys;
                            curMes = curObject.measurements(char(mKeys(j)));
                            if(strcmp(curMes.associatedObject,object.name))
                                curMes.x = object.location.xobj;
                                curMes.sigx = object.location.sigx;
                                curMes.y = object.location.yobj;
                                curMes.sigy = object.location.sigyobj;
                                updated = 1;
                            end
                        end
                        if updated
                            updateResults(map,curObject);
                        end
                        updated = 0;
                    end
                end
            end
        end
        
        function a = plot(obj, a)
            %image is 813px x 536px
            set(a,'XLim', [-50,50]);
            set(a,'YLim', [-35,35]);
            k = obj.objectMap.keys();
            hold(a);
            a = imagesc([-50,50],[-35,35],obj.img);
            n = size(k);
            for i = 1:n(2)
                o = obj.objectMap(char(k(i)));
                if (isreal(o.location.xobj) && isreal(o.location.yobj))
                    if strcmp(o.name, 'Origin')
                        plot(obj.originOffsetX,obj.originOffsetY,'or');
                    else
                        plot(obj.originOffsetX + o.location.xobj,obj.originOffsetY + o.location.yobj,'+r');
                    end
                end
            end
            hold off;
        end
    end
    
end