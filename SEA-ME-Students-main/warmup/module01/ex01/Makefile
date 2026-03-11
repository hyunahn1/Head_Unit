NAME = car

CC = c++

CFLAGS = -Wall -Werror -Wall

SRCS = Car.cpp SportsCar.cpp main.cpp Vehicle.cpp

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