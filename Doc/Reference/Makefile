# Makefile for Manual building

TEX= manual.tex commands.tex Reference1.tex Reference2.tex \
     Appendix-A.tex Appendix-B.tex Appendix-C.tex Appendix-D.tex

all: dvi ps

dvi: 	manual.dvi
ps: 	manual.ps

manual.dvi: $(TEX)
	echo -n > index.tex
	latex manual.tex
	tr A-Z a-z < manual.idx> manual2.idx
	mv manual2.idx manual.idx
	STK_LIBRARY=../../Lib ../../Src/stk -no -file index.stk
	bibtex manual
	latex manual.tex
	latex manual.tex
	
manual.ps: manual.dvi
	dvips -t a4 -f manual.dvi > manual.ps

clean: 
	rm -f  *.aux *.idx  *.log *.aux *.bbl *.blg  *.idx *.toc *~ index.tex

very-clean: clean
	rm -f manual.ps manual.dvi

