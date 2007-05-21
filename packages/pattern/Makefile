#---------------------------------------------------------------------------
# 
#   $Id: Makefile 1213 2006-09-28 16:53:40Z nscott $
# 
#   (c) Copyright 2001. Space Systems Laboratory.
#       University of Maryland, College Park, MD 20742
# 
#------------------------ < Revision Information > -------------------------

# 20-Jul-2006	J Lisee		Created
#
#---------------------------------------------------------------------------


VCPROJ_DIR = ../../..

include $(VCPROJ_DIR)/libs/buildfiles/$(SSL_ARCH)/$(SSL_OS)/Makefile
include $(RTSX_VOB_DIR)/buildfiles/Makefiles/Makefile.first

CFLAGS = -g

INCLUDES	+=	-I. \
				-I$(VCPROJ_DIR) \
				-I$(RTSX_VOB_DIR) \
                -I$(TOOLS__CXXTEST_DIR)/.. \
                -I$(TOOLS__BOOST_INCLUDE_DIR)  

SOURCES_cpp			= 	$(SRC_DIR)/subject.cpp \
                        $(SRC_DIR)/subjectmanager.cpp 

OBJECTS         	= $(SOURCES_cpp:$(SRC_DIR)/%.cpp=$(BIN_OBJ_DIR)/%.o) 
LIBS_TO_BUILD		= $(RTSX_FOUNDATION_PATTERN_LIB) 

$(RTSX_FOUNDATION_PATTERN_LIB) : $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^


#---------------------------------------------------------------------------
# Tests
#---------------------------------------------------------------------------

TESTCXXSOURCES_cxx	=	$(TST_SRC_DIR)/testsubject.cxx \
                        $(TST_SRC_DIR)/testsubjectmanager.cxx \
						$(TST_SRC_DIR)/testsingleton.cxx

$(TST_OBJ_DIR)/testsubject:	$(TST_OBJ_DIR)/testsubject.o $(RTSX_FOUNDATION_PATTERN_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^  $(RTSX_FOUNDATION_PATTERN_LIB)

$(TST_OBJ_DIR)/testsubjectmanager:	$(TST_OBJ_DIR)/testsubjectmanager.o $(RTSX_FOUNDATION_PATTERN_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(RTSX_FOUNDATION_PATTERN_LIB)

$(TST_OBJ_DIR)/testsingleton:	$(TST_OBJ_DIR)/testsingleton.o $(RTSX_FOUNDATION_PATTERN_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(RTSX_FOUNDATION_PATTERN_LIB)

include $(RTSX_MAKEFILES_DIR)/Makefile.last
