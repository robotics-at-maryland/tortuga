# STD Imports
import math as pmath

# Project Imports
import ext.core as core
import ext.vision as vision
import ext.math as math

#import ram.motion.common
import ram.motion.pipe # For the maneuvering motions

def ensureItemTracking(qeventHub, ai, name, foundEvent, lostEvent):
    if not ai.data.has_key(name):
        ai.data[name] = {}
        
    data = ai.data[name]
    
    if not data.has_key('trackingEnabled'):
        data['currentIds'] = set()
        data['itemData'] = {}

        def itemFound(event):
            """
            Ensures found event is stored, along with its latest data
            """ 
            id = event.id
            data['currentIds'].add(id)
            data['itemData'][id] = event
            
        def itemLost(event):
            """
            Remove dropped bins from records
            """

            id = event.id
            # Remove from the set of current bins
            data['currentIds'].remove(id)
    
            # Remove from our data list
            itemData = data['itemData']
            del itemData[id]
            data['itemData'] = itemData
            
        connA = qeventHub.subscribeToType(foundEvent, itemFound)
        connB = qeventHub.subscribeToType(lostEvent, itemLost)
    
        ai.addConnection(connA)
        ai.addConnection(connB)
        
        data['trackingEnabled'] = True
