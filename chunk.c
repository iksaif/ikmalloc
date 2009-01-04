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

static struct malloc_infos arena = {.needinit = 1 };

static void
lock(void)
{
  pthread_mutex_lock(&arena.mutex);
}

static void
unlock(void)
{
  pthread_mutex_unlock(&arena.mutex);
}

static size_t
npowof2(size_t size)
{
  size_t i;

  size--;
  for (i = 1; i < sizeof(size) * 8; i <<= 1)
    size = size | (size >> i);
  return (size + 1);
}

static void
malloc_init(void)
{
  if (arena.needinit)
    memset(&arena, 0, sizeof(arena));
  pthread_mutex_init(&arena.mutex, NULL);
}

static struct chunk *
mem2chunk(void *p)
{
  return ((struct chunk *) p - 1);
}

static void *
chunk2mem(struct chunk *chunk)
{
  return chunk + 1;
}

void *
get_chunk_aligned(size_t size, size_t alignment)
{
  struct chunk *chunk = NULL, *newp;
  char *p;
  size_t units;

  malloc_init();
  if (alignment <= MALLOC_ALIGNMENT)
    alignment = 0;
  if ((alignment & (alignment - 1)) != 0)
    alignment = npowof2(alignment);
  if (!alignment)
    return get_chunk(size);
  if (!size)
    return NULL;
  units = npowof2(DIV_ROUND_UP(size + MALLOC_MINSIZE + alignment, sizeof(struct chunk)) + 1);
  lock();
  chunk = chunk_search(&arena, units);
  if (chunk)
    {
      chunk_remove_free(&arena, chunk);
      chunk->asked_size = size;
      chunk->alignment = alignment;
    }
  unlock();
  p = chunk2mem(chunk);
  if ((((unsigned long) (p)) % alignment) == 0)
    {
      char *brk = (char *) mem2chunk((void *)(((unsigned long) (p + alignment - 1)) &
					      -((signed long) alignment)));
      if ((unsigned long) (brk - (char *) (p)) < MALLOC_MINSIZE)
	brk += alignment;

      newp = (struct chunk *) brk;
      memmove(newp, chunk, sizeof (struct chunk));
      chunk->size = chunk - newp - 1;
      newp->size = newp->size - chunk->size;
      chunk_insert_main(&arena, newp, chunk);
      chunk_append_free(&arena, chunk);
      chunk = newp;
      p = chunk2mem(chunk);
    }
  chunk_split(&arena, chunk, units);
  return p;
}

/*
 Faire deux gestruct chunk (un aligné et pas l'autre)
 Utiliser likely et unlikely
 Faire mieux les stats
 Faire les macros MALLOC_MIN et MALLOC_ALIGNMENT
 Voir pour size_t trop petit des fois
 Implémenter
 posix_memalign
 strdup
 strndup
 asprintf
 asvprintf
*/
void *
get_chunk(size_t size)
{
  struct chunk *chunk = NULL;
  size_t units;

  malloc_init();
  if (!size)
    return NULL;
  units = npowof2(DIV_ROUND_UP(size, sizeof(struct chunk)) + 1);
  lock();
  chunk = chunk_search(&arena, units);
  if (chunk)
    {
      chunk_remove_free(&arena, chunk);
      chunk_split(&arena, chunk, units);
      chunk->asked_size = size;
      chunk->alignment = 0;
    }
  unlock();
  return chunk2mem(chunk);
}

void
free_chunk(void *ptr)
{
  struct chunk *chunk;

  if (!ptr)
    return;
  chunk = ((struct chunk *) ptr - 1);
  if (chunk->type != MALLOC_CHUNK_USED)
    {
      fprintf(stderr, "Double free corruption %p\n", ptr);
      return;
    }
  lock();
  chunk_append_free(&arena, chunk);
  chunk_fusion(&arena, chunk);
  chunk_give_back(&arena);
  unlock();
}

void
show_alloc_mem(void)
{
  struct chunk *chunk;

  chunk = arena.lmain;
  puts("START\tSIZE\tASKED_SIZE");
  while (chunk)
    {
      printf("%p\t", (void *) (chunk + 1));
      printf("%zu\t%zu\n",
	     chunk->size * sizeof (struct chunk),
	     chunk->asked_size);
      chunk = chunk->main_next;
    }
}
