OBJECTS = featherweight.o net.o demo.o http.o route.o
CFLAGS  = -Wall -o3
LDFLAGS = -lpthread

default: demo

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

demo: $(OBJECTS)
	gcc $(OBJECTS) $(LDFLAGS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f demo
