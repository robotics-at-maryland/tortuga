import ext.pattern
import ext.core as core
import ext.control as control
import ext.vehicle as vehicle

cfg = core.ConfigNode.fromString(str({}))
veh = vehicle.Vehicle(cfg)

controller = control.BWPDController(veh, cfg)
