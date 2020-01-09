OBJECTS = featherweight.o net.o demo.o
LDFLAGS = -lpthread

default: demo

%.o: %.c
	gcc -c $< -o $@

demo: $(OBJECTS)
	gcc $(OBJECTS) $(LDFLAGS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f demo
