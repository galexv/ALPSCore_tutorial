#!/usr/bin/make -f

.PHONY:	default diffs

default: overview.pdf diffs

overview.pdf: overview.tex
	pdflatex --file-line-error -halt-on-error overview.tex

overview.tex: overview.wiki.txt
	wiki2beamer overview.wiki.txt >overview.tex

diffs:	step-5-7-main-cpp.diff step-5-7-sim-cpp.diff step-5-7-sim-hpp.diff

step-5-7-main-cpp.diff: step5.pi/main.cpp step7.saveload/main.cpp 
	diff -u $+ >$@; :

step-5-7-sim-cpp.diff: step5.pi/simulation.cpp step7.saveload/simulation.cpp 
	diff -u $+ >$@; :

step-5-7-sim-hpp.diff: step5.pi/simulation.hpp step7.saveload/simulation.hpp 
	diff -u $+ >$@; :
