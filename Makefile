CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -MMD
EXEC = bt_tree
OBJECTS = main.o btree_node.o btree.o data_node.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
		${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}
#		${CXX} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

clean:
		rm ${OBJECTS} ${EXEC} ${DEPENDS}

.PHONY: clean