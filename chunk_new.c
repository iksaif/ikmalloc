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

#include <unistd.h>
#include <stdio.h>

#include "malloc.h"

t_chunk		*chunk_new(t_chunk_list *list, size_t units)
{
  t_chunk	*chunk;

  units = MAX(units, MALLOC_MIN_BRK_SIZE);
  chunk = sbrk(units * sizeof (t_chunk));
  if (chunk == (t_chunk *)-1)
    return (NULL);
  chunk->size = units;
  chunk_append(list, chunk);
  return (chunk);
}
