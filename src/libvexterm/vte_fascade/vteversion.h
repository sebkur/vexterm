/*
 * Copyright © 2008 Christian Persch
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02110-1301  USA
 */

#if !defined (__VTE_VTE_H_INSIDE__) && !defined (VTE_COMPILATION)
#error "Only <vte/vte.h> can be included directly."
#endif

#ifndef vte_vteversion_h_included
#define vte_vteversion_h_included

#define VTE_MAJOR_VERSION (0)
#define VTE_MINOR_VERSION (20)
#define VTE_MICRO_VERSION (0)

#define VTE_CHECK_VERSION(major,minor,micro) \
  (VTE_MAJOR_VERSION > (major) || \
   (VTE_MAJOR_VERSION == (major) && VTE_MINOR_VERSION > (minor)) || \
   (VTE_MAJOR_VERSION == (major) && VTE_MINOR_VERSION == (minor) && VTE_MICRO_VERSION >= (micro)))

#endif /* !vte_vteversion_h_included */
