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

#include <stdio.h>

#include "malloc.h"

static t_chunk	*search_buckets(t_chunk_list *list, size_t units)
{
  t_chunk	*chunk;
  int		i;

  chunk = NULL;
  i = free_bucket(units);
  while (i <= MALLOC_SHIFT_HIGH && !chunk)
    {
      chunk = list->lfree[i];
      while (chunk && chunk->size < units)
	chunk = chunk->free_prev;
      i++;
    }
  return (chunk);
}

t_chunk		*chunk_search(t_chunk_list *list, size_t units)
{
  t_chunk	*chunk;

  chunk = search_buckets(list, units);
  return (chunk ? chunk : chunk_new(list, units));
}
