
set(WRAPPED_HEADERS
  ConfigNode.h
  EventConnection.h
  EventPublisher.h
  Event.h
  Application.h
  QueuedEventPublisher.h
  Events.h
  )
generate_wrappers( core "${WRAPPED_HEADERS}" )
