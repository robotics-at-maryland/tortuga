#I think it is possible to create code that will do that automaticly.

def aliases( version ):
    if version == "1.2":
        return { }
        
    else: #1.2
        return {}


def header_files( version ):
    if version == "1.2":
        return [ 'OgreReferenceAppLayer.h'
            ]
    else: #1.2
        return [ 'OgreReferenceAppLayer.h'

        ]

def huge_classes( version ):
    if version == "1.2":
        return [  ]
    else: #1.2
        return [   ]
