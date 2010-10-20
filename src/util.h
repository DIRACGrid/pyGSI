
/*
 * util.h
 *
 * Copyright (C) AB Strakt 2001, All rights reserved
 *
 * Export utility functions and macros.
 * See the file RATIONALE for a short explanation of why this module was written.
 *
 * Reviewed 2001-07-23
 *
 * @(#) $Id: util.h,v 1.5 2008/07/08 10:54:55 acasajus Exp $
 */
#ifndef PyGSI_UTIL_H_
#define PyGSI_UTIL_H_

#include <Python.h>
#include <time.h>
#include <openssl/err.h>
#include <string.h>
#include <stdlib.h>


/*
 * pymemcompat written by Michael Hudson and lets you program to the
 * Python 2.3 memory API while keeping backwards compatability.
 */
#include "pymemcompat.h"

extern PyObject *error_queue_to_list( void );
extern void flush_error_queue( void );

#define OBJ_BEGIN_THREADS( obj ) if( !obj -> tstate ) obj->tstate = PyEval_SaveThread()
#define OBJ_END_THREADS( obj ) if ( obj-> tstate ) { PyEval_RestoreThread( obj-> tstate ); obj->tstate = NULL;  }

#ifdef GSI_HANDSHAKE_DEBUG
#define MIN_LOG_LEVEL 0

static void
logMsg( int level, char *fmt, ... )
{
    char *mesg;
    va_list ap;

    va_start( ap, fmt );
    vasprintf( &mesg, fmt, ap );
    va_end( ap );

    if ( level >= MIN_LOG_LEVEL )
        printf( "[%d] %s\n", level, mesg );

    free( mesg );
}
#else
#define logMsg(...)
#endif


#if !defined(PY_MAJOR_VERSION) || PY_VERSION_HEX < 0x02000000
static int
PyModule_AddObject( PyObject * m, char *name, PyObject * o )
{
    PyObject *dict;

    if ( !PyModule_Check( m ) || o == NULL )
        return -1;
    dict = PyModule_GetDict( m );
    if ( dict == NULL )
        return -1;
    if ( PyDict_SetItemString( dict, name, o ) )
        return -1;
    Py_DECREF( o );
    return 0;
}

static int
PyModule_AddIntConstant( PyObject * m, char *name, long value )
{
    return PyModule_AddObject( m, name, PyInt_FromLong( value ) );
}

static int
PyObject_AsFileDescriptor( PyObject * o )
{
    int fd;
    PyObject *meth;

    if ( PyInt_Check( o ) )
    {
        fd = PyInt_AsLong( o );
    }
    else if ( PyLong_Check( o ) )
    {
        fd = PyLong_AsLong( o );
    }
    else if ( ( meth = PyObject_GetAttrString( o, "fileno" ) ) != NULL )
    {
        PyObject *fno = PyEval_CallObject( meth, NULL );

        Py_DECREF( meth );
        if ( fno == NULL )
            return -1;

        if ( PyInt_Check( fno ) )
        {
            fd = PyInt_AsLong( fno );
            Py_DECREF( fno );
        }
        else if ( PyLong_Check( fno ) )
        {
            fd = PyLong_AsLong( fno );
            Py_DECREF( fno );
        }
        else
        {
            PyErr_SetString( PyExc_TypeError,
                             "fileno() returned a non-integer" );
            Py_DECREF( fno );
            return -1;
        }
    }
    else
    {
        PyErr_SetString( PyExc_TypeError,
                         "argument must be an int, or have a fileno() method." );
        return -1;
    }

    if ( fd < 0 )
    {
        PyErr_Format( PyExc_ValueError,
                      "file descriptor cannot be a negative integer (%i)",
                      fd );
        return -1;
    }
    return fd;
}
#endif

#endif
