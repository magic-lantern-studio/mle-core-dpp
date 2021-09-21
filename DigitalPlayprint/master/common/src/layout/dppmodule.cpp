#define PY_SSIZE_T_CLEAN
#if defined(__linux__)
#include <Python.h>
#else
#include <Python/Python.h>
#endif

static PyObject *
dpp_system(PyObject *self, PyObject *args)
{
    const char *command;
    int sts;

    if (! PyArg_ParseTuple(args, "s", &command))
        return NULL;
    sts = system(command);
    return PyLong_FromLong(sts);
}
