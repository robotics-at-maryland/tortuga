
set(WRAPPED_HEADERS
  Events.h
  VisionSystem.h
  )

gccxml( vision "${WRAPPED_HEADERS}" )
generate_wrappers( vision )
python_files( vision )

test_wrapper( vision "ram_vision" )
