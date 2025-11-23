# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    build.mk                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: almounib <almounib@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/22 00:00:00 by almounib          #+#    #+#              #
#    Updated: 2025/01/22 00:10:00 by almounib         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

MAIN_NAME		= ircserv

MAIN_DIR		= main
MAIN_SRC_DIR	= $(MAIN_DIR)/src
MAIN_INC_DIR	= $(MAIN_DIR)/includes
MAIN_BUILD_DIR	= $(MAIN_DIR)/builds
MAIN_OBJ_DIR	= $(MAIN_BUILD_DIR)/obj
MAIN_DEP_DIR	= $(MAIN_BUILD_DIR)/dep
MAIN_LIB_DIR	= $(MAIN_BUILD_DIR)/lib

MAIN_CC			= c++
MAIN_CFLAGS		= -Wall -Wextra -Werror -std=c++98 -g3
MAIN_IFLAGS		= -I$(INC_DIR) -I$(MAIN_INC_DIR)
MAIN_LFLAGS		= -L$(MAIN_LIB_DIR) -l$(LIB_NAME)
MAIN_DEPFLAGS	= -MMD -MP

MAIN_SRC		=	IRC.cpp
MAIN_SRC		+=	IRCClient.cpp
MAIN_SRC		+=	IRCChannel.cpp
MAIN_SRC		+=	IRCServer.cpp
MAIN_SRC		+=	IRCCommands.cpp
MAIN_SRC		+=	IRCErrors.cpp

MAIN_FILE		=	main.cpp

MAIN_SRC_FILES	= $(addprefix $(MAIN_SRC_DIR)/, $(MAIN_SRC))
MAIN_MAIN_FILE	= $(addprefix $(MAIN_DIR)/, $(MAIN_FILE))

MAIN_SRC_OBJS	= $(patsubst $(MAIN_SRC_DIR)/%.cpp, $(MAIN_OBJ_DIR)/%.o, $(MAIN_SRC_FILES))
MAIN_MAIN_OBJ	= $(patsubst $(MAIN_DIR)/%.cpp, $(MAIN_OBJ_DIR)/%.o, $(MAIN_MAIN_FILE))

MAIN_OBJS		= $(MAIN_SRC_OBJS) $(MAIN_MAIN_OBJ)

MAIN_SRC_DEPS	= $(patsubst $(MAIN_SRC_DIR)/%.cpp, $(MAIN_DEP_DIR)/%.d, $(MAIN_SRC_FILES))
MAIN_MAIN_DEP	= $(patsubst $(MAIN_DIR)/%.cpp, $(MAIN_DEP_DIR)/%.d, $(MAIN_MAIN_FILE))

MAIN_DEPS		= $(MAIN_SRC_DEPS) $(MAIN_MAIN_DEP)

main: lib $(MAIN_NAME)

$(MAIN_NAME): $(MAIN_LIB_DIR)/$(LIB) $(MAIN_OBJS)
	@echo "$(SUNSET)$(BOLD)╔══════════════════════════════════════╗$(RESET)"
	@echo "$(SUNSET)$(BOLD)║$(RESET)  $(GOLD)Linking $(ORANGE)$(MAIN_NAME)$(RESET)"
	@echo "$(SUNSET)$(BOLD)╚══════════════════════════════════════╝$(RESET)"
	@$(MAIN_CC) $(MAIN_CFLAGS) $(MAIN_OBJS) $(MAIN_LFLAGS) -o $@
	@echo "$(GREEN)$(BOLD)✓$(RESET) $(LIME)IRC Server compiled successfully$(RESET)\n"

$(MAIN_LIB_DIR)/$(LIB): $(LIB_DIR)/$(LIB)
	@mkdir -p $(MAIN_LIB_DIR)
	@echo "$(ICE)$(BOLD)→$(RESET) $(CYAN)Copying library to main/builds$(RESET)"
	@cp $(LIB_DIR)/$(LIB) $(MAIN_LIB_DIR)/$(LIB)

$(MAIN_OBJ_DIR)/%.o: $(MAIN_SRC_DIR)/%.cpp
	@mkdir -p $(MAIN_OBJ_DIR)
	@mkdir -p $(MAIN_DEP_DIR)
	@echo "$(PURPLE)$(BOLD)→$(RESET) $(ICE)Compiling $(DIM)$<$(RESET)"
	@$(MAIN_CC) $(MAIN_CFLAGS) $(MAIN_IFLAGS) $(MAIN_DEPFLAGS) \
		-MF $(MAIN_DEP_DIR)/$*.d -c $< -o $@

$(MAIN_OBJ_DIR)/%.o: $(MAIN_DIR)/%.cpp
	@mkdir -p $(MAIN_OBJ_DIR)
	@mkdir -p $(MAIN_DEP_DIR)
	@echo "$(PURPLE)$(BOLD)→$(RESET) $(ICE)Compiling $(DIM)$<$(RESET)"
	@$(MAIN_CC) $(MAIN_CFLAGS) $(MAIN_IFLAGS) $(MAIN_DEPFLAGS) \
		-MF $(MAIN_DEP_DIR)/$*.d -c $< -o $@

.PHONY: main