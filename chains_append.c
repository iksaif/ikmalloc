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

#define LIST_APPEND(list, chunk, name)		\
  do {						\
    chunk->name##_next = NULL;			\
    chunk->name##_prev = list;			\
    if (list)					\
      list->name##_next = chunk;		\
    list = chunk;				\
  } while (0)

void	chunk_append_free(t_chunk_list *list, t_chunk *chunk)
{
  int	i;

  i = free_bucket(chunk->size);
  LIST_APPEND(list->lfree[i], chunk, free);
  chunk->type = MALLOC_CHUNK_FREE;
}

void	chunk_append_main(t_chunk_list *list, t_chunk *chunk)
{
  LIST_APPEND(list->lmain, chunk, main);
}

void	chunk_append(t_chunk_list *list, t_chunk *chunk)
{
  chunk_append_free(list, chunk);
  chunk_append_main(list, chunk);
}
