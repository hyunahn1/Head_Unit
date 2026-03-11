NAME = car

CC = c++

CFLAGS = -Wall -Werror -Wall

SRCS = Brake.cpp Engine.cpp Car.cpp Frame.cpp Lights.cpp main.cpp Part.cpp SteeringWheel.cpp Transmission.cpp Wheels.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all flcean clean re