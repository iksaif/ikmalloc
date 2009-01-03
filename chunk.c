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
#include <string.h>
#include <stdio.h>

#include "malloc.h"

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))

static t_chunk_list list = { .needinit = 1 };

static void	lock(void)
{
  while (list.lock);
  list.lock = 1;
}

static void	unlock(void)
{
  list.lock = 0;
}

static size_t	npowof2(size_t size)
{
  size_t	i;

  size--;
  for (i = 1; i < sizeof (size) * 8; i <<= 1)
    size = size | (size >> i);
  return (size + 1);
}

void		*get_chunk(size_t size)
{
  t_chunk	*chunk;
  size_t	units;

  if (list.needinit)
    memset(&list, 0, sizeof (list));
  if (!size)
    return (NULL);
  chunk = NULL;
  units = npowof2(DIV_ROUND_UP(size, sizeof (t_chunk)) + 1);
  lock();
  chunk = chunk_search(&list, units);
  if (chunk)
    {
      chunk_remove_free(&list, chunk);
      chunk_split(&list, chunk, units);
      chunk->asked_size = size;
    }
  unlock();
  return (chunk ? chunk + 1 : NULL);
}

void		free_chunk(void *ptr)
{
  t_chunk	*chunk;

  if (!ptr)
    return ;
  chunk = ((t_chunk *)ptr - 1);
  if (chunk->type != MALLOC_CHUNK_USED)
    {
      fprintf(stderr, "Double free corruption %p\n", ptr);
      return ;
    }
  lock();
  chunk_append_free(&list, chunk);
  chunk_fusion(&list, chunk);
  chunk_give_back(&list);
  unlock();
}

void		show_alloc_mem(void)
{
  t_chunk	*chunk;

  printf("break: %p\n", sbrk(0));
  chunk = list.lmain;
    while (chunk)
      {
	printf("%p - %p", (void *) (chunk + 1),
	       (void *) (chunk->asked_size + (char *) (chunk + 1)));
	printf(" : %zu octets\n", chunk->asked_size);
	chunk = chunk->main_next;
      }
}
