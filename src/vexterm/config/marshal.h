
#ifndef __g_cclosure_user_marshal_MARSHAL_H__
#define __g_cclosure_user_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* NONE:STRING,OBJECT (marshal.list:1) */
extern void g_cclosure_user_marshal_VOID__STRING_OBJECT (GClosure     *closure,
                                                         GValue       *return_value,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint,
                                                         gpointer      marshal_data);
#define g_cclosure_user_marshal_NONE__STRING_OBJECT	g_cclosure_user_marshal_VOID__STRING_OBJECT

/* NONE:STRING,STRING (marshal.list:2) */
extern void g_cclosure_user_marshal_VOID__STRING_STRING (GClosure     *closure,
                                                         GValue       *return_value,
                                                         guint         n_param_values,
                                                         const GValue *param_values,
                                                         gpointer      invocation_hint,
                                                         gpointer      marshal_data);
#define g_cclosure_user_marshal_NONE__STRING_STRING	g_cclosure_user_marshal_VOID__STRING_STRING

G_END_DECLS

#endif /* __g_cclosure_user_marshal_MARSHAL_H__ */

