#include "rpmsystem-py.h"

#include <rpm/rpmtypes.h>
#include <rpm/rpmstring.h>
#include <rpm/rpmlib.h>		/* rpmvercmp */

#include "header-py.h"
#include "rpmds-py.h"
#include "rpmstrpool-py.h"

struct rpmdsObject_s {
    PyObject_HEAD
    int		active;
    rpmds	ds;
};

static PyObject *
rpmds_Count(rpmdsObject * s)
{
    DEPRECATED_METHOD("use len(ds) instead");
    return Py_BuildValue("i", PyMapping_Size((PyObject *)s));
}

static PyObject *
rpmds_Ix(rpmdsObject * s)
{
    return Py_BuildValue("i", rpmdsIx(s->ds));
}

static PyObject *
rpmds_DNEVR(rpmdsObject * s)
{
    return utf8FromString(rpmdsDNEVR(s->ds));
}

static PyObject *
rpmds_N(rpmdsObject * s)
{
    return utf8FromString(rpmdsN(s->ds));
}

static PyObject *
rpmds_EVR(rpmdsObject * s)
{
    return utf8FromString(rpmdsEVR(s->ds));
}

static PyObject *
rpmds_Flags(rpmdsObject * s)
{
    return Py_BuildValue("i", rpmdsFlags(s->ds));
}

static PyObject *
rpmds_TagN(rpmdsObject * s)
{
    return Py_BuildValue("i", rpmdsTagN(s->ds));
}

static PyObject *
rpmds_Color(rpmdsObject * s)
{
    return Py_BuildValue("i", rpmdsColor(s->ds));
}

static PyObject *
rpmds_IsWeak(rpmdsObject * s)
{
    return PyBool_FromLong(rpmdsIsWeak(s->ds));
}

static PyObject *
rpmds_IsRich(rpmdsObject * s)
{
    return PyBool_FromLong(rpmdsIsRich(s->ds));
}

static PyObject *
rpmds_IsReverse(rpmdsObject * s)
{
    return PyBool_FromLong(rpmdsIsReverse(s->ds));
}

static PyObject *
rpmds_IsSysuser(rpmdsObject * s)
{
    return PyBool_FromLong(rpmdsIsSysuser(s->ds, NULL));
}

static PyObject *
rpmds_iternext(rpmdsObject * s)
{
    PyObject * result = NULL;

    /* Reset loop indices on 1st entry. */
    if (!s->active) {
	s->ds = rpmdsInit(s->ds);
	s->active = 1;
    }

    /* If more to do, return a (N, EVR, Flags) tuple. */
    if (rpmdsNext(s->ds) >= 0) {
	result = rpmds_Wrap(Py_TYPE(s), rpmdsCurrent(s->ds));
    } else
	s->active = 0;

    return result;
}

/* XXX rpmdsFind uses bsearch on s->ds, so a sort is needed. */
static PyObject *
rpmds_Sort(rpmdsObject * s)
{
    /* XXX sort on (N,EVR,F) here. */
    Py_RETURN_NONE;
}

static PyObject *
rpmds_Find(rpmdsObject * s, PyObject * arg)
{
    rpmdsObject * o;
    rpmmodule_state_t *modstate = rpmModState_FromObject((PyObject*)s);
    if (!modstate) {
	return NULL;
    }

    if (!PyArg_Parse(arg, "O!:Find", modstate->rpmds_Type, &o))
	return NULL;

    /* XXX make sure ods index is valid, real fix in lib/rpmds.c. */
    if (rpmdsIx(o->ds) == -1)	rpmdsSetIx(o->ds, 0);

    return Py_BuildValue("i", rpmdsFind(s->ds, o->ds));
}

static PyObject *
rpmds_Merge(rpmdsObject * s, PyObject * arg)
{
    rpmdsObject * o;
    rpmmodule_state_t *modstate = rpmModState_FromObject((PyObject*)s);
    if (!modstate) {
	return NULL;
    }

    if (!PyArg_Parse(arg, "O!:Merge", modstate->rpmds_Type, &o))
	return NULL;

    return Py_BuildValue("i", rpmdsMerge(&s->ds, o->ds));
}
static PyObject *
rpmds_Search(rpmdsObject * s, PyObject * arg)
{
    rpmdsObject * o;
    rpmmodule_state_t *modstate = rpmModState_FromObject((PyObject*)s);
    if (!modstate) {
	return NULL;
    }

    if (!PyArg_Parse(arg, "O!:Merge", modstate->rpmds_Type, &o))
        return NULL;

    return Py_BuildValue("i", rpmdsSearch(s->ds, o->ds));
}

static PyObject *rpmds_Compare(rpmdsObject * s, PyObject * o)
{
    rpmdsObject * ods;
    rpmmodule_state_t *modstate = rpmModState_FromObject((PyObject*)s);
    if (!modstate) {
	return NULL;
    }

    if (!PyArg_Parse(o, "O!:Compare", modstate->rpmds_Type, &ods))
	return NULL;

    return PyBool_FromLong(rpmdsCompare(s->ds, ods->ds));
}

static PyObject *rpmds_Instance(rpmdsObject * s)
{
    return Py_BuildValue("i", rpmdsInstance(s->ds));
}

static PyObject * rpmds_Rpmlib(rpmdsObject * s, PyObject *args, PyObject *kwds)
{
    PyObject *pool_source = NULL;
    rpmstrPool pool = NULL;
    rpmds ds = NULL;
    char * kwlist[] = {"pool", NULL};
    rpmmodule_state_t *modstate = rpmModState_FromObject((PyObject*)s);
    if (!modstate) {
	return NULL;
    }

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O:rpmds_Rpmlib", kwlist,
		 &pool_source))
	return NULL;

    if(pool_source && !poolFromPyObject(modstate, pool_source, &pool)) {
	    return NULL;
    }

    /* XXX check return code, permit arg (NULL uses system default). */
    rpmdsRpmlibPool(pool, &ds, NULL);

    return rpmds_Wrap(modstate->rpmds_Type, ds);
}

static struct PyMethodDef rpmds_methods[] = {
 {"Count",	(PyCFunction)rpmds_Count,	METH_NOARGS,
	"Deprecated, use len(ds) instead.\n" },
 {"Ix",		(PyCFunction)rpmds_Ix,		METH_NOARGS,
	"ds.Ix -> Ix -- Return current element index.\n" },
 {"DNEVR",	(PyCFunction)rpmds_DNEVR,	METH_NOARGS,
	"ds.DNEVR -> DNEVR -- Return current DNEVR.\n" },
 {"N",		(PyCFunction)rpmds_N,		METH_NOARGS,
	"ds.N -> N -- Return current N.\n" },
 {"EVR",	(PyCFunction)rpmds_EVR,		METH_NOARGS,
	"ds.EVR -> EVR -- Return current EVR.\n" },
 {"Flags",	(PyCFunction)rpmds_Flags,	METH_NOARGS,
	"ds.Flags -> Flags -- Return current Flags.\n" },
 {"TagN",	(PyCFunction)rpmds_TagN,	METH_NOARGS,
  "ds.TagN -> TagN -- Return TagN (RPMTAG_*NAME)\n\n"
  "the type of all dependencies in this set.\n" },
 {"Color",	(PyCFunction)rpmds_Color,	METH_NOARGS,
	"ds.Color -> Color -- Return current Color.\n" },
 {"Sort",	(PyCFunction)rpmds_Sort,	METH_NOARGS,
	NULL},
 {"Find",	(PyCFunction)rpmds_Find,	METH_O,
  "ds.find(other_ds) -- Return index of other_ds in ds"},
 {"Merge",	(PyCFunction)rpmds_Merge,	METH_O,
	NULL},
 {"Search",     (PyCFunction)rpmds_Search,      METH_O,
"ds.Search(element) -> matching ds index (-1 on failure)\n\
Check that element dependency range overlaps some member of ds.\n\
The current index in ds is positioned at overlapping member." },
 {"Rpmlib",     (PyCFunction)rpmds_Rpmlib,      METH_VARARGS|METH_KEYWORDS|METH_STATIC,
	"ds.Rpmlib -> nds -- Return internal rpmlib dependency set.\n"},
 {"Compare",	(PyCFunction)rpmds_Compare,	METH_O,
  "ds.compare(other) -- Compare current entries of self and other.\n\nReturns True if the entries match each other, False otherwise"},
 {"Instance",	(PyCFunction)rpmds_Instance,	METH_NOARGS,
  "ds.Instance() -- Return rpmdb key of corresponding package or 0."},
 {"IsWeak",	(PyCFunction)rpmds_IsWeak,	METH_NOARGS,
  "ds.IsWeak() -- Return whether the dependency is weak."},
 {"IsRich",	(PyCFunction)rpmds_IsRich,	METH_NOARGS,
  "ds.IsRich() -- Return whether the dependency is rich."},
 {"IsReverse",	(PyCFunction)rpmds_IsReverse,	METH_NOARGS,
  "ds.IsReverse() -- Return whether the dependency is reversed."},
 {"IsSysuser",	(PyCFunction)rpmds_IsSysuser,	METH_NOARGS,
  "ds.IsSysuser() -- Return whether the dependency represents a sysusers.d entry."},
 {NULL,		NULL}		/* sentinel */
};

/* ---------- */

static void
rpmds_dealloc(rpmdsObject * s)
{
    PyObject_GC_UnTrack(s);
    s->ds = rpmdsFree(s->ds);
    PyTypeObject *type = Py_TYPE(s);
    freefunc free = PyType_GetSlot(type, Py_tp_free);
    free(s);
    Py_DECREF(type);
}

static int rpmds_traverse(hdrObject * s, visitproc visit, void *arg)
{
    if (python_version >= 0x03090000) {
        Py_VISIT(Py_TYPE(s));
    }
    return 0;
}

static Py_ssize_t rpmds_length(rpmdsObject * s)
{
    return rpmdsCount(s->ds);
}

static PyObject *
rpmds_subscript(rpmdsObject * s, PyObject * key)
{
    int ix;

    if (!PyLong_Check(key)) {
	PyErr_SetString(PyExc_TypeError, "integer expected");
	return NULL;
    }

    ix = (int) PyLong_AsLong(key);
    rpmdsSetIx(s->ds, ix);
    return utf8FromString(rpmdsDNEVR(s->ds));
}


static int rpmds_init(rpmdsObject * s, PyObject *args, PyObject *kwds)
{
    s->active = 0;
    return 0;
}

static int depflags(PyObject *o, rpmsenseFlags *senseFlags)
{
    int ok = 0;
    PyObject *str = NULL;
    rpmsenseFlags flags = RPMSENSE_ANY;

    if (PyLong_Check(o)) {
	ok = 1;
	flags = PyLong_AsLong(o);
    } else if (utf8FromPyObject(o, &str)) {
	ok = 1;
	for (const char *s = PyBytes_AsString(str); *s; s++) {
	    switch (*s) {
	    case '=':
		flags |= RPMSENSE_EQUAL;
		break;
	    case '<':
		flags |= RPMSENSE_LESS;
		break;
	    case '>':
		flags |= RPMSENSE_GREATER;
		break;
	    default:
		ok = 0;
		break;
	    }
	}
	Py_DECREF(str);
    }

    if (flags == (RPMSENSE_EQUAL|RPMSENSE_LESS|RPMSENSE_GREATER))
	ok = 0;

    if (ok)
	*senseFlags = flags;

    return ok;
}

static PyObject * rpmds_new(PyTypeObject * subtype, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    rpmTagVal tagN = RPMTAG_REQUIRENAME;
    rpmds ds = NULL;
    Header h = NULL;
    PyObject *pool_source = NULL;
    rpmstrPool pool = NULL;
    char * kwlist[] = {"obj", "tag", "pool", NULL};
    rpmmodule_state_t *modstate = rpmModState_FromType(subtype);
    if (!modstate) {
	return NULL;
    }

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO&|O:rpmds_new", kwlist,
	    	 &obj, tagNumFromPyObject, &tagN,
		 &pool_source))
	return NULL;

    if(pool_source && !poolFromPyObject(modstate, pool_source, &pool)) {
	return NULL;
    }

    if (PyTuple_Check(obj)) {
	const char *name = NULL;
	const char *evr = NULL;
	rpmsenseFlags flags = RPMSENSE_ANY;
	/* TODO: if flags are specified, evr should be required too */
	if (PyArg_ParseTuple(obj, "s|O&s", &name, depflags, &flags, &evr)) {
	    ds = rpmdsSinglePool(pool, tagN, name, evr, flags);
	} else {
	    PyErr_SetString(PyExc_ValueError, "invalid dependency tuple");
	    return NULL;
	}
    } else if (hdrFromPyObject(obj, &h)) {
	if (tagN == RPMTAG_NEVR) {
	    ds = rpmdsThisPool(pool, h, RPMTAG_PROVIDENAME, RPMSENSE_EQUAL);
	} else {
	    ds = rpmdsNewPool(pool, h, tagN, 0);
	}
    } else {
	PyErr_SetString(PyExc_TypeError, "header or tuple expected");
	return NULL;
    }
    
    return rpmds_Wrap(subtype, ds);
}

static char rpmds_doc[] =
    "rpm.ds (dependendcy set) gives a more convenient access to dependencies\n\n"
    "It can hold multiple entries of Name Flags and EVR.\n"
    "It typically represents all dependencies of one kind of a package\n"
    "e.g. all Requires or all Conflicts.\n"
    "\n"
    "A dependency set can be instantiated from a header or a tuple:\n"
    "ds(hdr, tag, pool=None)\n"
    "ds((e, v, r), tag, pool=None)\n\n"
    "tag is the dependency type, eg 'requires', 'provides', 'obsoletes' etc"
    "or the corresponding RPMTAG_* value. As a special case, 'nevr' returns"
    "a provide of the header NEVR."
    ;

static PyType_Slot rpmds_Type_Slots[] = {
    {Py_tp_dealloc, rpmds_dealloc},
    {Py_tp_traverse, rpmds_traverse},
    {Py_mp_length, rpmds_length},
    {Py_mp_subscript, rpmds_subscript},
    {Py_tp_getattro, PyObject_GenericGetAttr},
    {Py_tp_setattro, PyObject_GenericSetAttr},
    {Py_tp_doc, rpmds_doc},
    {Py_tp_iter, PyObject_SelfIter},
    {Py_tp_iternext, rpmds_iternext},
    {Py_tp_methods, rpmds_methods},
    {Py_tp_init, rpmds_init},
    {Py_tp_new, rpmds_new},
    {0, NULL},
};
PyType_Spec rpmds_Type_Spec = {
    .name = "rpm.ds",
    .basicsize = sizeof(rpmdsObject),
    .flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_IMMUTABLETYPE,
    .slots = rpmds_Type_Slots,
};

/* ---------- */

rpmds dsFromDs(rpmdsObject * s)
{
    return s->ds;
}

PyObject * rpmds_Wrap(PyTypeObject *subtype, rpmds ds)
{
    allocfunc subtype_alloc = (allocfunc)PyType_GetSlot(subtype, Py_tp_alloc);
    rpmdsObject *s = (rpmdsObject *)subtype_alloc(subtype, 0);
    if (s == NULL) return NULL;

    s->ds = ds;
    s->active = 0;
    return (PyObject *) s;
}

