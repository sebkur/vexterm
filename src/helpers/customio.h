/* VexTerm - a lightweight and fast terminal emulator
 *
 * Copyright (C) 2010  Sebastian Kuerten
 *
 * This file is part of VexTerm.
 *
 * VexTerm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VexTerm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VexTerm.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * theese are stupid functions
 * use is to convert a double to a string in english notation (e.g. 14.543)
 * and the other way around
 * the reason is, that i did not get setlocale() to work properly with sprintf()
 */

/*
 * the length of the buffer should at least have size of precision+4
 * ['-'] + '(int)d' + '.' + precision
 */
void sprintdouble(char * buf, double d, int precision);

double strtodouble(char * buf);
