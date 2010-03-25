
#ifndef __g_cclosure_user_marshal_MARSHAL_H__
#define __g_cclosure_user_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* NONE:INT,INT (marshal.list:1) */
extern void g_cclosure_user_marshal_VOID__INT_INT (GClosure     *closure,
                                                   GValue       *return_value,
                                                   guint         n_param_values,
                                                   const GValue *param_values,
                                                   gpointer      invocation_hint,
                                                   gpointer      marshal_data);
#define g_cclosure_user_marshal_NONE__INT_INT	g_cclosure_user_marshal_VOID__INT_INT

G_END_DECLS

#endif /* __g_cclosure_user_marshal_MARSHAL_H__ */

