# Makefile for Manual building

include ../../config.make

TEX= manual.tex commands.tex Reference1.tex Reference2.tex \
     Appendix-A.tex Appendix-B.tex Appendix-C.tex Appendix-D.tex

all: 	dvi ps

dvi: 	manual.dvi
ps: 	manual.ps

install: ps
	$(CP) manual.ps $(DOC_DIR)

manual.dvi: bibli $(TEX) 
	echo -n > index.tex
	latex manual.tex
	tr A-Z a-z < manual.idx> manual2.idx
	mv manual2.idx manual.idx
	STK_LIBRARY=../../Lib ../../Src/stk -no -file index.stk
	latex manual.tex

manual.ps: manual.dvi
	dvips -t a4 -f manual.dvi > manual.ps

bibli: manual.bbl

manual.bbl: $(TEX) ../bibliography.bib
	echo -n > index.tex
	latex manual.tex
	bibtex manual
	latex manual.tex

clean: 
	rm -f  *.bbl *.aux *.idx  *.log *.aux  *.blg  *.idx *.toc *~ index.tex

very-clean: clean
	rm -f manual.ps manual.dvi

