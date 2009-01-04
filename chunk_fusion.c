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

#ifndef MALLOC_SPLIT
void
chunk_fusion(struct malloc_infos *list, struct chunk *chunk)
{
  (void) list;
  (void) chunk;
}
#else
static void
_chunk_fusion(struct malloc_infos *list, struct chunk *prev, struct chunk *next)
{
  if (next == prev + prev->size)
    {
      chunk_remove_free(list, prev);
      chunk_remove(list, next);
      prev->size += next->size;
      chunk_append_free(list, prev);
    }
}

void
chunk_fusion(struct malloc_infos *list, struct chunk *chunk)
{
  struct chunk *next;
  struct chunk *prev;

  next = chunk->main_next;
  prev = chunk->main_prev;
  if (next && next->type == MALLOC_CHUNK_FREE)
    _chunk_fusion(list, chunk, next);
  if (prev && prev->type == MALLOC_CHUNK_FREE)
    _chunk_fusion(list, prev, chunk);
}
#endif
