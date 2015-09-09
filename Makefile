LINK=-lGL -lGLU -lm -lglfw -lGLEW
ODIR=./obj
OUT=./bin/RedVSBlue
SRC=./src
INC=-Iinc

_OBJS = main.o CallFunctions.o GameInclude.o OpenglInclude.o

OBJS = $(patsubst %, $(ODIR)/%, $(_OBJS))


all: $(OBJS)
	gcc -o $(OUT) $(OBJS) $(LINK)  

$(ODIR):
	mkdir -p $(ODIR)


$(ODIR)/%.o: $(SRC)/%.c | $(ODIR) 
	gcc -Wall -c $< $(INC) -o $@

run:
	$(OUT)

clean:
	rm -f $(ODIR)/*.o $(OUT)

zip:
	zip -r Trab1-7153362 *