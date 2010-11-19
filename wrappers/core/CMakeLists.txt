
set(HEADERS
  ConfigNode.h
  EventConnection.h
  EventPublisher.h
  Event.h
  Application.h
  QueuedEventPublisher.h
  Events.h
  )
generate_wrappers( core "${HEADERS}" )
