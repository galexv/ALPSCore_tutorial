#!/usr/bin/make -f

.PHONY:	default

default: overview.pdf

overview.pdf: overview.tex
	pdflatex --file-line-error -halt-on-error overview.tex

overview.tex: overview.wiki.txt
	wiki2beamer overview.wiki.txt >overview.tex
