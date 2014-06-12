# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# These methods are ported from logging.config module in the python standard
# library and Copyright 2001-2005 by Vinay Sajip. All Rights Reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  logloader.py

"""
This takes a section from a yaml file and loads loggers and formatter froms it.
This is basically the logging.config format from python standard library made
sane.  All the same caveats to all the arguments apply.  The one change is
qualname is now name.  Here is an example::

  Logging:
      Loggers:
          Root:
              level: NOTSET
              handlers: Console
          Parse:
              level: DEBUG
              handlers: Console
              propagate: 1
              name: compiler.parser

      Handlers:
          Console:
              class: StreamHandler
              level: NOTSET
              formatter: Form1
              args: (sys.stdout,)
          File:
              class: FileHandler
              level: DEBUG
              formatter: Form2
              args: ('python.log', 'w')
            
      Formatters:
          Form1:
              format: F1 %(asctime)s %(levelname)s %(message)s
              class: logging.Formatter

"""

import logging, logging.handlers, string
    
class LogConfigError(Exception):
    pass
    
def load_loggers(config):
    """
    Read the logging configuration from a yaml config object

    This can be called several times from an application, allowing an end user
    the ability to select from various pre-canned configurations (if the
    developer provides a mechanism to present the choices and load the chosen
    configuration).
    """
    import yaml

    formatters = _create_formatters(config)

    # critical section
    logging._acquireLock()
    try:
        logging._handlers.clear()
        del logging._handlerList[:]
        # Handlers add themselves to logging._handlers
        handlers = _install_handlers(config, formatters)
        _install_loggers(config, handlers)
    finally:
        logging._releaseLock()

def setup_logger(logger_cfg, config):
    """
    This reads the same logging format as above but with a twist.  Handlers
    and formatters will be pulled out of the config argument, but the 
    information about the logger you wished to config wish to create will come
    out of the logger_cfg.
    """
    
    formatters = _create_formatters(config)
    handlers = _install_handlers(config, formatters)
    return _create_logger(config, handlers, "UNKNOWN")

def resolve(name):
    """Resolve a dotted name to a global object."""
    name = string.split(name, '.')
    used = name.pop(0)
    found = __import__(used)
    for n in name:
        used = used + '.' + n
        try:
            found = getattr(found, n)
        except AttributeError:
            __import__(used)
            found = getattr(found, n)
    return found


def _create_formatters(config):
    """Create and return formatters"""
    if "Formatters" not in config:
        return {}
    formatters = {}
    
    for form in config["Formatters"]:
        opts = config["Formatters"][form]
        if "format" in opts:
            fs = opts["format"]
        else:
            fs = None
        if "datefmt" in opts:
            dfs = opts["datefmt"]
        else:
            dfs = None
        c = logging.Formatter
        if "class" in opts:
            class_name = opts["class"]
            if class_name:
                c = resolve(class_name)
        f = c(fs, dfs)
        formatters[form] = f
        
    return formatters


def _install_handlers(config, formatters):
    """Install and return handlers"""
    if "Handlers" not in config:
        return {}
    handlers = {}
    fixups = [] #for inter-handler references
    for hand in config["Handlers"]:
        opts = config["Handlers"][hand]
        
        if "class" not in opts:
            raise LogConfigError("'%s' Does not have a class specified" % hand)
        
        if "formatter" in opts:
            fmt = opts["formatter"]
        else:
            fmt = ""
            
        klass = opts["class"]
        klass = eval(klass, vars(logging))
        args = opts["args"]
        args = eval(args, vars(logging))
        h = apply(klass, args)
        
        if "level" in opts:
            level = opts["level"]
            h.setLevel(logging._levelNames[level])
        if len(fmt):
            h.setFormatter(formatters[fmt])
        #temporary hack for FileHandler and MemoryHandler.
        if klass == logging.handlers.MemoryHandler:
            if "target" in opts:
                target = opts["target"]
            else:
                target = ""
            if len(target): #the target handler may not be loaded yet, so keep for later...
                fixups.append((h, target))
        handlers[hand] = h
    #now all handlers are loaded, fixup inter-handler references...
    for h, t in fixups:
        h.setTarget(handlers[t])
    return handlers


def _create_logger(opts, handlers, name):
    """Create a logger""" 
    if "name" not in opts:
        raise LogConfigError("Logger '%s' does not have a name" % name)
    qn = opts["name"]
 
    if "propagate" in opts:
        propagate = opts["propagate"]
    else:
        propagate = 1
        
    existing = logging.root.manager.loggerDict.keys()
    logger = logging.getLogger(qn)
    if qn in existing:
        existing.remove(qn)
        
    if "level" in opts:
        level = opts["level"]
        logger.setLevel(logging._levelNames[level])
        
    for h in logger.handlers[:]:
        logger.removeHandler(h)
    logger.propagate = propagate
    logger.disabled = 0
    
    if "handlers" in opts:
        for hand in opts["handlers"]:
            logger.addHandler(handlers[hand])
            
    return logger
    

def _install_loggers(config, handlers):
    """Create and install loggers"""

    # configure the root first
    loggers = config["Loggers"]
    if "Root" not in loggers:
        raise LogConfigError("You must have a config section for the 'Root'"
                             " logger")
    
    root = logging.root
    log = root
    opts = loggers["Root"]
    
    if "level" in opts:
        level = opts["level"]
        log.setLevel(logging._levelNames[level])
    for h in root.handlers[:]:
        root.removeHandler(h)
        
    if "handlers" in opts:
        for hand in opts["handlers"]:
            log.addHandler(handlers[hand])

    # We Don't want to reload root below
    del loggers["Root"]

    #and now the others...
    #we don't want to lose the existing loggers,
    #since other threads may have pointers to them.
    #existing is set to contain all existing loggers,
    #and as we go through the new configuration we
    #remove any which are configured. At the end,
    #what's left in existing is the set of loggers
    #which were in the previous configuration but
    #which are not in the new configuration.
    existing = root.manager.loggerDict.keys()
    #now set up the new ones...
    for log in loggers:  
        opts = loggers[log]
        _create_logger(opts, handlers, log)
        
    #Disable any old loggers. There's no point deleting
    #them as other threads may continue to hold references
    #and by disabling them, you stop them doing any logging.
    for log in existing:
        root.manager.loggerDict[log].disabled = 1
