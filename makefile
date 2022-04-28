OBJS =  p1-odProgram.c searchHashCode.c

all: fifos programa search
	./mkMyFIFO
	./searchHashCode &
	./p1-odProgram

fifos:
	gcc mkMyFIFO.c -o mkMyFIFO
	gcc rmMyFIFO.c -o rmMyFIFO

programa: 
	gcc p1-odProgram.c  -o p1-odProgram
	
search:
	gcc searchHashCode.c -o searchHashCode
	
clean:
	./rmMyFIFO
	rm mkMyFIFO
	rm rmMyFIFO
	rm p1-odProgram
	rm searchHashCode
