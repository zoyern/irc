# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: marvin <marvin@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/17 17:06:05 by almounib          #+#    #+#              #
#    Updated: 2025/08/31 16:22:39 by marvin           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = ircserv
LIB_NAME    = libirc.a

SRC_DIR     = srcs
MAIN_DIR    = main
INC_DIR     = includes
BUILD_DIR   = builds
OBJ_DIR     = $(BUILD_DIR)/obj
DEP_DIR     = $(BUILD_DIR)/dep
LIB_DIR     = $(BUILD_DIR)
LIB         = $(LIB_DIR)/$(LIB_NAME)

CC          = c++
CFLAGS      = -Wall -Wextra -Werror -g3 -std=c++98 -I$(INC_DIR)
DEPFLAGS    = -MMD -MP
AR          = ar rcs

SRCS		= signals.cpp

SRCS        +=	Server/Server.cpp \
				Server/Server_public.cpp \
				Server/Server_private.cpp \
				Server/Epoll/Epoll.cpp

SRCS        +=	Client/Client.cpp \
				Client/Client_public.cpp \
				Client/Client_private.cpp

SRCS        +=	Channel/Channel.cpp \
				Channel/Channel_public.cpp \
				Channel/Channel_private.cpp

SRCS        +=	Console/Console.cpp \
				Console/Console_public.cpp \
				Console/Console_private.cpp

MAIN_SRCS   = main.cpp

SRCS_FULL   = $(addprefix $(SRC_DIR)/, $(SRCS))
MAIN_FULL   = $(addprefix $(MAIN_DIR)/, $(MAIN_SRCS))
OBJS        = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS_FULL))
MAIN_OBJ    = $(patsubst $(MAIN_DIR)/%.cpp, $(OBJ_DIR)/main/%.o, $(MAIN_FULL))
DEPS        = $(patsubst %.o, $(DEP_DIR)/%.d, $(OBJS) $(MAIN_OBJ))

GREEN       = \033[32m
YELLOW      = \033[33m
RED         = \033[31m
RESET       = \033[0m

all: $(NAME)

$(NAME): $(LIB) $(MAIN_OBJ)
	@echo "$(YELLOW)Linking $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(MAIN_OBJ) -L$(LIB_DIR) -lirc -o $(NAME)
	@echo "$(GREEN)✓ Compilation réussie !$(RESET)"

$(LIB): $(OBJS)
	@echo "$(YELLOW)Creating library $(LIB_NAME)...$(RESET)"
	@$(AR) $(LIB) $(OBJS)
	@echo "$(GREEN)✓ Library created$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*)
	@echo "$(YELLOW)Compiling $<$(RESET)"
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@
	@mv $(OBJ_DIR)/$*.d $(DEP_DIR)/$*.d 2>/dev/null || true

$(OBJ_DIR)/main/%.o: $(MAIN_DIR)/%.cpp
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/main)
	@echo "$(YELLOW)Compiling $<$(RESET)"
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@
	@mv $(OBJ_DIR)/main/$*.d $(DEP_DIR)/main/$*.d 2>/dev/null || true

clean:
	@rm -rf $(BUILD_DIR)
	@echo "$(RED)Clean   : ./$(BUILD_DIR) !$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)Clean   : ./$(NAME)$(RESET)"

re: fclean all

.PHONY: all clean fclean re

-include $(DEPS)