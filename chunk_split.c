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

/*
 * If a chunk size is more than units + 1, we can split it
 * in two part.
 * [hxxxxxxxxxxxx] h = header
 * [hxxxxxxxxxxhx] x = data
 */
#ifndef MALLOC_SPLIT
void
chunk_split(struct malloc_infos *list, struct chunk *chunk, size_t size)
{
  (void) list;
  (void) chunk;
  (void) size;
}
#else
void
chunk_split(struct malloc_infos *list, struct chunk *chunk, size_t size)
{
  struct chunk *new;

  if (chunk->size - size < MALLOC_MINSIZE)
    return;
  new = chunk + size;
  new->size = chunk->size - size;
  chunk->size = size;
  chunk_insert_main(list, chunk, new);
  chunk_append_free(list, new);
}
#endif
