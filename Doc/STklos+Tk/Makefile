# Makefile for STklos+Tk document building

TEX= STklos+Tk.tex Chap1.tex Chap2.tex

all: dvi ps

dvi: 	STklos+Tk.dvi
ps: 	STklos+Tk.ps

STklos+Tk.dvi: $(TEX)
	       latex STklos+Tk.tex
	       makeindex STklos+Tk
	       latex STklos+Tk.tex
	       latex STklos+Tk.tex
	
STklos+Tk.ps: STklos+Tk.dvi
	dvips -t a4 -f STklos+Tk.dvi > STklos+Tk.ps

clean: 
	@rm -f  *.aux *.idx  *.log *.aux *.bbl *.blg  *.idx *.toc *~ *.ilg *.ind 

very-clean: clean
	rm -f STklos+Tk.ps STklos+Tk.dvi
