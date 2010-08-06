classdef Object < dynamicprops
%OBJECT Represents an object
%   
%   name - object name (char array)
%   measurements - containers.Map() storing all measurements
%   location - structure storing (x,y) +/- (sigx, sigy) information
%
%   location.xobj      - x coordinate of location
%   location.sigxobj   - error in location.x
%   location.yobj      - y coordinate of location
%   location.sigyobj   - error in location.y
%

   methods
       function obj = Object(name)
           obj.addprop('measurements');
           obj.addprop('removedMeasurements');
           obj.addprop('location');
           obj.addprop('name');
           obj.name = name;
           obj.location.xobj = Null();
           obj.location.sigxobj = Null();
           obj.location.yobj = Null();
           obj.location.sigyobj = Null();
           obj.measurements = containers.Map();
           obj.removedMeasurements = containers.Map();
       end
       
       function addMeasurement(obj,m)
           obj.measurements(m.name) = m;
       end
       
       function removeMeasurement(obj,m)
           obj.measurements.remove(m);
       end
       
       function m = getMeasurementByName(obj, name)
           m = obj.measurements(name);
       end
       
       function m = getMeasurementByIndex(obj, index)
           k = obj.measurements.keys();
           m = obj.measurements(char(k(index)));
       end
       
       function measurementList = getAllMeasurements(obj)
           measurementList = obj.measurements;
       end
       
       function updateLocation(obj)
          obj.location = CalculatePosition(obj.measurements); 
       end
       
       function o = clone(obj)
           o = Object(obj.name);
           o.measurements = cloneContainerMap(obj.measurements);
           o.removedMeasurements = cloneContainerMap(obj.removedMeasurements);
           o.location = obj.location;
       end
       
   end %methods
   
end  %classdef
