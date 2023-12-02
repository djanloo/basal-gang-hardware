cdef extern from "simple_worker.cpp":
    pass

# Declare the class with cdef
cdef extern from "simple_worker.h" namespace "shapes":
    cdef cppclass Rectangle: 
        Rectangle() except +
        Rectangle(int, int, int, int) except +
        int x0, y0, x1, y1
        int getArea()
        void getSize(int* width, int* height)
        void move(int, int)

cpdef SubstantiaNigra(Q)
