
set(WRAPPED_HEADERS
  EventPlayer.h
  )

gccxml( logging "${WRAPPED_HEADERS}" )
generate_wrappers( logging )
python_files( logging )

test_wrapper( logging "ram_logging" )