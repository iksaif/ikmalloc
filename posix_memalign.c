/* Copyright (C) 2007-2008 Chary Corentin.
 *  This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <errno.h>

#include "malloc.h"

int
posix_memalign(void **retptr, size_t boundary, size_t size)
{
  *retptr = NULL;
  if ((boundary & (boundary - 1)) != 0 || (boundary % sizeof(void *)) != 0)
    return EINVAL;
  if (!size)
    return 0;
  *retptr = get_chunk_aligned(size, boundary);
  if (!*retptr)
    return ENOMEM;
  return 0;
}
