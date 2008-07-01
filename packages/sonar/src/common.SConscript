Import('env')

src = Split("""
	fixed/atan2.c
	SampleDelay.cpp
	SlidingDFT.cpp
	Sonar.cpp
	SonarChunk.cpp
	TDOA.cpp
	""")

lib = env.Library('ram_sonar', src)
Return('lib')
