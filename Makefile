SRCS			= main.cpp Server.cpp Client.cpp Message.cpp Utils.cpp Commands.cpp ChatBot.cpp Channel.cpp
OBJS			= $(SRCS:.cpp=.o)

CXX				= c++
RM				= rm -f
CXXFLAGS		= -I.  -g -fsanitize=address -Wall -Wextra -Werror

NAME			= ircserv
all:	$(NAME)

$(NAME):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS) -lcurl

clean:
	$(RM) $(OBJS) 


fclean:			clean
	$(RM) $(NAME)
re:				fclean $(NAME)
