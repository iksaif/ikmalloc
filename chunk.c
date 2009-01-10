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

static struct malloc_infos arena = {.needinit = 1 };

static void
lock(void)
{
# ifdef MALLOC_USES_SPINLOCK
  pthread_spin_unlock(&arena.mutex);
# else
  pthread_mutex_lock(&arena.mutex);
# endif
}

static void
unlock(void)
{
# ifdef MALLOC_USES_SPINLOCK
  pthread_spin_unlock(&arena.mutex);
# else
  pthread_mutex_unlock(&arena.mutex);
# endif
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
# ifdef MALLOC_USES_SPINLOCK
  pthread_spin_init(&arena.mutex, PTHREAD_PROCESS_SHARED);
# else
  pthread_mutex_init(&arena.mutex, NULL);
# endif
}

static struct chunk *
mem2chunk(void *p)
{
  return (p ? (struct chunk *) p - 1 : NULL);
}

static void *
chunk2mem(struct chunk *chunk)
{
  return (chunk ? chunk + 1 : NULL);
}

void *
get_chunk_aligned(size_t size, size_t alignment)
{
  struct chunk *chunk = NULL, *newp;
  size_t asize = size;
  char *p = NULL;

  malloc_init();
  if (alignment <= MALLOC_ALIGNMENT)
    return get_chunk(size);
  if ((alignment & (alignment - 1)) != 0)
    alignment = npowof2(alignment);
  if (!size)
    return NULL;
  size = npowof2(size + MALLOC_MINSIZE + alignment + sizeof (struct chunk));
  lock();
  chunk = chunk_search(&arena, size);
  if (!chunk)
    goto end;
  chunk_remove_free(&arena, chunk);
  p = chunk2mem(chunk);
  if ((((unsigned long) (p)) % alignment) != 0)
    {
      char *brk = (char *) mem2chunk(align(p, alignment));

      if ((long) (brk - (char *) (p)) < MALLOC_MINSIZE)
	brk += alignment;

      newp = (struct chunk *) brk;
      newp->size = chunk->size - sizeof (struct chunk) - (brk - p);
      newp->type = MALLOC_CHUNK_USED;
      chunk->size = brk - p;
      chunk->alignment = 0;

      chunk_insert_main(&arena, chunk, newp);
      chunk_append_free(&arena, chunk);

      chunk = newp;
      chunk->alignment = alignment;
      chunk->asked_size = asize;
      p = chunk2mem(chunk);
    }
  chunk_split(&arena, chunk, asize);
end:
  unlock();
  return p;
}

void *
get_chunk(size_t size)
{
  struct chunk *chunk = NULL;
  size_t asize = size;

  malloc_init();
  if (!size)
    return NULL;
  size = npowof2(size);
  lock();
  chunk = chunk_search(&arena, size);
  if (chunk)
    {
      chunk_remove_free(&arena, chunk);
      chunk_split(&arena, chunk, size);
      chunk->asked_size = asize;
      chunk->alignment = MALLOC_ALIGNMENT;
    }
  unlock();
  return chunk2mem(chunk);
}

void
free_chunk(void *ptr)
{
  struct chunk *chunk;

  malloc_init();
  if (!ptr)
    return;
  chunk = ((struct chunk *) ptr - 1);
  if (chunk->type != MALLOC_CHUNK_USED)
    {
      fprintf(stderr, "Double free corruption %p %#zx\n", ptr, chunk->type);
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
  puts("START\t\tSIZE\tASIZE\tALIGN\tTYPE");
  while (chunk)
    {
      printf("%p\t", (void *) chunk);
      printf("%zu\t%zu\t%zu\t", chunk->size,
	     chunk->asked_size, chunk->alignment);
      if (chunk->type == MALLOC_CHUNK_FREE)
	puts("free");
      else if (chunk->type == MALLOC_CHUNK_USED)
	puts("used");
      else
	printf("bad (%zx)\n", chunk->type);
      chunk = chunk->main_prev;
    }
  for (int i = MALLOC_SHIFT_LOW; i <= MALLOC_SHIFT_HIGH; ++i)
    printf("%6d : %zu\n", 1 << i, arena.lfree_cnt[i]);
}
