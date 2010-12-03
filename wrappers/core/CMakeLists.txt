
set(WRAPPED_HEADERS
  ConfigNode.h
  EventConnection.h
  EventPublisher.h
  Event.h
  Application.h
  QueuedEventPublisher.h
  Events.h
  )
gccxml( core "${WRAPPED_HEADERS}" )
generate_wrappers( core )
python_files( core )

test_wrapper( core "ram_core" )
