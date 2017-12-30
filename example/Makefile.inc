BUILD_DIR := bin
SRC_DIR := .
CXXFLAGS := -MMD -Werror -O3 -Wall
CPP_FILES := $(wildcard ${SRC_DIR}/*.cpp)
OBJ_FILES := $(addprefix ${BUILD_DIR}/, $(notdir ${CPP_FILES:.cpp=.o}))
OBJ_FILES := ${OBJ_FILES:.cc=.o}

all: mkdir link

mkdir:
	mkdir -p ${BUILD_DIR}

link: ${OBJ_FILES}
	${CXX} -o ${BUILD_DIR}/${NAME} $^

clean:
	rm -rf -- ${BUILD_DIR}

${BUILD_DIR}/%.o: ${SRC_DIR}/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include ${BUILD_DIR}/*.d

.PHONY: clean all mkdir link
