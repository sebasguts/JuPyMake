#include <Python.h>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <polymake/Main.h>

/*
 * Python different version stuff
 */

#if PY_MAJOR_VERSION >= 3
#define to_python_string(o) PyUnicode_FromString(o)
#else
#define to_python_string(o) PyString_FromString(const_cast<char*>(o))
#endif

polymake::Main* main_polymake_session;

/*
 * Python functions
 */
static PyObject * RunPolymakeCommand( PyObject* self, PyObject* args ){
    
    const char * input_string;
    if (! PyArg_ParseTuple(args, "s", &input_string) )
        return NULL;
    string polymake_input(input_string);
    const char* output_char;
    PyObject* success;
    polymake::perl::Scope current_scope = main_polymake_session->newScope();
    try{
      string output = main_polymake_session->simulate_shell_input(polymake_input);
      output_char = output.c_str();
      success = Py_True;
    }catch( std::exception& e ){
      output_char = e.what();
      success = Py_False;
    }
    return PyTuple_Pack( 2, success, to_python_string( output_char ) );
}

/*
 * Python mixed init stuff
 */

struct module_state {
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

static PyObject * error_out(PyObject *m) {
    struct module_state *st = GETSTATE(m);
    PyErr_SetString(st->error, "something bad happened");
    return NULL;
}

static PyMethodDef JuPyMakeMethods[] = {
    {"RunPolymakeCommand",(PyCFunction)RunPolymakeCommand, METH_VARARGS,
     "Runs a polymake command"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


#if PY_MAJOR_VERSION >= 3

static int JuPyMake_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int JuPyMake_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}


static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "JuPyMake",
        NULL,
        sizeof(struct module_state),
        JuPyMakeMethods,
        NULL,
        JuPyMake_traverse,
        JuPyMake_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC PyInit_JuPyMake(void)

#else
#define INITERROR return

extern "C" void initJuPyMake(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
#else
    PyObject *module = Py_InitModule("JuPyMake", JuPyMakeMethods);
#endif

    if (module == NULL)
        INITERROR;
    struct module_state *st = GETSTATE(module);
    main_polymake_session = new polymake::Main;
    main_polymake_session->set_application("polytope");


#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
