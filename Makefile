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
LIB_NAME    = sockell

SRC_DIR     = srcs
MAIN_DIR    = main
INC_DIR     = includes
BUILD_DIR   = builds
OBJ_DIR     = $(BUILD_DIR)/obj
DEP_DIR     = $(BUILD_DIR)/dep
LIB_DIR     = $(BUILD_DIR)
LIB         = $(LIB_DIR)/lib$(LIB_NAME).a

CC          = c++
CFLAGS      = -Wall -Wextra -Werror -g3 -std=c++98 -I$(INC_DIR)
DEPFLAGS    = -MMD -MP
AR          = ar rcs

SRCS		= signals.cpp

SRCS        +=	SkllErrors/SkllErrors.cpp

SRCS        +=	SkllHook/SkllHook.cpp

SRCS        +=	SkllNetwork/SkllNetwork.cpp \
				SkllNetwork/SkllNetwork_private.cpp \

SRCS        +=	SkllProtocol/SkllProtocol.cpp

SRCS        +=	SkllConsole/SkllConsole.cpp \
				SkllConsole/SkllConsole_public.cpp \
				SkllConsole/SkllConsole_private.cpp

SRCS        +=	SkllClient/SkllClient.cpp \
				SkllClient/SkllClient_public.cpp \
				SkllClient/SkllClient_private.cpp

SRCS        +=	SkllChannel/SkllChannel.cpp \
				SkllChannel/SkllChannel_public.cpp \
				SkllChannel/SkllChannel_private.cpp

SRCS        +=	SkllServer/SkllServer.cpp \
				SkllServer/SkllServer_public.cpp \
				SkllServer/SkllServer_private.cpp


MAIN_SRCS   = main.cpp hook_server.cpp hook_channel.cpp

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
	@$(CC) $(CFLAGS) $(MAIN_OBJ) -L$(LIB_DIR) -l$(LIB_NAME) -o $(NAME)
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

s: all
	./$(NAME) 6667 pass

re: fclean all

.PHONY: all clean fclean re

-include $(DEPS)