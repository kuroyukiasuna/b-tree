CXX = g++
SRCDIR = src
CXXFLAGS = -std=c++17 -O3 -Wall -MMD -I./${SRCDIR}
EXEC = bt_main
MAIN_TARGET = main.o
TREE_TARGETS = btree_node.o btree.o
SN_TARGETS = data_node.o
LOCK_TARGETS = rw_lock.o
OBJECTS = ${TREE_TARGETS:%=${SRCDIR}/tree/%} \
		  ${SN_TARGETS:%=${SRCDIR}/sn/%} \
		  ${LOCK_TARGETS:%=${SRCDIR}/lock/%} \
		  ${MAIN_TARGET:%=${SRCDIR}/%} \

DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
		${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}
#		${CXX} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

clean:
		rm ${OBJECTS} ${EXEC} ${DEPENDS}

.PHONY: clean