/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
/*
 * Copyright (c) 2009  litl, LLC
 * Copyright (c) 2010  Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#if !defined (__GJS_GJS_MODULE_H__) && !defined (GJS_COMPILATION)
#error "Only <gjs/gjs-module.h> can be included directly."
#endif

#ifndef __GJS_COMPAT_H__
#define __GJS_COMPAT_H__

#include <jsapi.h>
#include <glib.h>

G_BEGIN_DECLS

/* This file inspects jsapi.h and attempts to provide a compatibility shim.
 * See https://bugzilla.gnome.org/show_bug.cgi?id=622896 for some initial discussion.
 */

/* The old JS_AddRoot accepted anything via void *, new
 * api is stricter.
 * Upstream commit 2fc2a12a4565, Spidermonkey >= Jun 07 2010
 */
#ifndef JS_TYPED_ROOTING_API
#define JS_AddValueRoot JS_AddRoot
#define JS_AddObjectRoot JS_AddRoot
#define JS_AddStringRoot JS_AddRoot
#define JS_AddGCThingRoot JS_AddRoot
#define JS_RemoveValueRoot JS_RemoveRoot
#define JS_RemoveObjectRoot JS_RemoveRoot
#define JS_RemoveStringRoot JS_RemoveRoot
#define JS_RemoveGCThingRoot JS_RemoveRoot
#endif

/* commit 5ad4532aa996, Spidermonkey > Jun 17 2010
 * This one is complex; jsid appears to be explicitly
 * different from JSVAL now.  If we're on an old xulrunner,
 * define JSID_IS_VOID in a compatible way.
 */
#ifndef JSID_VOID
#define JSID_VOID JSVAL_VOID
#define JSID_IS_VOID(id) (id == JSVAL_VOID)
#define INT_TO_JSID(i) ((jsid) INT_TO_JSVAL(i))
#endif

/* commit 66c8ad02543b, Spidermonkey > Aug 16 2010
 * "Slow natives" */
#ifdef JSFUN_CONSTRUCTOR
/* All functions are "fast", so define this to a no-op */
#define JSFUN_FAST_NATIVE 0

/**
 * GJS_NATIVE_CONSTRUCTOR_DECLARE:
 * Prototype a constructor.
 */
#define GJS_NATIVE_CONSTRUCTOR_DECLARE(name)            \
static JSBool                                           \
gjs_##name##_constructor(JSContext  *context,           \
                         uintN       argc,              \
                         jsval      *vp)

/**
 * GJS_NATIVE_CONSTRUCTOR_VARIABLES:
 * Declare variables necessary for the constructor; should
 * be at the very top.
 */
#define GJS_NATIVE_CONSTRUCTOR_VARIABLES(name)          \
    JSObject *object = NULL;                            \
    jsval *argv = JS_ARGV(context, vp);

/**
 * GJS_NATIVE_CONSTRUCTOR_PRELUDE:
 * Call after the initial variable declaration.
 */
#define GJS_NATIVE_CONSTRUCTOR_PRELUDE(name)                                         \
    {                                                                                \
        if (!JS_IsConstructing_PossiblyWithGivenThisObject(context, vp, &object)) {  \
            gjs_throw_constructor_error(context);                                    \
            return JS_FALSE;                                                         \
        }                                                                            \
        if (object == NULL)                                                          \
            object = JS_NewObjectForConstructor(context, vp);                        \
        if (object == NULL)                                                          \
            return JS_FALSE;                                                         \
    }


/**
 * GJS_NATIVE_CONSTRUCTOR_FINISH:
 * Call this at the end of a constructor when it's completed
 * successfully.
 */
#define GJS_NATIVE_CONSTRUCTOR_FINISH(name)             \
    JS_SET_RVAL(context, vp, OBJECT_TO_JSVAL(object));
#else

#define GJS_NATIVE_CONSTRUCTOR_DECLARE(name)            \
static JSBool                                           \
gjs_##name##_constructor(JSContext *context,            \
                         JSObject  *object,             \
                         uintN      argc,               \
                         jsval     *argv,               \
                         jsval     *retval)

#define GJS_NATIVE_CONSTRUCTOR_VARIABLES(name)

#define GJS_NATIVE_CONSTRUCTOR_PRELUDE(name)            \
    if (!JS_IsConstructing(context)) {                  \
        gjs_throw_constructor_error(context);           \
        return JS_FALSE;                                \
    }

#define GJS_NATIVE_CONSTRUCTOR_FINISH(name)

#endif

#ifndef HAVE_JS_STRICTPROPERTYSTUB
#define JS_StrictPropertyStub JS_PropertyStub
#endif

#ifndef HAVE_JS_GETGLOBALFORSCOPECHAIN
#define JS_GetGlobalForScopeChain JS_GetScopeChain
#endif

G_END_DECLS

#endif  /* __GJS_COMPAT_H__ */
