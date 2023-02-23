CC := g++
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:%.cpp=%.o)

all: ${OBJS} perlin_ppm_maker

perlin_ppm_maker: ${OBJS}
	@echo "Checking.."
	${CC} $^ -o $@

%.o: %.c
	@echo "Creating object.."
	${CC} -c $<

clean:
	@echo "Cleaning up..."
	rm -rvf *.o *.ppm perlin_ppm_maker