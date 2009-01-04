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

#include "malloc.h"

#define LIST_INSERT(list, pos, chunk, name)		\
  do {							\
    chunk->name##_next = pos->name##_next;		\
    chunk->name##_prev = pos;				\
    if (pos->name##_next)				\
      pos->name##_next->name##_prev = chunk;		\
    pos->name##_next = chunk;				\
    if (list == pos)					\
      list = chunk;					\
  } while (0)

void
chunk_insert_free(struct malloc_infos *list, struct chunk *pos,
		  struct chunk *chunk)
{
  int i;

  i = free_bucket(chunk->size);
  LIST_INSERT(list->lfree[i], pos, chunk, free);
  chunk->type = MALLOC_CHUNK_FREE;
}

void
chunk_insert_main(struct malloc_infos *list, struct chunk *pos,
		  struct chunk *chunk)
{
  LIST_INSERT(list->lmain, pos, chunk, main);
}
