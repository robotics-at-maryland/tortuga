# Library Imports
from pyplusplus import module_builder
mod_builder = module_builder

# Project Imports
import buildfiles.wrap as wrap

def generate(module_builder, local_ns, global_ns):
    """
    local_ns: is the namespace that corresponds to the given namespace
    global_ns: is the module builder for the entire library
    """

    local_ns.exclude()
    classes = []

    # NetworkPublisher
    NetworkPublisher = local_ns.class_('NetworkPublisher')
    NetworkPublisher.include()
    classes.append(NetworkPublisher)

    # NetworkHub
    NetworkHub = local_ns.class_('NetworkHub')
    NetworkHub.include()
    classes.append(NetworkHub)

    events = wrap.expose_events(local_ns)

    if events:
        wrap.make_already_exposed(global_ns, 'ram::core', ['Event'])
        classes += events

    wrap.registerSubsystemConverter(NetworkPublisher)
    wrap.registerSubsystemConverter(NetworkHub)

    wrap.add_needed_includes(classes)

    include_files = set([cls.location.file_name for cls in classes])
    for cls in classes:
        include_files.update(cls.include_files)
    return list(include_files)
