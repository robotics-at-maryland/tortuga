#!/usr/bin/env python
usage = """
Usage: search_leaks.py <directory> > log.txt

This simple script parses a source directory
of one of OpenCV source modules
and tries to find cvAlloc, cvCreateMat etc.
without corresponding cvFree, cvReleaseMat etc. 
Information about the suspicious places
is printed to standard output.
"""

import sys, os, re, glob

allocfuncs = [ ('cvAlloc', 'cvFree'),
               ('cvCreateMatND', 'cvReleaseMatND'),
               ('cvCreateMat', 'cvReleaseMat'),
               ('cvCreateImage', 'cvReleaseImage'),
               ('cvCreateHist', 'cvReleaseHist'),
               ('cvCreateSparseMat', 'cvReleaseSparseMat'),
               ('cvCreateMemStorage', 'cvReleaseMemStorage'),
               ('cvCreateChildMemStorage', 'cvReleaseMemStorage'),
               ('icvIPPFilterInit', 'cvFree'),
               ('icvFilterInitAlloc', 'icvFilterFree'),
               ('icvSobelInitAlloc', 'icvFilterFree'),
               ('icvScharrInitAlloc', 'icvFilterFree'),
               ('icvLaplaceInitAlloc', 'icvFilterFree'),
               ('icvSmoothInitAlloc', 'icvSmoothFree'),
               ('icvMorphInitAlloc', 'icvMorphFree'),]

if len(sys.argv) != 2:
    print usage

leaks = 0
falsealarms = 0

glob_expr = sys.argv[1] + '/*.c*'
for srcname in glob.glob(glob_expr):
    f = open(srcname,'r')
    lineno = 0
    allocdict = {}
    fname = ""
    for l in f.xreadlines():
        lineno += 1
        if len(l)>1 and l[0] not in " \t/":
            if l[0] == "}":
                # analyze the allocation map
                leaks0 = leaks
                for (name,p) in allocdict.items():
                    if p[1] > 0:
                        print "%s(%d): function %s: %s is not deallocated" % \
                            (srcname, lineno, fname, name)
                        leaks += 1
                if fname == "cvAlloc" or re.findall( "^i?cv(Create)|(Read)|(Clone)", fname ):
                    if leaks > leaks0:
                        print "%s(%d): function %s: some of leaks are probably false alarms" % \
                            (srcname, lineno, fname)
                        falsealarms += leaks - leaks0
                fname = ""
                allocdict = {}
                #print "%s(%d): clearing map..." % (srcname, lineno)
            else:
                m = re.findall( r'(\bi?cv\w+)\(', l )
                if m and m[0]:
                    fname = m[0]
                    allocdict = {}
                    #print "%s(%d): clearing map..." % (srcname, lineno)
        # that's a small speedup that might fail
        # if create/release naming convention is not used for some structure
        elif l.find( 'Alloc') >= 0 or l.find('Create') >= 0 or l.find('Init') >= 0:
            for i in allocfuncs:
                m = re.findall( r'\b' + i[0] + r'\s*\(', l )
                if m and m[0]:
                    m = re.findall( r'[\s\(\*]([\w\.\->\[\]]+)\s*=[^=]', l )
                    if not m or not m[0]:
                        print "%s(%d): function %s: no variable found in create function" % \
                            (srcname, lineno, fname)
                        break
                    #assert (m and m[0]) # if "create" is called,
                    #                    # there should be a target pointer
                    allocdict[m[0]] = (i[1],1)
                    break
        # the same notice as for cvAlloc & cvCreate
        elif l.find( 'Free') >= 0 or l.find('Release') >= 0:
            for i in allocfuncs:
                m = re.findall( r'\b' + i[1] + r'\s*\(', l )
                if m and m[0]:
                    m = re.findall( r'([\w\.\->\[\]]+)[\s\)]+;', l )
                    #assert (m and m[0]) # if "release" is called,
                    #                    # there should be a released pointer (last argument)
                    if not m or not m[0]:
                        print "%s(%d): function %s: no variable found in release function" % \
                            (srcname, lineno, fname)
                        break
                    p = allocdict.get(m[0],("",0))
                    if not p[0]:
                        print "%s(%d): function %s: unmatching release of %s" % \
                            (srcname, lineno, fname, m[0])
                        
                    #assert (p[0] == i[1]) # "release" function should match
                    #                      # the "create" function used
                    allocdict[m[0]] = (p[0],p[1]-1)
                    break

    f.close()

print "Result: %d potential leaks found, %d out of them are possible false alarms" % (leaks, falsealarms)
