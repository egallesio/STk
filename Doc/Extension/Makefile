#
# Makefile for Doc/Extension
#

SRC = Extending.tex

all: dvi ps

dvi: 	Extending.dvi
ps:	Extending.ps

Extending.dvi: $(SRC)
	echo -n > Extending.ind
	latex Extending; makeindex Extending; bibtex Extending;
	latex Extending; latex Extending

Extending.ps: Extending.dvi
	dvips -f Extending > Extending.ps

clean:
	rm -f *.aux *.idx *.log *~ *.so *.bbl *.blg *.ilg *.ind *.toc

very-clean: clean
	rm -f *.dvi *.ps
