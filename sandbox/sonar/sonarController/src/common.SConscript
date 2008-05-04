Import('env')

src = Split("""
	dft.c
	SampleDelay.cpp
	SlidingDFT.cpp
	Sonar.cpp
	SonarChunk.cpp
	TDOA.cpp
	SonarService.cpp
	""")

lib = env.Library('ram_sonar', src)
Return('lib')
