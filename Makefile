# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: almounib <almounib@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/17 17:06:05 by almounib          #+#    #+#              #
#    Updated: 2025/01/21 10:00:00 by almounib         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

include main/build.mk

LIB_NAME	= sockell
LIB			= lib$(LIB_NAME).a


SRC_DIR		= src
INC_DIR		= includes
BUILD_DIR	= builds
OBJ_DIR		= $(BUILD_DIR)/obj
DEP_DIR		= $(BUILD_DIR)/dep
LIB_DIR		= $(BUILD_DIR)/lib

CC			= c++
CFLAGS		= -Wall -Wextra -Werror -std=c++98 -g3
IFLAGS		= -I$(INC_DIR)
DEPFLAGS	= -MMD -MP -MF $(DEP_DIR)/$*.d
AR			= ar rcs
RM			= rm -rf

SRC			=	Sockell.cpp
SRC			+=	SkllSignals.cpp
SRC			+=	SkllErrors.cpp
SRC			+=	SkllHook.cpp
SRC			+=	SkllEvent.cpp
SRC			+=	SkllMessage.cpp
SRC			+=	SkllRouter.cpp
SRC			+=	SkllProtocol.cpp
SRC			+=	SkllNetwork.cpp
SRC			+=	SkllClient.cpp
SRC			+=	SkllChannel.cpp
SRC			+=	SkllServer.cpp \
				SkllServer/SkllServerGetters.cpp \
				SkllServer/SkllServerPrivate.cpp \
				SkllServer/SkllServerSetters.cpp

SRCS		= $(addprefix $(SRC_DIR)/, $(SRC))
OBJS		= $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DEPS		= $(patsubst $(SRC_DIR)/%.cpp, $(DEP_DIR)/%.d, $(SRCS))

ORANGE		= \033[38;5;208m
GOLD		= \033[38;5;220m
CORAL		= \033[38;5;203m
SUNSET		= \033[38;5;209m
FIRE		= \033[38;5;196m
ICE			= \033[38;5;51m
CYAN		= \033[38;5;87m
BLUE		= \033[38;5;75m
PURPLE		= \033[38;5;141m
GREEN		= \033[38;5;77m
LIME		= \033[38;5;118m
YELLOW		= \033[38;5;227m
RED			= \033[38;5;196m
GRAY		= \033[38;5;243m
BOLD		= \033[1m
DIM			= \033[2m
RESET		= \033[0m

all: lib main

lib: $(LIB_DIR)/$(LIB)

$(LIB_DIR)/$(LIB): $(OBJS)
	@mkdir -p $(LIB_DIR)
	@echo "$(SUNSET)$(BOLD)╔══════════════════════════════════════╗$(RESET)"
	@echo "$(SUNSET)$(BOLD)║$(RESET)  $(GOLD)Creating library $(ORANGE)$(LIB)$(RESET)"
	@echo "$(SUNSET)$(BOLD)╚══════════════════════════════════════╝$(RESET)"
	@$(AR) $@ $(OBJS)
	@echo "$(GREEN)$(BOLD)✓$(RESET) $(LIME)Library compiled successfully$(RESET)\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEP_DIR)/$*)
	@echo "$(CYAN)$(BOLD)→$(RESET) $(BLUE)Compiling $(DIM)$<$(RESET)"
	@$(CC) $(CFLAGS) $(IFLAGS) $(DEPFLAGS) -c $< -o $@


clean:
	@$(RM) $(BUILD_DIR)
	@$(RM) $(MAIN_BUILD_DIR)
	@echo "$(RED)$(BOLD)✗$(RESET) $(CORAL)Cleaned: $(GRAY)builds/$(RESET)"

fclean: clean
	@$(RM) $(MAIN_NAME)
	@echo "$(RED)$(BOLD)✗$(RESET) $(CORAL)Cleaned: $(GRAY)$(MAIN_NAME)$(RESET)"

re: fclean all

.PHONY: all lib main clean fclean re run clear

-include $(DEPS)
-include $(MAIN_DEPS)
