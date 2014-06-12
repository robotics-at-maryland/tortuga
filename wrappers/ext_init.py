
def import_all_modules():
    module_names = ['core', 'math', 'vision', 'vehicle',
                    'vehicle.device', 'control', 'logging', 'network']

    # Attempt to import all the modules listed
    for import_name in ['ext.%s' % short_name for short_name in module_names]:
        try:
            __import__(import_name)
        except ImportError:
            # Could not import module
            pass
