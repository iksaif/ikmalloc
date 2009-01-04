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

struct chunk *
chunk_new(struct malloc_infos *list, size_t units)
{
  struct chunk *chunk;

  units = MAX(units, MALLOC_MIN_BRK_SIZE);
  chunk = sbrk(units * sizeof(struct chunk));
  if (chunk == (struct chunk *) -1)
    return (NULL);
  chunk->size = units;
  chunk_append(list, chunk);
  return (chunk);
}
